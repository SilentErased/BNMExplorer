#pragma once
#include <atomic>
#include <chrono>
#include <deque>
#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <dobby.h>
#include "json.hpp"
#include "lua_values.hpp"
#include "main_thread.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

struct call_entry {
    int id = -1;
    void* address = nullptr;
    std::string cls;
    std::string name;
    std::string params;
    std::string target;
    bool is_static = false;
    bool dead = false;
    int param_count = 0;
    value_kind kinds[16];
    std::atomic<bool> ignored{false};
    std::atomic<uint64_t> count{0};
    std::atomic<uint64_t> dropped{0};
    std::atomic<int64_t> window_start{0};
    std::atomic<int> window_recorded{0};
};

struct call_record {
    uint64_t seq;
    int64_t t_ms;
    int tid;
    int entry_id;
    uint64_t self;
    std::string args;
};

struct call_target {
    int id;
    std::string label;
    std::vector<int> entries;
};

struct call_job_state {
    std::mutex mtx;
    bool running = false;
    bool cancel = false;
    int total = 0;
    int done = 0;
    int hooked = 0;
    int failed = 0;
    std::string label;
    std::string error;
};

static std::vector<std::unique_ptr<call_entry>> g_call_entries;
static std::unordered_map<void*, call_entry*> g_call_by_addr;
static std::vector<call_target> g_call_targets;
static std::shared_mutex g_call_mtx;
static std::deque<call_record> g_call_log;
static std::mutex g_call_log_mtx;
static uint64_t g_call_seq = 0;
static std::atomic<bool> g_call_paused{false};
static std::atomic<int> g_call_rate_limit{100};
static std::atomic<size_t> g_call_max_log{5000};
static int g_call_next_target = 1;
static call_job_state g_call_job;

static const int call_max_assembly_methods = 40000;

static int64_t calls_now_ms() {
    static const auto start = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
}

struct calls_pipe_holder {
    int fds[2] = {-1, -1};
    bool failed = false;
    ~calls_pipe_holder() {
        if (fds[0] >= 0) close(fds[0]);
        if (fds[1] >= 0) close(fds[1]);
    }
};

static bool calls_safe_read(uintptr_t addr, void* out, size_t n) {
    uintptr_t clean = addr & 0x00FFFFFFFFFFFFFFULL;
    if (n == 0 || n > 512 || clean < 0x10000 || clean >= 0x0001000000000000ULL) return false;
    static thread_local calls_pipe_holder pipe_holder;
    if (pipe_holder.failed) return false;
    if (pipe_holder.fds[0] < 0) {
        if (pipe2(pipe_holder.fds, O_CLOEXEC | O_NONBLOCK) != 0) {
            pipe_holder.failed = true;
            return false;
        }
    }
    ssize_t w = write(pipe_holder.fds[1], (const void*)addr, n);
    if (w < 0) return false;
    if ((size_t)w != n) {
        char sink[512];
        ssize_t left = w;
        while (left > 0) {
            ssize_t r = read(pipe_holder.fds[0], sink, std::min<size_t>(sizeof(sink), (size_t)left));
            if (r <= 0) break;
            left -= r;
        }
        return false;
    }
    ssize_t r = read(pipe_holder.fds[0], out, n);
    return r == (ssize_t)n;
}

static bool calls_read_cstring(uintptr_t addr, std::string& out, size_t max_len) {
    out.clear();
    char chunk[16];
    size_t step = 16;
    while (out.size() < max_len) {
        if (!calls_safe_read(addr + out.size(), chunk, step)) {
            if (step == 1) return !out.empty();
            step = 1;
            continue;
        }
        for (size_t i = 0; i < step; i++) {
            if (chunk[i] == '\0') return true;
            out += chunk[i];
            if (out.size() >= max_len) return true;
        }
    }
    return true;
}

static void calls_append_utf8(std::string& out, uint32_t cp) {
    if (cp < 0x80) out += (char)cp;
    else if (cp < 0x800) {
        out += (char)(0xC0 | (cp >> 6));
        out += (char)(0x80 | (cp & 0x3F));
    } else {
        out += (char)(0xE0 | (cp >> 12));
        out += (char)(0x80 | ((cp >> 6) & 0x3F));
        out += (char)(0x80 | (cp & 0x3F));
    }
}

static std::string calls_format_string(uint64_t ptr) {
    if (!ptr) return "null";
    int32_t length = 0;
    if (!calls_safe_read((uintptr_t)ptr + 0x10, &length, sizeof(length))) return "<unreadable string>";
    if (length < 0 || length > 100000) return "<invalid string>";
    int take = length > 120 ? 120 : length;
    std::string out = "\"";
    uint16_t chars[120];
    if (take > 0 && !calls_safe_read((uintptr_t)ptr + 0x14, chars, (size_t)take * 2)) return "<unreadable string>";
    for (int i = 0; i < take; i++) {
        uint16_t c = chars[i];
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else if (c < 0x20) out += '?';
        else if (c >= 0xD800 && c <= 0xDFFF) out += '?';
        else calls_append_utf8(out, c);
    }
    if (length > take) out += "...";
    out += "\"";
    return out;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
static std::string calls_format_object(uint64_t ptr) {
    if (!ptr) return "null";
    char addr_buf[32];
    snprintf(addr_buf, sizeof(addr_buf), "0x%llX", (unsigned long long)ptr);
    uint64_t klass = 0;
    if (!calls_safe_read((uintptr_t)ptr, &klass, sizeof(klass))) return addr_buf;
    uint64_t name_ptr = 0;
    uint64_t ns_ptr = 0;
    if (!calls_safe_read((uintptr_t)klass + offsetof(Il2CppClass, name), &name_ptr, sizeof(name_ptr))) return addr_buf;
    if (!calls_safe_read((uintptr_t)klass + offsetof(Il2CppClass, namespaze), &ns_ptr, sizeof(ns_ptr))) ns_ptr = 0;
    std::string name;
    if (!name_ptr || !calls_read_cstring((uintptr_t)name_ptr, name, 80)) return addr_buf;
    return name + "@" + addr_buf;
}
#pragma GCC diagnostic pop

static std::string calls_format_float(double v) {
    char buf[48];
    snprintf(buf, sizeof(buf), "%.6g", v);
    return buf;
}

#if defined(__aarch64__)
static std::string calls_format_args(const call_entry* e, DobbyRegisterContext* ctx, uint64_t& self_out) {
    int gi = 0;
    int fi = 0;
    self_out = 0;
    if (!e->is_static) self_out = ctx->general.x[gi++];
    std::string out;
    bool stop = false;
    for (int i = 0; i < e->param_count && !stop; i++) {
        if (i) out += ", ";
        value_kind k = e->kinds[i];
        std::string v;
        auto take_int = [&](uint64_t& dst) -> bool {
            if (gi >= 8) return false;
            dst = ctx->general.x[gi++];
            return true;
        };
        auto take_float = [&](float& dst) -> bool {
            if (fi >= 8) return false;
            dst = ctx->floating.q[fi++].f.f1;
            return true;
        };
        uint64_t x = 0;
        switch (k) {
            case value_kind::boolean:
                v = take_int(x) ? ((x & 1) ? "true" : "false") : "?";
                break;
            case value_kind::int8:
                v = take_int(x) ? std::to_string((int)(int8_t)x) : "?";
                break;
            case value_kind::uint8:
                v = take_int(x) ? std::to_string((unsigned)(uint8_t)x) : "?";
                break;
            case value_kind::int16:
                v = take_int(x) ? std::to_string((int)(int16_t)x) : "?";
                break;
            case value_kind::uint16:
                v = take_int(x) ? std::to_string((unsigned)(uint16_t)x) : "?";
                break;
            case value_kind::int32:
            case value_kind::enumeration:
                v = take_int(x) ? std::to_string((int)(int32_t)x) : "?";
                break;
            case value_kind::uint32:
                v = take_int(x) ? std::to_string((unsigned)(uint32_t)x) : "?";
                break;
            case value_kind::int64:
                v = take_int(x) ? std::to_string((long long)(int64_t)x) : "?";
                break;
            case value_kind::uint64:
                v = take_int(x) ? std::to_string((unsigned long long)x) : "?";
                break;
            case value_kind::float32: {
                float f = 0;
                v = take_float(f) ? calls_format_float(f) : "?";
                break;
            }
            case value_kind::float64: {
                double d = 0;
                if (fi < 8) {
                    d = ctx->floating.q[fi++].d.d1;
                    v = calls_format_float(d);
                } else {
                    v = "?";
                }
                break;
            }
            case value_kind::vec2:
            case value_kind::vec3:
            case value_kind::vec4:
            case value_kind::color: {
                int n = kind_components(k);
                v = "(";
                for (int c = 0; c < n; c++) {
                    float f = 0;
                    if (c) v += ", ";
                    v += take_float(f) ? calls_format_float(f) : "?";
                }
                v += ")";
                break;
            }
            case value_kind::string:
                v = take_int(x) ? calls_format_string(x) : "?";
                break;
            case value_kind::object:
                v = take_int(x) ? calls_format_object(x) : "?";
                break;
            default:
                v = "<struct>";
                stop = true;
                break;
        }
        out += v;
    }
    if (stop) out += ", ...";
    return out;
}
#else
static std::string calls_format_args(const call_entry*, DobbyRegisterContext*, uint64_t& self_out) {
    self_out = 0;
    return "";
}
#endif

static void calls_on_enter(void* address, DobbyRegisterContext* ctx) {
    static thread_local bool in_callback = false;
    if (in_callback) return;
    call_entry* e = nullptr;
    {
        std::shared_lock<std::shared_mutex> lk(g_call_mtx);
        auto it = g_call_by_addr.find(address);
        if (it == g_call_by_addr.end()) return;
        e = it->second;
    }
    e->count.fetch_add(1, std::memory_order_relaxed);
    if (g_call_paused.load(std::memory_order_relaxed) || e->ignored.load(std::memory_order_relaxed)) return;
    int64_t now = calls_now_ms();
    int64_t window = e->window_start.load(std::memory_order_relaxed);
    if (now - window >= 1000) {
        e->window_start.store(now, std::memory_order_relaxed);
        e->window_recorded.store(0, std::memory_order_relaxed);
    }
    int limit = g_call_rate_limit.load(std::memory_order_relaxed);
    if (limit > 0 && e->window_recorded.fetch_add(1, std::memory_order_relaxed) >= limit) {
        e->dropped.fetch_add(1, std::memory_order_relaxed);
        return;
    }
    in_callback = true;
    call_record rec;
    rec.t_ms = now;
    rec.tid = (int)syscall(SYS_gettid);
    rec.entry_id = e->id;
    rec.self = 0;
    rec.args = calls_format_args(e, ctx, rec.self);
    {
        std::lock_guard<std::mutex> lk(g_call_log_mtx);
        rec.seq = ++g_call_seq;
        g_call_log.push_back(std::move(rec));
        size_t max_log = g_call_max_log.load(std::memory_order_relaxed);
        while (g_call_log.size() > max_log) g_call_log.pop_front();
    }
    in_callback = false;
}

static std::string calls_short_type(const std::string& full) {
    size_t dot = full.rfind('.');
    return dot == std::string::npos ? full : full.substr(dot + 1);
}

static bool calls_hook_method(MethodInfo* mi, const std::string& cls_full, const std::string& target_label, int& out_id) {
    out_id = -1;
    if (!mi || !mi->methodPointer || mi->is_generic || !mi->name) return false;
    if (mi->parameters_count > 16) return false;
    void* addr = (void*)mi->methodPointer;
    {
        std::shared_lock<std::shared_mutex> lk(g_call_mtx);
        if (g_call_by_addr.count(addr)) return false;
    }
    auto entry = std::make_unique<call_entry>();
    entry->address = addr;
    entry->cls = cls_full;
    entry->name = mi->name;
    entry->target = target_label;
    entry->is_static = (mi->flags & 0x10) != 0;
    entry->param_count = (int)mi->parameters_count;
    for (int i = 0; i < entry->param_count; i++) {
        entry->kinds[i] = kind_from_type(bnm_method_parameter_type(mi, static_cast<uint32_t>(i)));
        if (i) entry->params += ", ";
        entry->params += calls_short_type(type_full_name(bnm_method_parameter_type(mi, static_cast<uint32_t>(i))));
    }
    call_entry* raw = entry.get();
    {
        std::unique_lock<std::shared_mutex> lk(g_call_mtx);
        raw->id = (int)g_call_entries.size();
        g_call_entries.push_back(std::move(entry));
        g_call_by_addr[addr] = raw;
    }
    int rc = DobbyInstrument(addr, calls_on_enter);
    if (rc != RT_SUCCESS) {
        std::unique_lock<std::shared_mutex> lk(g_call_mtx);
        g_call_by_addr.erase(addr);
        raw->dead = true;
        return false;
    }
    out_id = raw->id;
    return true;
}

static const char* calls_skip_names[] = {
    "Update", "LateUpdate", "FixedUpdate", "OnGUI", "OnPreRender", "OnPostRender", "OnPreCull",
    "OnRenderObject", "OnWillRenderObject", "OnRenderImage", "OnAnimatorMove", "OnAnimatorIK",
    "OnDrawGizmos", "OnDrawGizmosSelected", "OnValidate", "OnAudioFilterRead", ".cctor", nullptr
};

static bool calls_name_skipped(const char* n) {
    for (auto** s = calls_skip_names; *s; s++)
        if (strcmp(n, *s) == 0) return true;
    return false;
}

static bool calls_method_allowed(MethodInfo* mi) {
    if (!mi || !mi->name || !mi->methodPointer || mi->is_generic) return false;
    if (mi->name[0] == '\0' || calls_name_skipped(mi->name)) return false;
    if (mi->flags & 0x0400) return false;
    if (mi->parameters_count > 16) return false;
    uint8_t probe[16];
    if (!calls_safe_read((uintptr_t)mi->methodPointer, probe, sizeof(probe))) return false;
    return true;
}

static bool calls_class_is_engine(Il2CppClass* k) {
    if (!k || !k->name) return true;
    const char* an = (k->image && k->image->name) ? k->image->name : "";
    if (strncmp(an, "UnityEngine", 11) == 0 || strncmp(an, "Unity.", 6) == 0 || strncmp(an, "mscorlib", 8) == 0 ||
        strncmp(an, "System", 6) == 0 || strncmp(an, "netstandard", 11) == 0)
        return true;
    return strcmp(k->name, "MonoBehaviour") == 0 || strcmp(k->name, "Behaviour") == 0 || strcmp(k->name, "Component") == 0 ||
           strcmp(k->name, "Object") == 0 || strcmp(k->name, "ScriptableObject") == 0;
}

static void calls_collect_class(Class& cls, std::vector<std::pair<MethodInfo*, std::string>>& out) {
    auto* k = cls.IsValid() ? cls.GetClass() : nullptr;
    if (!k || !k->name || k->name[0] == '<') return;
    std::string full = k->name;
    if (k->namespaze && k->namespaze[0] != '\0') full = std::string(k->namespaze) + "." + full;
    for (auto& m : cls.GetMethods(false)) {
        auto* mi = m.GetInfo();
        if (calls_method_allowed(mi)) out.push_back({mi, full});
    }
}

static void calls_run_watch_job(std::string asm_name, std::string ns, std::string cls_name) {
    BNM::AttachIl2Cpp();
    std::vector<std::pair<MethodInfo*, std::string>> methods;
    std::string label;
    try {
        if (!cls_name.empty()) {
            label = ns.empty() ? cls_name : ns + "." + cls_name;
            Class c = asm_name.empty() ? Class(ns.c_str(), cls_name.c_str()) : Class(ns.c_str(), cls_name.c_str(), Image(asm_name.c_str()));
            if (!c.IsValid()) {
                for (auto& img : Image::GetImages()) {
                    if (!img.IsValid()) continue;
                    Class tc(ns.c_str(), cls_name.c_str(), img);
                    if (tc.IsValid()) {
                        c = tc;
                        break;
                    }
                }
            }
            calls_collect_class(c, methods);
        } else {
            label = asm_name;
            Image img(asm_name);
            if (img.IsValid()) {
                for (auto& c : img.GetClasses(false)) {
                    calls_collect_class(c, methods);
                    if ((int)methods.size() > call_max_assembly_methods) break;
                }
            }
        }
    } catch (...) {
    }
    if ((int)methods.size() > call_max_assembly_methods) methods.resize(call_max_assembly_methods);
    {
        std::lock_guard<std::mutex> lk(g_call_job.mtx);
        g_call_job.total = (int)methods.size();
        g_call_job.label = label;
        if (methods.empty()) g_call_job.error = "nothing to hook: class or assembly not found";
    }
    call_target target;
    target.label = label;
    int hooked = 0;
    int failed = 0;
    for (size_t i = 0; i < methods.size(); i++) {
        {
            std::lock_guard<std::mutex> lk(g_call_job.mtx);
            if (g_call_job.cancel) break;
        }
        int id = -1;
        try {
            if (calls_hook_method(methods[i].first, methods[i].second, label, id)) {
                hooked++;
                target.entries.push_back(id);
            } else {
                failed++;
            }
        } catch (...) {
            failed++;
        }
        if ((i & 31) == 0) {
            std::lock_guard<std::mutex> lk(g_call_job.mtx);
            g_call_job.done = (int)i + 1;
            g_call_job.hooked = hooked;
            g_call_job.failed = failed;
        }
    }
    if (hooked > 0) {
        std::unique_lock<std::shared_mutex> lk(g_call_mtx);
        target.id = g_call_next_target++;
        g_call_targets.push_back(std::move(target));
    }
    std::lock_guard<std::mutex> lk(g_call_job.mtx);
    g_call_job.done = (int)methods.size();
    g_call_job.hooked = hooked;
    g_call_job.failed = failed;
    g_call_job.running = false;
}

static bool calls_start_watch(const std::string& asm_name, const std::string& ns, const std::string& cls_name, std::string& error) {
    std::lock_guard<std::mutex> lk(g_call_job.mtx);
    if (g_call_job.running) {
        error = "another hook job is still running";
        return false;
    }
    g_call_job.running = true;
    g_call_job.cancel = false;
    g_call_job.total = 0;
    g_call_job.done = 0;
    g_call_job.hooked = 0;
    g_call_job.failed = 0;
    g_call_job.error.clear();
    g_call_job.label = cls_name.empty() ? asm_name : cls_name;
    std::thread(calls_run_watch_job, asm_name, ns, cls_name).detach();
    return true;
}

static int calls_unwatch(int target_id);
static void calls_unwatch_forward(int target_id) { calls_unwatch(target_id); }

struct scene_watch_state {
    std::atomic<bool> on{false};
    std::atomic<bool> auto_new{true};
    std::atomic<int> generation{0};
    std::atomic<int> classes{0};
    std::atomic<int> queued{0};
    std::atomic<int> hooked{0};
    std::atomic<int> failed{0};
    std::atomic<int> target_id{0};
};

static scene_watch_state g_scene_watch;

static void calls_scene_new_classes(std::unordered_set<Il2CppClass*>& seen, std::vector<Il2CppClass*>& out) {
    Il2CppObject* mono_type = nullptr;
    Class mc("UnityEngine", "MonoBehaviour");
    if (mc.IsValid()) mono_type = (Il2CppObject*)mc.GetMonoType();
    if (!mono_type) mono_type = sys_type_of("UnityEngine.MonoBehaviour, UnityEngine.CoreModule");
    if (!mono_type) return;
    Array<Il2CppObject*>* arr = nullptr;
    auto with_inactive = find_objs_method_inactive();
    if (with_inactive.IsValid()) {
        try {
            arr = Method<Array<Il2CppObject*>*>(with_inactive)(mono_type, true);
        } catch (...) {
            arr = nullptr;
        }
    }
    if (!arr) {
        auto plain = find_objs_method();
        if (plain.IsValid()) {
            try {
                arr = Method<Array<Il2CppObject*>*>(plain)(mono_type);
            } catch (...) {
                arr = nullptr;
            }
        }
    }
    if (!arr) return;
    for (int i = 0; i < arr->capacity; i++) {
        Il2CppObject* o = arr->m_Items[i];
        if (!o || !is_valid_pointer(o)) continue;
        Il2CppClass* k = o->klass;
        if (!k || !is_valid_pointer(k)) continue;
        for (Il2CppClass* cur = k; cur && is_valid_pointer(cur); cur = cur->parent) {
            if (!cur->name || !is_valid_pointer(cur->name)) break;
            if (calls_class_is_engine(cur)) break;
            if (!seen.insert(cur).second) break;
            out.push_back(cur);
        }
    }
}

static void calls_target_add(int target_id, int entry_id) {
    std::unique_lock<std::shared_mutex> lk(g_call_mtx);
    for (auto& t : g_call_targets)
        if (t.id == target_id) {
            t.entries.push_back(entry_id);
            return;
        }
}

static void calls_start_scene_watch(bool auto_new) {
    int gen = g_scene_watch.generation.fetch_add(1) + 1;
    g_scene_watch.on.store(true);
    g_scene_watch.auto_new.store(auto_new);
    g_scene_watch.classes.store(0);
    g_scene_watch.queued.store(0);
    g_scene_watch.hooked.store(0);
    g_scene_watch.failed.store(0);
    int tid = 0;
    {
        std::unique_lock<std::shared_mutex> lk(g_call_mtx);
        call_target target;
        target.id = g_call_next_target++;
        target.label = "scene components";
        tid = target.id;
        g_call_targets.push_back(std::move(target));
    }
    g_scene_watch.target_id.store(tid);
    auto seen = std::make_shared<std::unordered_set<Il2CppClass*>>();
    auto queue = std::make_shared<std::deque<std::pair<MethodInfo*, std::string>>>();
    auto last_scan = std::make_shared<int64_t>(-1);
    mt_post([=]() -> bool {
        if (!g_scene_watch.on.load() || g_scene_watch.generation.load() != gen) return true;
        int64_t now = calls_now_ms();
        bool first = *last_scan < 0;
        if (queue->empty() && (first || (auto_new && now - *last_scan >= 1500))) {
            *last_scan = now;
            std::vector<Il2CppClass*> fresh;
            calls_scene_new_classes(*seen, fresh);
            for (auto* k : fresh) {
                Class cls(k);
                std::vector<std::pair<MethodInfo*, std::string>> found;
                calls_collect_class(cls, found);
                for (auto& f : found) queue->push_back(f);
            }
            g_scene_watch.classes.store((int)seen->size());
        }
        auto t0 = std::chrono::steady_clock::now();
        while (!queue->empty()) {
            auto item = queue->front();
            queue->pop_front();
            int id = -1;
            bool ok = false;
            try {
                ok = calls_hook_method(item.first, item.second, "scene components", id);
            } catch (...) {
                ok = false;
            }
            if (ok) {
                g_scene_watch.hooked.fetch_add(1);
                calls_target_add(tid, id);
            } else {
                g_scene_watch.failed.fetch_add(1);
            }
            if (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - t0).count() > 2000) break;
        }
        g_scene_watch.queued.store((int)queue->size());
        if (!auto_new && queue->empty() && !first) return true;
        if (!auto_new && queue->empty() && first && g_scene_watch.hooked.load() + g_scene_watch.failed.load() > 0) return true;
        return false;
    }, false);
}

static void calls_stop_scene_watch() {
    g_scene_watch.on.store(false);
    g_scene_watch.generation.fetch_add(1);
    int tid = g_scene_watch.target_id.exchange(0);
    if (tid > 0) {
        mt_post([tid]() -> bool {
            calls_unwatch_forward(tid);
            return true;
        }, false);
    }
}

static void calls_unhook_entry(call_entry* e) {
    if (!e || e->dead) return;
    DobbyDestroy(e->address);
    e->dead = true;
    g_call_by_addr.erase(e->address);
}

static int calls_unwatch(int target_id) {
    std::unique_lock<std::shared_mutex> lk(g_call_mtx);
    int removed = 0;
    for (size_t i = 0; i < g_call_targets.size();) {
        if (target_id == 0 || g_call_targets[i].id == target_id) {
            for (int id : g_call_targets[i].entries) {
                if (id >= 0 && id < (int)g_call_entries.size()) {
                    calls_unhook_entry(g_call_entries[id].get());
                    removed++;
                }
            }
            g_call_targets.erase(g_call_targets.begin() + i);
        } else {
            i++;
        }
    }
    return removed;
}

static std::string calls_entry_name(const call_entry* e) {
    return e->cls + "::" + e->name;
}

static void calls_reply(httplib::Response& res, const nlohmann::json& j, int status = 200) {
    res.status = status;
    res.set_content(j.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace), "application/json");
}

static void register_calls_routes(httplib::Server& svr) {
    svr.Get("/api/calls/status", [](const httplib::Request&, httplib::Response& res) {
        nlohmann::json j;
        int live = 0;
        nlohmann::json targets = nlohmann::json::array();
        {
            std::shared_lock<std::shared_mutex> lk(g_call_mtx);
            for (auto& e : g_call_entries)
                if (!e->dead) live++;
            for (auto& t : g_call_targets) {
                nlohmann::json item;
                item["id"] = t.id;
                item["label"] = t.label;
                item["count"] = t.entries.size();
                targets.push_back(item);
            }
        }
        j["hooked"] = live;
        {
            nlohmann::json sc;
            sc["on"] = g_scene_watch.on.load();
            sc["auto"] = g_scene_watch.auto_new.load();
            sc["classes"] = g_scene_watch.classes.load();
            sc["queued"] = g_scene_watch.queued.load();
            sc["hooked"] = g_scene_watch.hooked.load();
            sc["failed"] = g_scene_watch.failed.load();
            sc["main_thread"] = g_mt_ready.load();
            sc["main_thread_hook"] = g_mt_hooked_name;
            sc["main_thread_error"] = g_mt_error;
            j["scene"] = sc;
        }
        j["targets"] = targets;
        j["paused"] = g_call_paused.load();
        j["limit"] = g_call_rate_limit.load();
        j["max_log"] = g_call_max_log.load();
        {
            std::lock_guard<std::mutex> lk(g_call_log_mtx);
            j["seq"] = g_call_seq;
        }
        {
            std::lock_guard<std::mutex> lk(g_call_job.mtx);
            nlohmann::json job;
            job["running"] = g_call_job.running;
            job["total"] = g_call_job.total;
            job["done"] = g_call_job.done;
            job["hooked"] = g_call_job.hooked;
            job["failed"] = g_call_job.failed;
            job["label"] = g_call_job.label;
            job["error"] = g_call_job.error;
            j["job"] = job;
        }
        calls_reply(res, j);
    });

    svr.Get("/api/calls/log", [](const httplib::Request& req, httplib::Response& res) {
        uint64_t since = strtoull(req.get_param_value("since").c_str(), nullptr, 10);
        std::vector<call_record> batch;
        uint64_t last = 0;
        {
            std::lock_guard<std::mutex> lk(g_call_log_mtx);
            last = g_call_seq;
            for (auto& r : g_call_log) {
                if (r.seq > since) batch.push_back(r);
                if (batch.size() >= 800) break;
            }
        }
        nlohmann::json items = nlohmann::json::array();
        {
            std::shared_lock<std::shared_mutex> lk(g_call_mtx);
            for (auto& r : batch) {
                nlohmann::json item;
                item["s"] = r.seq;
                item["t"] = r.t_ms;
                item["th"] = r.tid;
                item["m"] = r.entry_id;
                if (r.entry_id >= 0 && r.entry_id < (int)g_call_entries.size()) {
                    const call_entry* e = g_call_entries[r.entry_id].get();
                    item["c"] = e->cls;
                    item["f"] = e->name;
                    item["st"] = e->is_static;
                }
                char self_buf[24];
                snprintf(self_buf, sizeof(self_buf), "0x%llX", (unsigned long long)r.self);
                if (r.self) item["i"] = self_buf;
                item["a"] = r.args;
                items.push_back(item);
            }
        }
        nlohmann::json j;
        j["items"] = items;
        j["seq"] = batch.empty() ? last : batch.back().seq;
        j["latest"] = last;
        calls_reply(res, j);
    });

    svr.Get("/api/calls/methods", [](const httplib::Request&, httplib::Response& res) {
        struct row {
            int id;
            uint64_t count;
        };
        std::vector<row> rows;
        nlohmann::json list = nlohmann::json::array();
        {
            std::shared_lock<std::shared_mutex> lk(g_call_mtx);
            for (auto& e : g_call_entries)
                if (!e->dead) rows.push_back({e->id, e->count.load()});
            std::sort(rows.begin(), rows.end(), [](const row& a, const row& b) { return a.count > b.count; });
            size_t limit = 4000;
            for (size_t i = 0; i < rows.size() && i < limit; i++) {
                const call_entry* e = g_call_entries[rows[i].id].get();
                nlohmann::json item;
                item["id"] = e->id;
                item["c"] = e->cls;
                item["f"] = e->name;
                item["p"] = e->params;
                item["st"] = e->is_static;
                item["n"] = e->count.load();
                item["d"] = e->dropped.load();
                item["i"] = e->ignored.load();
                list.push_back(item);
            }
        }
        nlohmann::json j;
        j["methods"] = list;
        j["total"] = rows.size();
        calls_reply(res, j);
    });

    svr.Post("/api/calls/watch", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body = nlohmann::json::parse(req.body, nullptr, false);
        nlohmann::json j;
        if (!body.is_object()) {
            j["error"] = "invalid request";
            calls_reply(res, j, 400);
            return;
        }
        auto get = [&](const char* key) -> std::string {
            auto it = body.find(key);
            return (it != body.end() && it->is_string()) ? it->get<std::string>() : "";
        };
        std::string asm_name = get("asm");
        std::string ns = get("ns");
        std::string cls = get("cls");
        if (asm_name.empty() && cls.empty()) {
            j["error"] = "specify a class or an assembly";
            calls_reply(res, j, 400);
            return;
        }
        std::string error;
        if (!calls_start_watch(asm_name, ns, cls, error)) {
            j["error"] = error;
            calls_reply(res, j, 409);
            return;
        }
        j["ok"] = true;
        calls_reply(res, j);
    });

    svr.Post("/api/calls/scene", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body = nlohmann::json::parse(req.body, nullptr, false);
        bool on = true;
        bool auto_new = true;
        if (body.is_object()) {
            if (body.contains("on") && body["on"].is_boolean()) on = body["on"].get<bool>();
            if (body.contains("auto") && body["auto"].is_boolean()) auto_new = body["auto"].get<bool>();
        }
        if (on) {
            if (g_scene_watch.on.load()) calls_stop_scene_watch();
            calls_start_scene_watch(auto_new);
        } else {
            calls_stop_scene_watch();
        }
        nlohmann::json j;
        j["ok"] = true;
        j["on"] = g_scene_watch.on.load();
        calls_reply(res, j);
    });

    svr.Post("/api/calls/cancel", [](const httplib::Request&, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lk(g_call_job.mtx);
            g_call_job.cancel = true;
        }
        nlohmann::json j;
        j["ok"] = true;
        calls_reply(res, j);
    });

    svr.Post("/api/calls/unwatch", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body = nlohmann::json::parse(req.body, nullptr, false);
        int target = 0;
        if (body.is_object() && body.contains("target") && body["target"].is_number_integer()) target = body["target"].get<int>();
        nlohmann::json j;
        if (target == 0 || target == g_scene_watch.target_id.load()) {
            g_scene_watch.on.store(false);
            g_scene_watch.generation.fetch_add(1);
            g_scene_watch.target_id.store(0);
        }
        j["ok"] = true;
        j["removed"] = calls_unwatch(target);
        calls_reply(res, j);
    });

    svr.Post("/api/calls/ignore", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body = nlohmann::json::parse(req.body, nullptr, false);
        nlohmann::json j;
        if (!body.is_object() || !body.contains("ignored") || !body["ignored"].is_boolean()) {
            j["error"] = "invalid request";
            calls_reply(res, j, 400);
            return;
        }
        bool ignored = body["ignored"].get<bool>();
        int changed = 0;
        std::shared_lock<std::shared_mutex> lk(g_call_mtx);
        if (body.contains("ids") && body["ids"].is_array()) {
            for (auto& v : body["ids"]) {
                if (!v.is_number_integer()) continue;
                int id = v.get<int>();
                if (id >= 0 && id < (int)g_call_entries.size() && !g_call_entries[id]->dead) {
                    g_call_entries[id]->ignored.store(ignored);
                    changed++;
                }
            }
        }
        std::string name = body.contains("name") && body["name"].is_string() ? body["name"].get<std::string>() : "";
        std::string cls = body.contains("cls") && body["cls"].is_string() ? body["cls"].get<std::string>() : "";
        if (!name.empty() || !cls.empty()) {
            for (auto& e : g_call_entries) {
                if (e->dead) continue;
                if (!name.empty() && e->name != name) continue;
                if (!cls.empty() && e->cls != cls) continue;
                e->ignored.store(ignored);
                changed++;
            }
        }
        if (body.contains("all") && body["all"].is_boolean() && body["all"].get<bool>()) {
            for (auto& e : g_call_entries) {
                if (e->dead) continue;
                e->ignored.store(ignored);
                changed++;
            }
        }
        j["ok"] = true;
        j["changed"] = changed;
        calls_reply(res, j);
    });

    svr.Post("/api/calls/pause", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body = nlohmann::json::parse(req.body, nullptr, false);
        if (body.is_object() && body.contains("paused") && body["paused"].is_boolean()) g_call_paused.store(body["paused"].get<bool>());
        nlohmann::json j;
        j["ok"] = true;
        j["paused"] = g_call_paused.load();
        calls_reply(res, j);
    });

    svr.Post("/api/calls/clear", [](const httplib::Request&, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lk(g_call_log_mtx);
            g_call_log.clear();
        }
        {
            std::shared_lock<std::shared_mutex> lk(g_call_mtx);
            for (auto& e : g_call_entries) {
                e->count.store(0);
                e->dropped.store(0);
            }
        }
        nlohmann::json j;
        j["ok"] = true;
        calls_reply(res, j);
    });

    svr.Post("/api/calls/config", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body = nlohmann::json::parse(req.body, nullptr, false);
        if (body.is_object()) {
            if (body.contains("limit") && body["limit"].is_number_integer()) g_call_rate_limit.store(std::max(0, std::min(100000, body["limit"].get<int>())));
            if (body.contains("max_log") && body["max_log"].is_number_integer()) g_call_max_log.store((size_t)std::max(100, std::min(50000, body["max_log"].get<int>())));
        }
        nlohmann::json j;
        j["ok"] = true;
        j["limit"] = g_call_rate_limit.load();
        j["max_log"] = g_call_max_log.load();
        calls_reply(res, j);
    });
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
