#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <dlfcn.h>
#include <map>
#include <mutex>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#if defined(__ANDROID__) && defined(__aarch64__)
#include <unwind.h>
#endif
#include <dobby.h>
#include "json.hpp"

static std::atomic<bool> g_freeze{false};
static std::atomic<uint64_t> g_freeze_blocked_destroy{0};
static std::atomic<uint64_t> g_freeze_blocked_scene{0};
static thread_local int t_freeze_bypass = 0;

struct freeze_bypass_guard {
    freeze_bypass_guard() { t_freeze_bypass++; }
    ~freeze_bypass_guard() { t_freeze_bypass--; }
};

static inline bool freeze_active() {
    return g_freeze.load(std::memory_order_relaxed) && t_freeze_bypass == 0;
}

struct freeze_track_entry {
    std::string caller;
    std::vector<std::string> caller_stack;
    std::vector<std::string> managed_stack;
    std::vector<std::string> native_stack;
    std::string action;
    std::string target;
    bool in_scene = false;
    uint64_t count = 0;
};

static std::mutex g_fz_track_mtx;
static std::map<std::string, freeze_track_entry> g_fz_tracks;

struct fz_method_symbol {
    uintptr_t start = 0;
    uintptr_t module_base = 0;
    std::string label;
};

static std::mutex g_fz_sym_mtx;
static std::vector<fz_method_symbol> g_fz_syms;
static bool g_fz_syms_built = false;

static std::string fz_image_name(BNM::IL2CPP::Il2CppClass* klass) {
    if (!klass || !klass->image || !klass->image->name) return "unknown";

    std::string name = klass->image->name;
    const size_t dll = name.rfind(".dll");
    if (dll != std::string::npos) name.resize(dll);
    return name.empty() ? "unknown" : name;
}

static bool fz_build_symbol_table(bool force_rebuild = false) {
    {
        std::lock_guard<std::mutex> lk(g_fz_sym_mtx);
        if (g_fz_syms_built && !force_rebuild) return !g_fz_syms.empty();
    }

    std::vector<fz_method_symbol> fresh;

    try {
        BNM::AttachIl2Cpp();
        const auto images = BNM::Image::GetImages();
        if (images.empty()) return false;

        for (const auto& img : images) {
            if (!img.IsValid()) continue;

            try {
                for (const auto& cls : img.GetClasses(true)) {
                    if (!cls.IsValid()) continue;

                    auto* klass = cls.GetClass();
                    if (!klass || !klass->name) continue;

                    const std::string assembly = fz_image_name(klass);
                    const std::string namespaze =
                            (klass->namespaze && klass->namespaze[0] != '\0')
                            ? klass->namespaze
                            : "global";
                    const std::string class_name = klass->name;

                    for (const auto& method : cls.GetMethods(false)) {
                        auto* mi = method.GetInfo();
                        if (!mi || !mi->methodPointer || !mi->name) continue;

                        fz_method_symbol symbol;
                        symbol.start = reinterpret_cast<uintptr_t>(mi->methodPointer);
                        Dl_info method_info{};
                        if (dladdr(reinterpret_cast<void*>(mi->methodPointer), &method_info) != 0 &&
                            method_info.dli_fbase) {
                            symbol.module_base =
                                    reinterpret_cast<uintptr_t>(method_info.dli_fbase);
                        }
                        symbol.label = assembly + ":" + namespaze + ":" +
                                       class_name + "." + mi->name;
                        fresh.push_back(std::move(symbol));
                    }
                }
            } catch (...) {
                continue;
            }
        }
    } catch (...) {
        return false;
    }

    if (fresh.empty()) return false;

    std::sort(fresh.begin(), fresh.end(), [](const fz_method_symbol& a,
                                             const fz_method_symbol& b) {
        if (a.start != b.start) return a.start < b.start;
        if (a.module_base != b.module_base) return a.module_base < b.module_base;
        return a.label < b.label;
    });

    fresh.erase(std::unique(fresh.begin(), fresh.end(),
                            [](const fz_method_symbol& a, const fz_method_symbol& b) {
                                return a.start == b.start && a.module_base == b.module_base &&
                                       a.label == b.label;
                            }), fresh.end());

    {
        std::lock_guard<std::mutex> lk(g_fz_sym_mtx);
        g_fz_syms.swap(fresh);
        g_fz_syms_built = !g_fz_syms.empty();
    }

    return true;
}

static std::string fz_resolve_symbol(void* ret_addr) {
    if (!ret_addr) return "native";

    fz_build_symbol_table();
    const uintptr_t addr = reinterpret_cast<uintptr_t>(ret_addr);

    std::string candidate;
    Dl_info caller_info{};
    const bool caller_module = dladdr(ret_addr, &caller_info) != 0 &&
                               caller_info.dli_fbase != nullptr;
    const uintptr_t caller_base = caller_module
                                  ? reinterpret_cast<uintptr_t>(caller_info.dli_fbase)
                                  : 0;

    {
        std::lock_guard<std::mutex> lk(g_fz_sym_mtx);
        if (caller_module && !g_fz_syms.empty()) {
            auto it = std::upper_bound(
                    g_fz_syms.begin(), g_fz_syms.end(), addr,
                    [](uintptr_t value, const fz_method_symbol& symbol) {
                        return value < symbol.start;
                    });

            while (it != g_fz_syms.begin()) {
                --it;
                if (it->start > addr || it->module_base != caller_base) continue;

                const uintptr_t method_start = it->start;
                const uintptr_t diff = addr - method_start;
                auto first = std::lower_bound(
                        g_fz_syms.begin(), g_fz_syms.end(), method_start,
                        [](const fz_method_symbol& symbol, uintptr_t value) {
                            return symbol.start < value;
                        });

                std::string labels;
                size_t aliases = 0;
                for (auto cur = first;
                     cur != g_fz_syms.end() && cur->start == method_start; ++cur) {
                    if (cur->module_base != caller_base) continue;
                    if (!labels.empty()) labels += " | ";
                    labels += cur->label;
                    ++aliases;
                }

                char offset[48]{};
                std::snprintf(offset, sizeof(offset), "+0x%llx",
                              static_cast<unsigned long long>(diff));

                if (aliases > 1)
                    candidate = "[nearest/shared-entry]{" + labels + "}" + offset;
                else if (aliases == 1)
                    candidate = "[nearest-entry]" + labels + offset;
                break;
            }
        }
    }

    if (!candidate.empty()) return candidate;

    Dl_info info{};
    if (dladdr(ret_addr, &info) != 0 && info.dli_fname && info.dli_fbase) {
        const char* base_name = std::strrchr(info.dli_fname, '/');
        base_name = base_name ? base_name + 1 : info.dli_fname;

        char buffer[192]{};
        const uintptr_t offset = addr - reinterpret_cast<uintptr_t>(info.dli_fbase);
        std::snprintf(buffer, sizeof(buffer), "%s+0x%llx", base_name,
                      static_cast<unsigned long long>(offset));
        return buffer;
    }

    return "native";
}

static bool fz_is_runtime_frame(const std::string& frame) {
    return frame.rfind("System.", 0) == 0 ||
           frame.rfind("UnityEngine.", 0) == 0 ||
           frame.rfind("UnityEngineInternal.", 0) == 0 ||
           frame.rfind("Microsoft.", 0) == 0 ||
           frame.rfind("Mono.", 0) == 0 ||
           frame.rfind("BNM.", 0) == 0;
}

static std::vector<std::string> fz_extract_managed_stack() {
    std::vector<std::string> frames;
    freeze_bypass_guard guard;

    try {
        BNM::AttachIl2Cpp();
        BNM::Class env("System", "Environment");
        if (!env.IsValid()) return frames;

        BNM::Method<BNM::Structures::Mono::String*> get_stack = env.GetMethod("get_StackTrace");
        if (!get_stack.IsValid()) return frames;

        auto* stack_string = get_stack();
        if (!stack_string) return frames;

        const std::string stack = stack_string->str();
        size_t pos = 0;

        while (pos < stack.size() && frames.size() < 32) {
            const size_t eol = stack.find('\n', pos);
            const size_t line_end = eol == std::string::npos ? stack.size() : eol;
            std::string line = stack.substr(pos, line_end - pos);
            pos = eol == std::string::npos ? stack.size() : eol + 1;

            const size_t at = line.find("at ");
            if (at == std::string::npos) continue;

            std::string frame = line.substr(at + 3);
            const size_t paren = frame.find('(');
            if (paren != std::string::npos) frame.resize(paren);

            while (!frame.empty() &&
                   (frame.back() == ' ' || frame.back() == '\r' || frame.back() == '\t')) {
                frame.pop_back();
            }

            while (!frame.empty() && (frame.front() == ' ' || frame.front() == '\t')) {
                frame.erase(frame.begin());
            }

            if (frame.empty() || fz_is_runtime_frame(frame)) continue;

            std::string resolved;
            const size_t method_dot = frame.rfind('.');
            if (method_dot != std::string::npos && method_dot > 0 && method_dot + 1 < frame.size()) {
                const std::string full_class = frame.substr(0, method_dot);
                const std::string method_name = frame.substr(method_dot + 1);
                const size_t namespace_dot = full_class.rfind('.');

                const std::string namespaze = namespace_dot == std::string::npos
                                              ? "global"
                                              : full_class.substr(0, namespace_dot);
                const std::string class_name = namespace_dot == std::string::npos
                                               ? full_class
                                               : full_class.substr(namespace_dot + 1);

                BNM::Class klass(namespaze == "global" ? "" : namespaze, class_name);
                if (klass.IsValid() && klass.GetClass()) {
                    resolved = fz_image_name(klass.GetClass()) + ":" + namespaze + ":" +
                               class_name + "." + method_name + " [managed_stack]";
                }
            }

            if (resolved.empty()) resolved = "[managed_unresolved]" + frame;
            if (std::find(frames.begin(), frames.end(), resolved) == frames.end()) {
                frames.push_back(std::move(resolved));
            }
        }
    } catch (...) {
    }

    return frames;
}

#if defined(__ANDROID__) && defined(__aarch64__)
struct fz_unwind_state {
    std::vector<uintptr_t>* frames;
};

static _Unwind_Reason_Code fz_unwind_callback(_Unwind_Context* context, void* user_data) {
    auto* state = static_cast<fz_unwind_state*>(user_data);
    if (!state || !state->frames) return _URC_END_OF_STACK;

    const uintptr_t ip = static_cast<uintptr_t>(_Unwind_GetIP(context));
    if (ip) state->frames->push_back(ip);

    return state->frames->size() >= 64 ? _URC_END_OF_STACK : _URC_NO_REASON;
}

static std::vector<std::string> fz_extract_native_stack(void* ret_addr) {
    std::vector<std::string> frames;
    std::vector<uintptr_t> addresses;

    if (ret_addr) addresses.push_back(reinterpret_cast<uintptr_t>(ret_addr));

    fz_unwind_state state{&addresses};
    _Unwind_Backtrace(fz_unwind_callback, &state);

    std::unordered_set<std::string> seen;
    for (uintptr_t address : addresses) {
        const std::string symbol = fz_resolve_symbol(reinterpret_cast<void*>(address));
        if (symbol == "native") continue;
        if (symbol.rfind("[nearest-entry]", 0) != 0 &&
            symbol.rfind("[nearest/shared-entry]", 0) != 0) {
            continue;
        }
        if (!seen.insert(symbol).second) continue;
        frames.push_back(symbol);
        if (frames.size() >= 24) break;
    }

    return frames;
}
#else
static std::vector<std::string> fz_extract_native_stack(void*) {
    return {};
}
#endif

static std::string fz_join_stack(const std::vector<std::string>& frames, const char* separator) {
    std::string result;
    for (const auto& frame : frames) {
        if (frame.empty()) continue;
        if (!result.empty()) result += separator;
        result += frame;
    }
    return result;
}

struct fz_target_info {
    std::string desc = "none";
    bool in_scene = false;
};

static fz_target_info fz_inspect_target(uint64_t obj_ptr, bool has_obj) {
    fz_target_info out;
    if (!has_obj || !obj_ptr) return out;
    freeze_bypass_guard guard;
    BNM::AttachIl2Cpp();
    try {
        auto* obj = (BNM::IL2CPP::Il2CppObject*)obj_ptr;
        if (!is_valid_pointer(obj) || !is_valid_pointer(obj->klass)) return out;
        Il2CppClass* k = obj->klass;
        std::string cls_name = (k->name && is_valid_pointer(k->name)) ? k->name : "Object";
        if (k->namespaze && is_valid_pointer(k->namespaze) && k->namespaze[0] != '\0') {
            cls_name = std::string(k->namespaze) + "." + cls_name;
        }

        std::string obj_name;
        BNM::Class oc("UnityEngine", "Object");
        if (oc.IsValid()) {
            BNM::Method<BNM::Structures::Mono::String*> get_name = oc.GetMethod("get_name");
            if (get_name.IsValid()) {
                auto* s = get_name[obj]();
                if (s) obj_name = s->str();
            }
        }

        BNM::IL2CPP::Il2CppObject* go_obj = nullptr;
        BNM::Class go_cls("UnityEngine", "GameObject");
        BNM::Class comp_cls("UnityEngine", "Component");
        if (go_cls.IsValid() && k == go_cls.GetClass()) {
            go_obj = obj;
        } else if (comp_cls.IsValid()) {
            for (Il2CppClass* cur = k; cur && is_valid_pointer(cur); cur = cur->parent) {
                if (cur == comp_cls.GetClass()) {
                    BNM::Method<BNM::IL2CPP::Il2CppObject*> get_go = comp_cls.GetMethod("get_gameObject");
                    if (get_go.IsValid()) go_obj = get_go[obj]();
                    break;
                }
            }
        }

        std::string scene_label = "off-scene";
        if (go_obj && is_valid_pointer(go_obj) && go_cls.IsValid()) {
            BNM::Method<int> get_handle = go_cls.GetMethod("get_sceneCullingMask");
            BNM::Method<bool> get_active_hier = go_cls.GetMethod("get_activeInHierarchy");
            bool active_hier = get_active_hier.IsValid() ? get_active_hier[go_obj]() : false;

            BNM::Class tr_cls("UnityEngine", "Transform");
            BNM::Method<BNM::IL2CPP::Il2CppObject*> get_tr = go_cls.GetMethod("get_transform");
            BNM::IL2CPP::Il2CppObject* tr = get_tr.IsValid() ? get_tr[go_obj]() : nullptr;
            std::string path = obj_name;
            if (tr && is_valid_pointer(tr) && tr_cls.IsValid()) {
                BNM::Method<BNM::IL2CPP::Il2CppObject*> get_parent = tr_cls.GetMethod("get_parent");
                BNM::Method<BNM::Structures::Mono::String*> tr_name = oc.GetMethod("get_name");
                BNM::IL2CPP::Il2CppObject* cur_tr = get_parent.IsValid() ? get_parent[tr]() : nullptr;
                int depth = 0;
                while (cur_tr && is_valid_pointer(cur_tr) && depth < 6) {
                    auto* ps = tr_name.IsValid() ? tr_name[cur_tr]() : nullptr;
                    if (ps) path = ps->str() + "/" + path;
                    cur_tr = get_parent[cur_tr]();
                    depth++;
                }
            }

            out.in_scene = active_hier || (tr != nullptr);
            scene_label = out.in_scene ? (active_hier ? "in-scene:active" : "in-scene:inactive") : "off-scene";
            out.desc = cls_name + " \"" + path + "\" [" + scene_label + "]";
            return out;
        }

        out.desc = obj_name.empty() ? cls_name : (cls_name + " \"" + obj_name + "\" [asset/non-scene]");
    } catch (...) {
    }
    return out;
}

static void fz_record_attempt(const char* action, void* ret_addr, uint64_t obj_ptr, bool has_obj) {
    std::vector<std::string> managed_stack = fz_extract_managed_stack();
    std::vector<std::string> native_stack = fz_extract_native_stack(ret_addr);
    std::vector<std::string> caller_stack = managed_stack;

    for (const auto& frame : native_stack) {
        if (std::find(caller_stack.begin(), caller_stack.end(), frame) == caller_stack.end()) {
            caller_stack.push_back("[native_stack]" + frame);
        }
    }

    if (caller_stack.empty()) {
        const std::string fallback = fz_resolve_symbol(ret_addr);
        if (fallback != "native") caller_stack.push_back(fallback);
    }

    const std::string caller = fz_join_stack(caller_stack, " <- ");
    std::string act = action ? action : "unknown";
    fz_target_info tinfo = fz_inspect_target(obj_ptr, has_obj);
    const std::string key = caller + "|" + act + "|" + tinfo.desc;

    std::lock_guard<std::mutex> lk(g_fz_track_mtx);
    auto& entry = g_fz_tracks[key];
    if (entry.count == 0) {
        entry.caller = caller;
        entry.caller_stack = std::move(caller_stack);
        entry.managed_stack = std::move(managed_stack);
        entry.native_stack = std::move(native_stack);
        entry.action = std::move(act);
        entry.target = std::move(tinfo.desc);
        entry.in_scene = tinfo.in_scene;
    }
    entry.count++;
}

#if defined(__aarch64__)

enum class freeze_kind {
    block,
    block_destroy,
    defer_async,
    ignore_true,
    ignore_false,
    scene_internal,
    unload_internal
};

static const int freeze_slots = 48;
static uint64_t (*g_fz_orig[freeze_slots])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = {};
static freeze_kind g_fz_kind[freeze_slots] = {};
static int g_fz_complete_reg[freeze_slots] = {};
static const char* g_fz_name[freeze_slots] = {};
static bool g_fz_has_obj[freeze_slots] = {};
static int g_fz_used = 0;
static std::unordered_set<void*> g_fz_hooked;
static std::mutex g_fz_mtx;
static bool g_fz_installed = false;
static std::string g_fz_error;
static int g_fz_hook_count = 0;

static void fz_force_no_activation(uint64_t op) {
    if (!op) return;
    try {
        BNM::Class ao("UnityEngine", "AsyncOperation");
        if (!ao.IsValid()) return;
        BNM::Method<void> setter = ao.GetMethod("set_allowSceneActivation");
        if (setter.IsValid()) {
            freeze_bypass_guard guard;
            setter[(BNM::IL2CPP::Il2CppObject*)op](false);
        }
    } catch (...) {
    }
}

template <int N>
static uint64_t fz_hook(uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7) {
    void* ret_addr = __builtin_return_address(0);
    auto orig = g_fz_orig[N];
    if (freeze_active()) {
        switch (g_fz_kind[N]) {
            case freeze_kind::block:
                g_freeze_blocked_scene.fetch_add(1, std::memory_order_relaxed);
                fz_record_attempt(g_fz_name[N], ret_addr, a0, g_fz_has_obj[N]);
                return 0;
            case freeze_kind::block_destroy:
                g_freeze_blocked_destroy.fetch_add(1, std::memory_order_relaxed);
                fz_record_attempt(g_fz_name[N], ret_addr, a0, g_fz_has_obj[N]);
                return 0;
            case freeze_kind::ignore_true:
                if (a1 & 1) {
                    g_freeze_blocked_scene.fetch_add(1, std::memory_order_relaxed);
                    fz_record_attempt(g_fz_name[N], ret_addr, a0, g_fz_has_obj[N]);
                    return 0;
                }
                break;
            case freeze_kind::ignore_false:
                if ((a1 & 1) == 0) {
                    g_freeze_blocked_destroy.fetch_add(1, std::memory_order_relaxed);
                    fz_record_attempt(g_fz_name[N], ret_addr, a0, g_fz_has_obj[N]);
                    return 0;
                }
                break;
            case freeze_kind::scene_internal: {
                uint64_t regs[8] = {a0, a1, a2, a3, a4, a5, a6, a7};
                int r = g_fz_complete_reg[N];
                g_freeze_blocked_scene.fetch_add(1, std::memory_order_relaxed);
                fz_record_attempt(g_fz_name[N], ret_addr, 0, false);
                if (r >= 0 && r < 8 && (regs[r] & 1)) return 0;
                uint64_t res = orig ? orig(a0, a1, a2, a3, a4, a5, a6, a7) : 0;
                fz_force_no_activation(res);
                return res;
            }
            case freeze_kind::unload_internal: {
                uint64_t regs[8] = {a0, a1, a2, a3, a4, a5, a6, a7};
                int r = g_fz_complete_reg[N];
                g_freeze_blocked_scene.fetch_add(1, std::memory_order_relaxed);
                fz_record_attempt(g_fz_name[N], ret_addr, 0, false);
                if (r >= 0 && r < 8 && (regs[r] & 1)) return 0;
                return 0;
            }
            case freeze_kind::defer_async: {
                g_freeze_blocked_scene.fetch_add(1, std::memory_order_relaxed);
                fz_record_attempt(g_fz_name[N], ret_addr, 0, false);
                uint64_t res = orig ? orig(a0, a1, a2, a3, a4, a5, a6, a7) : 0;
                fz_force_no_activation(res);
                return res;
            }
        }
    }
    return orig ? orig(a0, a1, a2, a3, a4, a5, a6, a7) : 0;
}

using fz_fn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

template <int... I>
static constexpr std::array<fz_fn, sizeof...(I)> fz_make_table(std::integer_sequence<int, I...>) {
    return {{fz_hook<I>...}};
}

static const std::array<fz_fn, freeze_slots> g_fz_fns = fz_make_table(std::make_integer_sequence<int, freeze_slots>{});

static bool fz_hook_generic(BNM::IL2CPP::MethodInfo* mi, freeze_kind kind, int complete_reg, const char* label, bool has_obj = false) {
    if (!mi || !mi->methodPointer || mi->is_generic) return false;
    void* addr = (void*)mi->methodPointer;
    if (g_fz_hooked.count(addr) || g_fz_used >= freeze_slots) return false;
    int slot = g_fz_used;
    g_fz_kind[slot] = kind;
    g_fz_complete_reg[slot] = complete_reg;
    g_fz_name[slot] = label;
    g_fz_has_obj[slot] = has_obj;
    int rc = DobbyHook(addr, (dobby_dummy_func_t)g_fz_fns[slot], (dobby_dummy_func_t*)&g_fz_orig[slot]);
    if (rc != RT_SUCCESS) return false;
    g_fz_used++;
    g_fz_hooked.insert(addr);
    g_fz_hook_count++;
    return true;
}

static bool freeze_install(std::string& error) {
    std::lock_guard<std::mutex> lk(g_fz_mtx);
    if (g_fz_installed) return true;
    BNM::AttachIl2Cpp();
    fz_build_symbol_table(true);
    int destroy_hooks = 0;
    try {
        BNM::Class oc("UnityEngine", "Object");
        if (oc.IsValid()) {
            for (auto& m : oc.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (!mi || !mi->name || !mi->methodPointer || mi->is_generic) continue;
                if (strcmp(mi->name, "Destroy") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block_destroy, -1, "Object.Destroy", true)) destroy_hooks++;
                } else if (strcmp(mi->name, "DestroyImmediate") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block_destroy, -1, "Object.DestroyImmediate", true)) destroy_hooks++;
                } else if (strcmp(mi->name, "DestroyObject") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block_destroy, -1, "Object.DestroyObject", true)) destroy_hooks++;
                }
            }
        }
    } catch (...) {
    }
    try {
        BNM::Class go("UnityEngine", "GameObject");
        if (go.IsValid()) {
            for (auto& m : go.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (!mi || !mi->name || !mi->methodPointer || mi->is_generic) continue;
                if (strcmp(mi->name, "SetActive") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::ignore_false, -1, "GameObject.SetActive(false)", true)) destroy_hooks++;
                } else if (strcmp(mi->name, "SetActiveRecursively") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::ignore_false, -1, "GameObject.SetActiveRecursively(false)", true)) destroy_hooks++;
                }
            }
        }
    } catch (...) {
    }
    try {
        BNM::Class tr("UnityEngine", "Transform");
        if (tr.IsValid()) {
            for (auto& m : tr.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (!mi || !mi->name || !mi->methodPointer || mi->is_generic) continue;
                if (strcmp(mi->name, "DetachChildren") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block_destroy, -1, "Transform.DetachChildren", true)) destroy_hooks++;
                }
            }
        }
    } catch (...) {
    }
    try {
        BNM::Class res("UnityEngine", "Resources");
        if (res.IsValid()) {
            for (auto& m : res.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (!mi || !mi->name || !mi->methodPointer || mi->is_generic) continue;
                if (strcmp(mi->name, "UnloadAsset") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block_destroy, -1, "Resources.UnloadAsset", true)) destroy_hooks++;
                } else if (strcmp(mi->name, "UnloadUnusedAssets") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block_destroy, -1, "Resources.UnloadUnusedAssets", false)) destroy_hooks++;
                }
            }
        }
    } catch (...) {
    }
    try {
        BNM::Class ab("UnityEngine", "AssetBundle");
        if (ab.IsValid()) {
            for (auto& m : ab.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (!mi || !mi->name || !mi->methodPointer || mi->is_generic) continue;
                if (strcmp(mi->name, "Unload") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block_destroy, -1, "AssetBundle.Unload", true)) destroy_hooks++;
                } else if (strcmp(mi->name, "UnloadAsync") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block_destroy, -1, "AssetBundle.UnloadAsync", true)) destroy_hooks++;
                } else if (strcmp(mi->name, "UnloadAllAssetBundles") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block_destroy, -1, "AssetBundle.UnloadAllAssetBundles", false)) destroy_hooks++;
                }
            }
        }
    } catch (...) {
    }
    int scene_hooks = 0;
    try {
        BNM::Class sm("UnityEngine.SceneManagement", "SceneManager");
        if (sm.IsValid()) {
            for (auto& m : sm.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (!mi || !mi->name || !mi->methodPointer || mi->is_generic) continue;
                if (strcmp(mi->name, "LoadSceneAsyncNameIndexInternal") == 0) {
                    int reg = 3;
                    for (int i = 0; i < (int)mi->parameters_count && i < 8; i++)
                        if (get_param_name(mi, i) == "mustCompleteNextFrame") reg = i;
                    if (fz_hook_generic(mi, freeze_kind::scene_internal, reg, "SceneManager.LoadScene")) scene_hooks++;
                } else if (strcmp(mi->name, "UnloadSceneNameIndexInternal") == 0 ||
                           strcmp(mi->name, "UnloadSceneAsyncInternal") == 0) {
                    int reg = 2;
                    for (int i = 0; i < (int)mi->parameters_count && i < 8; i++)
                        if (get_param_name(mi, i) == "immediately") reg = i;
                    if (fz_hook_generic(mi, freeze_kind::unload_internal, reg, "SceneManager.UnloadScene")) scene_hooks++;
                } else if (strcmp(mi->name, "LoadScene") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block, -1, "SceneManager.LoadScene")) scene_hooks++;
                } else if (strcmp(mi->name, "UnloadScene") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block, -1, "SceneManager.UnloadScene")) scene_hooks++;
                } else if (strcmp(mi->name, "UnloadSceneAsync") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block, -1, "SceneManager.UnloadSceneAsync")) scene_hooks++;
                } else if (strcmp(mi->name, "MoveGameObjectToScene") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block, -1, "SceneManager.MoveGameObjectToScene", true)) scene_hooks++;
                } else if (strcmp(mi->name, "MoveGameObjectsToScene") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block, -1, "SceneManager.MoveGameObjectsToScene")) scene_hooks++;
                } else if (strcmp(mi->name, "MergeScenes") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::block, -1, "SceneManager.MergeScenes")) scene_hooks++;
                } else if (strcmp(mi->name, "LoadSceneAsync") == 0) {
                    if (fz_hook_generic(mi, freeze_kind::defer_async, -1, "SceneManager.LoadSceneAsync")) scene_hooks++;
                }
            }
        }
        BNM::Class sm_api("UnityEngine.SceneManagement", "SceneManagerAPIInternal");
        if (sm_api.IsValid()) {
            for (auto& m : sm_api.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (!mi || !mi->name || !mi->methodPointer || mi->is_generic) continue;
                if (strcmp(mi->name, "LoadSceneAsyncNameIndexInternal") == 0) {
                    int reg = 3;
                    for (int i = 0; i < (int)mi->parameters_count && i < 8; i++)
                        if (get_param_name(mi, i) == "mustCompleteNextFrame") reg = i;
                    if (fz_hook_generic(mi, freeze_kind::scene_internal, reg, "SceneManager.LoadScene")) scene_hooks++;
                } else if (strcmp(mi->name, "UnloadSceneNameIndexInternal") == 0) {
                    int reg = 2;
                    for (int i = 0; i < (int)mi->parameters_count && i < 8; i++)
                        if (get_param_name(mi, i) == "immediately") reg = i;
                    if (fz_hook_generic(mi, freeze_kind::unload_internal, reg, "SceneManager.UnloadScene")) scene_hooks++;
                }
            }
        }
        BNM::Class ao("UnityEngine", "AsyncOperation");
        if (ao.IsValid()) {
            for (auto& m : ao.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (!mi || !mi->name || !mi->methodPointer) continue;
                if (strcmp(mi->name, "set_allowSceneActivation") == 0 && fz_hook_generic(mi, freeze_kind::ignore_true, -1, "AsyncOperation.allowSceneActivation")) scene_hooks++;
            }
        }
    } catch (...) {
    }
    if (destroy_hooks == 0 && scene_hooks == 0) {
        error = "no hookable Destroy / LoadScene methods found";
        g_fz_error = error;
        return false;
    }
    g_fz_installed = true;
    return true;
}

#else

static int g_fz_hook_count = 0;
static std::string g_fz_error;

static bool freeze_install(std::string& error) {
    error = "freeze is supported on arm64 only";
    g_fz_error = error;
    return false;
}

#endif

static void register_freeze_routes(httplib::Server& svr) {
    svr.Get("/api/freeze", [](const httplib::Request&, httplib::Response& res) {
        nlohmann::json j;
        j["frozen"] = g_freeze.load();
        j["hooks"] = g_fz_hook_count;
        {
            std::lock_guard<std::mutex> lk(g_fz_sym_mtx);
            j["symbols_ready"] = g_fz_syms_built;
            j["symbols"] = g_fz_syms.size();
        }
        j["blocked_destroy"] = g_freeze_blocked_destroy.load();
        j["blocked_scene"] = g_freeze_blocked_scene.load();
        j["error"] = g_fz_error;
        res.set_content(j.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace), "application/json");
    });

    svr.Post("/api/freeze", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body = nlohmann::json::parse(req.body, nullptr, false);
        nlohmann::json j;
        bool on = !g_freeze.load();
        if (body.is_object() && body.contains("on") && body["on"].is_boolean()) on = body["on"].get<bool>();
        if (on) {
            std::string error;
            if (!freeze_install(error)) {
                j["ok"] = false;
                j["frozen"] = false;
                j["error"] = error;
                res.set_content(j.dump(), "application/json");
                return;
            }
            g_freeze_blocked_destroy.store(0);
            g_freeze_blocked_scene.store(0);
        }
        g_freeze.store(on);
        j["ok"] = true;
        j["frozen"] = on;
        j["hooks"] = g_fz_hook_count;
        {
            std::lock_guard<std::mutex> lk(g_fz_sym_mtx);
            j["symbols_ready"] = g_fz_syms_built;
            j["symbols"] = g_fz_syms.size();
        }
        res.set_content(j.dump(), "application/json");
    });

    svr.Get("/api/freeze/tracker", [](const httplib::Request&, httplib::Response& res) {
        nlohmann::json j = nlohmann::json::object();
        j["frozen"] = g_freeze.load();
        nlohmann::json items = nlohmann::json::array();
        {
            std::lock_guard<std::mutex> lk(g_fz_track_mtx);
            for (const auto& kv : g_fz_tracks) {
                nlohmann::json item;
                item["caller"] = kv.second.caller;
                item["caller_stack"] = kv.second.caller_stack;
                item["managed_stack"] = kv.second.managed_stack;
                item["native_stack"] = kv.second.native_stack;
                item["action"] = kv.second.action;
                item["target"] = kv.second.target;
                item["in_scene"] = kv.second.in_scene;
                item["count"] = kv.second.count;
                items.push_back(std::move(item));
            }
        }
        j["items"] = std::move(items);
        res.set_content(j.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace), "application/json");
    });

    svr.Post("/api/freeze/tracker/clear", [](const httplib::Request&, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lk(g_fz_track_mtx);
            g_fz_tracks.clear();
        }
        g_freeze_blocked_destroy.store(0);
        g_freeze_blocked_scene.store(0);
        res.set_content("{\"ok\":true}", "application/json");
    });
}
