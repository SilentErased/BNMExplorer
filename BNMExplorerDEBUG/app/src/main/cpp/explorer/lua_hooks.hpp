#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <dobby.h>
#include "lua_runtime.hpp"
#include "symbols.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

static constexpr int lua_hook_slot_count = 256;
static constexpr int lua_hook_float_slot_count = 48;
static constexpr int lua_hook_int_regs = 8;
static constexpr int lua_hook_fp_regs = 8;
static constexpr int lua_hook_max_params = 16;

#define LUA_HOOK_PARAMS uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7, \
                        double d0, double d1, double d2, double d3, double d4, double d5, double d6, double d7
#define LUA_HOOK_REGS {{a0, a1, a2, a3, a4, a5, a6, a7}, {d0, d1, d2, d3, d4, d5, d6, d7}}
#define LUA_HOOK_UNPACK(r) r.x[0], r.x[1], r.x[2], r.x[3], r.x[4], r.x[5], r.x[6], r.x[7], \
                           r.d[0], r.d[1], r.d[2], r.d[3], r.d[4], r.d[5], r.d[6], r.d[7]

struct lua_hook_regs {
    uint64_t x[lua_hook_int_regs];
    double d[lua_hook_fp_regs];
};

struct lua_hook_param {
    value_kind kind = value_kind::unsupported;
    bool in_fp = false;
    bool byref = false;
    int reg = 0;
};

struct lua_hook_entry {
    std::atomic<bool> active{false};
    std::atomic<uint64_t> calls{0};
    bool installed = false;
    bool is_static = false;
    int param_count = 0;
    int callback_ref = LUA_NOREF;
    void* address = nullptr;
    void* original = nullptr;
    value_kind return_kind = value_kind::unsupported;
    lua_hook_param params[lua_hook_max_params];
    std::string label;
};

struct lua_hook_result {
    bool call_original = true;
    uint64_t word = 0;
    float f32 = 0.0f;
    double f64 = 0.0;
    float vec[4] = {0, 0, 0, 0};
};

struct lua_hook_vec_ret {
    float v[4];
};

static lua_hook_entry g_lua_hooks[lua_hook_slot_count];
static std::mutex g_lua_hook_install_mutex;
static int g_lua_hook_null_marker = 0;
static thread_local lua_hook_entry* g_lua_hook_current = nullptr;
static thread_local const lua_hook_regs* g_lua_hook_current_regs = nullptr;
static thread_local uintptr_t g_lua_hook_current_caller = 0;

static bool lua_hook_is_word_kind(value_kind kind) {
    switch (kind) {
        case value_kind::boolean:
        case value_kind::int8:
        case value_kind::uint8:
        case value_kind::int16:
        case value_kind::uint16:
        case value_kind::int32:
        case value_kind::uint32:
        case value_kind::int64:
        case value_kind::uint64:
        case value_kind::enumeration:
        case value_kind::string:
        case value_kind::object:
            return true;
        default:
            return false;
    }
}

static bool lua_hook_is_fp_kind(value_kind kind) {
    return kind == value_kind::float32 || kind == value_kind::float64 || kind_is_vector(kind);
}

static float lua_hook_reg_to_float(double reg) {
    float f;
    memcpy(&f, &reg, sizeof(f));
    return f;
}

static double lua_hook_float_to_reg(float f) {
    double reg = 0.0;
    memcpy(&reg, &f, sizeof(f));
    return reg;
}

static void lua_hook_push_word(lua_State* L, value_kind kind, uint64_t raw) {
    if (kind == value_kind::object) {
        push_object(L, (Il2CppObject*)(uintptr_t)raw);
        return;
    }
    if (kind == value_kind::string) {
        auto* value = (BNM::Structures::Mono::String*)(uintptr_t)raw;
        if (value) lua_pushstring(L, value->str().c_str());
        else lua_pushnil(L);
        return;
    }
    push_from_memory(L, kind, &raw);
}

static void lua_hook_push_param(lua_State* L, const lua_hook_param& p, const lua_hook_regs& r) {
    if (p.byref) {
        lua_pushnumber(L, (double)r.x[p.reg]);
        return;
    }
    if (!p.in_fp) {
        lua_hook_push_word(L, p.kind, r.x[p.reg]);
        return;
    }
    if (p.kind == value_kind::float32) {
        lua_pushnumber(L, lua_hook_reg_to_float(r.d[p.reg]));
    } else if (p.kind == value_kind::float64) {
        lua_pushnumber(L, r.d[p.reg]);
    } else {
        int n = kind_components(p.kind);
        float c[4] = {0, 0, 0, 0};
        for (int i = 0; i < n; ++i) c[i] = lua_hook_reg_to_float(r.d[p.reg + i]);
        push_vec(L, c, n, p.kind == value_kind::color);
    }
}

static bool lua_hook_is_null_marker(lua_State* L, int index) {
    return lua_islightuserdata(L, index) && lua_touserdata(L, index) == &g_lua_hook_null_marker;
}

static uint64_t lua_hook_read_word(lua_State* L, int index, value_kind kind) {
    switch (kind) {
        case value_kind::boolean:
            return lua_toboolean(L, index) ? 1ULL : 0ULL;
        case value_kind::int8:
            return (uint64_t)(int64_t)(int8_t)lua_tointeger(L, index);
        case value_kind::uint8:
            return (uint64_t)(uint8_t)lua_tointeger(L, index);
        case value_kind::int16:
            return (uint64_t)(int64_t)(int16_t)lua_tointeger(L, index);
        case value_kind::uint16:
            return (uint64_t)(uint16_t)lua_tointeger(L, index);
        case value_kind::int32:
        case value_kind::enumeration:
            return (uint64_t)(int64_t)(int32_t)lua_tointeger(L, index);
        case value_kind::uint32:
            return (uint64_t)(uint32_t)lua_tointeger(L, index);
        case value_kind::int64:
            return (uint64_t)(int64_t)lua_tonumber(L, index);
        case value_kind::uint64:
            return (uint64_t)lua_tonumber(L, index);
        case value_kind::string:
            if (lua_hook_is_null_marker(L, index) || lua_isnil(L, index)) return 0;
            if (lua_type(L, index) == LUA_TSTRING) return (uint64_t)(uintptr_t)CreateMonoString(lua_tostring(L, index));
            if (lua_type(L, index) == LUA_TTABLE) {
                lua_getfield(L, index, "addr");
                uint64_t saddr = lua_isnumber(L, -1) ? (uint64_t)lua_tonumber(L, -1) : 0;
                lua_pop(L, 1);
                return saddr;
            }
            return 0;
        case value_kind::object: {
            if (lua_hook_is_null_marker(L, index) || lua_isnil(L, index)) return 0;
            if (auto* ref = (lua_ref*)test_udata(L, index, mt_ref)) return (uint64_t)ref->inst;
            if (lua_islightuserdata(L, index)) return (uint64_t)(uintptr_t)lua_touserdata(L, index);
            if (lua_isnumber(L, index)) return (uint64_t)lua_tonumber(L, index);
            if (lua_type(L, index) == LUA_TSTRING) return (uint64_t)(uintptr_t)CreateMonoString(lua_tostring(L, index));
            if (lua_type(L, index) == LUA_TTABLE) {
                lua_getfield(L, index, "addr");
                uint64_t taddr = lua_isnumber(L, -1) ? (uint64_t)lua_tonumber(L, -1) : 0;
                lua_pop(L, 1);
                return taddr;
            }
            return 0;
        }
        default:
            return 0;
    }
}

static bool lua_hook_is_original_token(lua_State* L, int index) {
    if (lua_type(L, index) != LUA_TSTRING) return false;
    const char* value = lua_tostring(L, index);
    return value && strcmp(value, "original") == 0;
}

static lua_hook_result lua_hook_dispatch(int slot, const lua_hook_regs& regs, uintptr_t caller) {
    lua_hook_result result{};
    lua_hook_entry& entry = g_lua_hooks[slot];
    if (!entry.installed || !entry.active.load(std::memory_order_acquire)) return result;

    std::lock_guard<std::recursive_mutex> lock(g_rt.mtx);
    if (!entry.active.load(std::memory_order_acquire) || !g_rt.state || entry.callback_ref == LUA_NOREF) return result;

    BNM::AttachIl2Cpp();
    entry.calls.fetch_add(1, std::memory_order_relaxed);
    lua_State* L = g_rt.state;
    int base = lua_gettop(L);
    double previous_deadline = g_rt.deadline;
    lua_hook_entry* previous_entry = g_lua_hook_current;
    const lua_hook_regs* previous_regs = g_lua_hook_current_regs;
    uintptr_t previous_caller = g_lua_hook_current_caller;
    g_lua_hook_current = &entry;
    g_lua_hook_current_regs = &regs;
    g_lua_hook_current_caller = caller;

    lua_rawgeti(L, LUA_REGISTRYINDEX, entry.callback_ref);
    int pushed = 0;
    if (!entry.is_static) {
        push_object(L, (Il2CppObject*)(uintptr_t)regs.x[0]);
        ++pushed;
    }
    for (int i = 0; i < entry.param_count; ++i) lua_hook_push_param(L, entry.params[i], regs);
    pushed += entry.param_count;

    g_rt.deadline = rt_now() + callback_timeout_seconds;
    int status = lua_pcall(L, pushed, 1, 0);
    g_rt.deadline = previous_deadline;
    g_lua_hook_current = previous_entry;
    g_lua_hook_current_regs = previous_regs;
    g_lua_hook_current_caller = previous_caller;

    if (status != 0) {
        const char* error = lua_tostring(L, -1);
        rt_append_output("[Hook " + std::to_string(slot + 1) + " " + entry.label + "] " + (error ? error : "Lua callback failed"));
        lua_settop(L, base);
        return result;
    }

    int value_index = lua_gettop(L);
    if (entry.return_kind == value_kind::nothing) {
        result.call_original = (lua_isboolean(L, value_index) && lua_toboolean(L, value_index)) || lua_hook_is_original_token(L, value_index);
    } else if (lua_isnil(L, value_index) || lua_hook_is_original_token(L, value_index)) {
        result.call_original = true;
    } else if (kind_is_vector(entry.return_kind)) {
        result.call_original = false;
        vec_value v{};
        if (read_vec(L, value_index, v)) {
            if (entry.return_kind == value_kind::color && v.n < 4) v.c[3] = 1.0f;
            int n = kind_components(entry.return_kind);
            for (int i = 0; i < n; ++i) result.vec[i] = v.c[i];
        }
    } else {
        result.call_original = false;
        if (entry.return_kind == value_kind::float32) result.f32 = (float)lua_tonumber(L, value_index);
        else if (entry.return_kind == value_kind::float64) result.f64 = (double)lua_tonumber(L, value_index);
        else result.word = lua_hook_read_word(L, value_index, entry.return_kind);
    }
    lua_settop(L, base);
    return result;
}

template<typename R>
static R lua_hook_call_original(void* original, const lua_hook_regs& r) {
    using fn_t = R (*)(LUA_HOOK_PARAMS);
    auto fn = (fn_t)original;
    if constexpr (std::is_void_v<R>) {
        if (fn) fn(LUA_HOOK_UNPACK(r));
    } else {
        return fn ? fn(LUA_HOOK_UNPACK(r)) : R{};
    }
}

template<int Slot>
static uint64_t lua_hook_word(LUA_HOOK_PARAMS) {
    lua_hook_regs regs = LUA_HOOK_REGS;
    lua_hook_entry& entry = g_lua_hooks[Slot];
    if (!entry.installed) return 0;
    lua_hook_result result = lua_hook_dispatch(Slot, regs, (uintptr_t)__builtin_return_address(0));
    if (!result.call_original) return result.word;
    if (entry.return_kind == value_kind::nothing) {
        lua_hook_call_original<void>(entry.original, regs);
        return 0;
    }
    return lua_hook_call_original<uint64_t>(entry.original, regs);
}

template<int Slot>
static float lua_hook_float(LUA_HOOK_PARAMS) {
    lua_hook_regs regs = LUA_HOOK_REGS;
    lua_hook_entry& entry = g_lua_hooks[Slot];
    if (!entry.installed) return 0.0f;
    lua_hook_result result = lua_hook_dispatch(Slot, regs, (uintptr_t)__builtin_return_address(0));
    return result.call_original ? lua_hook_call_original<float>(entry.original, regs) : result.f32;
}

template<int Slot>
static double lua_hook_double(LUA_HOOK_PARAMS) {
    lua_hook_regs regs = LUA_HOOK_REGS;
    lua_hook_entry& entry = g_lua_hooks[Slot];
    if (!entry.installed) return 0.0;
    lua_hook_result result = lua_hook_dispatch(Slot, regs, (uintptr_t)__builtin_return_address(0));
    return result.call_original ? lua_hook_call_original<double>(entry.original, regs) : result.f64;
}

template<int Slot>
static lua_hook_vec_ret lua_hook_vec(LUA_HOOK_PARAMS) {
    lua_hook_regs regs = LUA_HOOK_REGS;
    lua_hook_entry& entry = g_lua_hooks[Slot];
    lua_hook_vec_ret out{};
    if (!entry.installed) return out;
    lua_hook_result result = lua_hook_dispatch(Slot, regs, (uintptr_t)__builtin_return_address(0));
    if (result.call_original) return lua_hook_call_original<lua_hook_vec_ret>(entry.original, regs);
    for (int i = 0; i < 4; ++i) out.v[i] = result.vec[i];
    return out;
}

using lua_hook_word_fn = uint64_t (*)(LUA_HOOK_PARAMS);
using lua_hook_float_fn = float (*)(LUA_HOOK_PARAMS);
using lua_hook_double_fn = double (*)(LUA_HOOK_PARAMS);
using lua_hook_vec_fn = lua_hook_vec_ret (*)(LUA_HOOK_PARAMS);

template<int... I>
static constexpr std::array<lua_hook_word_fn, sizeof...(I)> lua_hook_make_word_table(std::integer_sequence<int, I...>) {
    return {{&lua_hook_word<I>...}};
}

template<int... I>
static constexpr std::array<lua_hook_float_fn, sizeof...(I)> lua_hook_make_float_table(std::integer_sequence<int, I...>) {
    return {{&lua_hook_float<I>...}};
}

template<int... I>
static constexpr std::array<lua_hook_double_fn, sizeof...(I)> lua_hook_make_double_table(std::integer_sequence<int, I...>) {
    return {{&lua_hook_double<I>...}};
}

template<int... I>
static constexpr std::array<lua_hook_vec_fn, sizeof...(I)> lua_hook_make_vec_table(std::integer_sequence<int, I...>) {
    return {{&lua_hook_vec<I>...}};
}

static const auto g_lua_hook_word_thunks = lua_hook_make_word_table(std::make_integer_sequence<int, lua_hook_slot_count>{});
static const auto g_lua_hook_float_thunks = lua_hook_make_float_table(std::make_integer_sequence<int, lua_hook_float_slot_count>{});
static const auto g_lua_hook_double_thunks = lua_hook_make_double_table(std::make_integer_sequence<int, lua_hook_float_slot_count>{});
static const auto g_lua_hook_vec_thunks = lua_hook_make_vec_table(std::make_integer_sequence<int, lua_hook_float_slot_count>{});

static std::string lua_hook_short_type(const std::string& full) {
    size_t dot = full.rfind('.');
    std::string name = dot == std::string::npos ? full : full.substr(dot + 1);
    for (auto& c : name) c = (char)tolower((unsigned char)c);
    static const char* aliases[][2] = {
        {"int", "int32"}, {"uint", "uint32"}, {"long", "int64"}, {"ulong", "uint64"}, {"short", "int16"},
        {"ushort", "uint16"}, {"byte", "uint8"}, {"sbyte", "int8"}, {"float", "single"}, {"bool", "boolean"}
    };
    for (auto& a : aliases)
        if (name == a[0]) return a[1];
    return name;
}

static std::vector<std::string> lua_hook_split_signature(const std::string& text) {
    std::vector<std::string> out;
    std::string current;
    for (char c : text) {
        if (c == '(' || c == ')' || c == ' ') continue;
        if (c == ',') {
            out.push_back(lua_hook_short_type(current));
            current.clear();
        } else {
            current += c;
        }
    }
    if (!current.empty()) out.push_back(lua_hook_short_type(current));
    return out;
}

static std::string lua_hook_param_type(const MethodInfo* mi, int index) {
    return type_full_name(bnm_method_parameter_type(mi, static_cast<uint32_t>(index)));
}

static std::string lua_hook_method_signature(const MethodInfo* mi) {
    std::string out = std::string(mi->name) + "(";
    for (int i = 0; i < (int)mi->parameters_count; ++i) {
        if (i) out += ", ";
        std::string full = lua_hook_param_type(mi, i);
        size_t dot = full.rfind('.');
        out += dot == std::string::npos ? full : full.substr(dot + 1);
    }
    return out + ")";
}

static bool lua_hook_signature_matches(const MethodInfo* mi, const std::vector<std::string>& wanted) {
    if ((size_t)mi->parameters_count != wanted.size()) return false;
    for (int i = 0; i < (int)mi->parameters_count; ++i)
        if (lua_hook_short_type(lua_hook_param_type(mi, i)) != wanted[i]) return false;
    return true;
}

static std::string lua_hook_overload_list(const std::vector<MethodInfo*>& methods) {
    std::string list;
    for (auto* mi : methods) list += "\n  " + lua_hook_method_signature(mi);
    return list;
}

static bool lua_hook_assign_registers(MethodInfo* method, lua_hook_entry& entry, std::string& error) {
    int next_int = entry.is_static ? 0 : 1;
    int next_fp = 0;
    for (int i = 0; i < entry.param_count; ++i) {
        lua_hook_param& p = entry.params[i];
        const Il2CppType* pt = bnm_method_parameter_type(method, static_cast<uint32_t>(i));
        p.kind = kind_from_type(pt);
        p.byref = arg_shape_of(pt) == arg_shape::byref;
        if (p.byref || lua_hook_is_word_kind(p.kind)) {
            if (next_int >= lua_hook_int_regs) {
                error = "too many integer arguments for a register hook";
                return false;
            }
            p.in_fp = false;
            p.reg = next_int++;
        } else if (lua_hook_is_fp_kind(p.kind)) {
            int need = (p.kind == value_kind::float32 || p.kind == value_kind::float64) ? 1 : kind_components(p.kind);
            if (next_fp + need > lua_hook_fp_regs) {
                error = "too many float arguments for a register hook";
                return false;
            }
            p.in_fp = true;
            p.reg = next_fp;
            next_fp += need;
        } else {
            error = "parameter " + std::to_string(i + 1) + " has an unsupported type";
            return false;
        }
    }
    return true;
}

static MethodInfo* lua_hook_pick_method(lua_State* L, Il2CppClass* klass, const char* name, int selector_index) {
    int type = lua_type(L, selector_index);
    if (type == LUA_TNUMBER) {
        int count = (int)lua_tointeger(L, selector_index);
        const auto& found = find_candidates(klass, name, count);
        if (found.empty()) luaL_error(L, "method '%s' with %d parameters not found", name, count);
        if (found.size() > 1)
            luaL_error(L, "method '%s' has several overloads with %d parameters, pass a signature string:%s", name, count, lua_hook_overload_list(found).c_str());
        return found.front();
    }
    const auto& all = find_candidates(klass, name, -1);
    if (all.empty()) luaL_error(L, "method '%s' not found", name);
    if (type == LUA_TSTRING) {
        auto wanted = lua_hook_split_signature(lua_tostring(L, selector_index));
        for (auto* mi : all)
            if (lua_hook_signature_matches(mi, wanted)) return mi;
        luaL_error(L, "no overload of '%s' matches that signature, available:%s", name, lua_hook_overload_list(all).c_str());
    }
    if (all.size() > 1)
        luaL_error(L, "method '%s' has several overloads, pass a parameter count or a signature:%s", name, lua_hook_overload_list(all).c_str());
    return all.front();
}

static int lua_hook_allocate_slot(void* address, value_kind return_kind, bool& reuse) {
    for (int i = 0; i < lua_hook_slot_count; ++i) {
        if (g_lua_hooks[i].installed && g_lua_hooks[i].address == address) {
            reuse = true;
            return i;
        }
    }
    reuse = false;
    const bool needs_float_slot = return_kind == value_kind::float32 || return_kind == value_kind::float64 || kind_is_vector(return_kind);
    if (needs_float_slot) {
        for (int i = 0; i < lua_hook_float_slot_count; ++i)
            if (!g_lua_hooks[i].installed) return i;
        return -1;
    }
    for (int i = lua_hook_slot_count - 1; i >= 0; --i)
        if (!g_lua_hooks[i].installed) return i;
    return -1;
}

static int l_hook_method(lua_State* L) {
    const char* class_name = luaL_checkstring(L, 1);
    const char* method_name = luaL_checkstring(L, 2);
    luaL_checktype(L, 4, LUA_TFUNCTION);
    const char* assembly_name = lua_gettop(L) >= 5 && lua_isstring(L, 5) ? lua_tostring(L, 5) : "";

    BNM::AttachIl2Cpp();
    Il2CppClass* klass = find_class_cached(class_name, assembly_name);
    if (!klass) return luaL_error(L, "class '%s' not found", class_name);

    MethodInfo* method = lua_hook_pick_method(L, klass, method_name, 3);
    if (!method || !method->methodPointer || method->is_generic) return luaL_error(L, "method has no native body or is generic");
    if ((int)method->parameters_count > lua_hook_max_params) return luaL_error(L, "method has too many parameters");

    value_kind return_kind = kind_from_type(method->return_type);
    const bool fp_return = return_kind == value_kind::float32 || return_kind == value_kind::float64 || kind_is_vector(return_kind);
    if (!(return_kind == value_kind::nothing || lua_hook_is_word_kind(return_kind) || fp_return))
        return luaL_error(L, "unsupported return type for hook_method");

    std::lock_guard<std::mutex> install_lock(g_lua_hook_install_mutex);
    std::lock_guard<std::recursive_mutex> runtime_lock(g_rt.mtx);
    void* address = (void*)method->methodPointer;
    bool reuse = false;
    int slot = lua_hook_allocate_slot(address, return_kind, reuse);
    if (slot < 0) return luaL_error(L, "hook slot limit reached");

    lua_hook_entry& entry = g_lua_hooks[slot];
    if (reuse && entry.return_kind != return_kind) return luaL_error(L, "method was already hooked with a different return type");
    entry.active.store(false, std::memory_order_release);
    if (entry.callback_ref != LUA_NOREF) luaL_unref(L, LUA_REGISTRYINDEX, entry.callback_ref);
    entry.callback_ref = LUA_NOREF;

    entry.is_static = (method->flags & 0x10) != 0;
    entry.param_count = (int)method->parameters_count;
    entry.return_kind = return_kind;
    std::string error;
    if (!lua_hook_assign_registers(method, entry, error)) return luaL_error(L, "%s", error.c_str());
    entry.label = lua_class_full_name(bnm_method_class(method)) + "::" + method->name;

    lua_pushvalue(L, 4);
    entry.callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    if (!reuse) {
        dobby_dummy_func_t replacement = (dobby_dummy_func_t)g_lua_hook_word_thunks[slot];
        if (return_kind == value_kind::float32) replacement = (dobby_dummy_func_t)g_lua_hook_float_thunks[slot];
        else if (return_kind == value_kind::float64) replacement = (dobby_dummy_func_t)g_lua_hook_double_thunks[slot];
        else if (kind_is_vector(return_kind)) replacement = (dobby_dummy_func_t)g_lua_hook_vec_thunks[slot];
        dobby_dummy_func_t original = nullptr;
        int status = DobbyHook(address, replacement, &original);
        if (status != RT_SUCCESS || !original) {
            luaL_unref(L, LUA_REGISTRYINDEX, entry.callback_ref);
            entry.callback_ref = LUA_NOREF;
            return luaL_error(L, "DobbyHook failed (%d)", status);
        }
        entry.address = address;
        entry.original = (void*)original;
        entry.installed = true;
    }
    entry.calls.store(0);
    entry.active.store(true, std::memory_order_release);
    lua_pushinteger(L, slot + 1);
    return 1;
}

static lua_hook_entry* lua_hook_by_id(int id) {
    return id >= 1 && id <= lua_hook_slot_count && g_lua_hooks[id - 1].installed ? &g_lua_hooks[id - 1] : nullptr;
}

static void lua_hook_deactivate(lua_hook_entry& entry) {
    entry.active.store(false, std::memory_order_release);
    if (g_rt.state && entry.callback_ref != LUA_NOREF) luaL_unref(g_rt.state, LUA_REGISTRYINDEX, entry.callback_ref);
    entry.callback_ref = LUA_NOREF;
}

static int lua_hooks_deactivate_all() {
    int disabled = 0;
    for (auto& entry : g_lua_hooks) {
        if (!entry.installed || !entry.active.load()) continue;
        lua_hook_deactivate(entry);
        ++disabled;
    }
    return disabled;
}

static int l_unhook_method(lua_State* L) {
    lua_hook_entry* entry = lua_hook_by_id((int)luaL_checkinteger(L, 1));
    std::lock_guard<std::recursive_mutex> lock(g_rt.mtx);
    bool was_active = entry && entry->active.load();
    if (was_active) lua_hook_deactivate(*entry);
    lua_pushboolean(L, was_active);
    return 1;
}

static int l_unhook_all_methods(lua_State* L) {
    std::lock_guard<std::recursive_mutex> lock(g_rt.mtx);
    lua_pushinteger(L, lua_hooks_deactivate_all());
    return 1;
}

static int l_has_method_hook(lua_State* L) {
    lua_hook_entry* entry = lua_hook_by_id((int)luaL_checkinteger(L, 1));
    lua_pushboolean(L, entry && entry->active.load());
    return 1;
}

static int l_hook_list(lua_State* L) {
    lua_newtable(L);
    int row = 0;
    for (int i = 0; i < lua_hook_slot_count; ++i) {
        lua_hook_entry& entry = g_lua_hooks[i];
        if (!entry.installed) continue;
        lua_newtable(L);
        lua_pushinteger(L, i + 1);
        lua_setfield(L, -2, "id");
        lua_pushstring(L, entry.label.c_str());
        lua_setfield(L, -2, "name");
        lua_pushboolean(L, entry.active.load());
        lua_setfield(L, -2, "active");
        lua_pushnumber(L, (double)entry.calls.load());
        lua_setfield(L, -2, "calls");
        lua_rawseti(L, -2, ++row);
    }
    return 1;
}

static int l_hook_caller(lua_State* L) {
    if (!g_lua_hook_current) return luaL_error(L, "hook_caller can only be used inside a hook callback");
    symbols_ensure();
    lua_pushstring(L, symbols_describe(g_lua_hook_current_caller).c_str());
    return 1;
}

static const char* lua_hook_kind_label(value_kind k) {
    switch (k) {
        case value_kind::nothing: return "void";
        case value_kind::boolean: return "bool";
        case value_kind::int8: return "int8";
        case value_kind::uint8: return "uint8";
        case value_kind::int16: return "int16";
        case value_kind::uint16: return "uint16";
        case value_kind::int32: return "int32";
        case value_kind::uint32: return "uint32";
        case value_kind::int64: return "int64";
        case value_kind::uint64: return "uint64";
        case value_kind::float32: return "float";
        case value_kind::float64: return "double";
        case value_kind::string: return "string";
        case value_kind::vec2: return "vec2";
        case value_kind::vec3: return "vec3";
        case value_kind::vec4: return "vec4";
        case value_kind::color: return "color";
        case value_kind::enumeration: return "enum";
        case value_kind::object: return "object";
        default: return "unsupported";
    }
}

static lua_hook_entry* lua_hook_check_entry(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    if (id < 1 || id > lua_hook_slot_count || !g_lua_hooks[id - 1].installed)
        return nullptr;
    return &g_lua_hooks[id - 1];
}

static int l_set_hook_callback(lua_State* L) {
    luaL_checktype(L, 2, LUA_TFUNCTION);
    std::lock_guard<std::recursive_mutex> lock(g_rt.mtx);
    lua_hook_entry* entry = lua_hook_check_entry(L);
    if (!entry) return luaL_error(L, "hook %d is not installed", (int)luaL_checkinteger(L, 1));
    lua_pushvalue(L, 2);
    int new_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    if (entry->callback_ref != LUA_NOREF) luaL_unref(L, LUA_REGISTRYINDEX, entry->callback_ref);
    entry->callback_ref = new_ref;
    entry->calls.store(0);
    entry->active.store(true, std::memory_order_release);
    lua_pushboolean(L, 1);
    return 1;
}

static int l_get_hook(lua_State* L) {
    lua_hook_entry* entry = lua_hook_check_entry(L);
    if (!entry) {
        lua_pushnil(L);
        return 1;
    }
    lua_newtable(L);
    lua_pushinteger(L, (lua_Integer)(entry - g_lua_hooks) + 1);
    lua_setfield(L, -2, "id");
    lua_pushstring(L, entry->label.c_str());
    lua_setfield(L, -2, "name");
    lua_pushboolean(L, entry->active.load());
    lua_setfield(L, -2, "active");
    lua_pushnumber(L, (double)entry->calls.load());
    lua_setfield(L, -2, "calls");
    lua_pushboolean(L, entry->is_static);
    lua_setfield(L, -2, "is_static");
    lua_pushinteger(L, entry->param_count);
    lua_setfield(L, -2, "param_count");
    lua_pushstring(L, lua_hook_kind_label(entry->return_kind));
    lua_setfield(L, -2, "return_type");
    char addr_buf[32];
    snprintf(addr_buf, sizeof(addr_buf), "0x%llX", (unsigned long long)(uintptr_t)entry->address);
    lua_pushstring(L, addr_buf);
    lua_setfield(L, -2, "address");
    return 1;
}

static void lua_hook_fill_regs(lua_State* L, const lua_hook_entry& entry, lua_hook_regs& regs) {
    int index = 1;
    if (!entry.is_static) regs.x[0] = lua_hook_read_word(L, index++, value_kind::object);
    for (int i = 0; i < entry.param_count; ++i, ++index) {
        const lua_hook_param& p = entry.params[i];
        if (index > lua_gettop(L)) luaL_error(L, "hook_original: missing argument %d", i + 1);
        if (!p.in_fp) {
            if (p.byref) {
                if (lua_isnumber(L, index)) regs.x[p.reg] = (uint64_t)lua_tonumber(L, index);
                else {
                    bool is_ref = false;
                    regs.x[p.reg] = ref_address_at(L, index, is_ref);
                }
            } else {
                regs.x[p.reg] = lua_hook_read_word(L, index, p.kind);
            }
        } else if (p.kind == value_kind::float32) {
            regs.d[p.reg] = lua_hook_float_to_reg((float)luaL_checknumber(L, index));
        } else if (p.kind == value_kind::float64) {
            regs.d[p.reg] = luaL_checknumber(L, index);
        } else {
            vec_value v{};
            if (!read_vec(L, index, v)) luaL_error(L, "hook_original: argument %d must be a vector", i + 1);
            for (int k = 0; k < kind_components(p.kind); ++k) regs.d[p.reg + k] = lua_hook_float_to_reg(v.c[k]);
        }
    }
}

static int l_hook_original(lua_State* L) {
    lua_hook_entry* entry = g_lua_hook_current;
    if (!entry || !g_lua_hook_current_regs) return luaL_error(L, "hook_original can only be used inside a hook callback");
    lua_hook_regs regs = *g_lua_hook_current_regs;
    if (lua_gettop(L) > 0) lua_hook_fill_regs(L, *entry, regs);
    if (kind_is_vector(entry->return_kind)) {
        lua_hook_vec_ret ret = lua_hook_call_original<lua_hook_vec_ret>(entry->original, regs);
        push_vec(L, ret.v, kind_components(entry->return_kind), entry->return_kind == value_kind::color);
        return 1;
    }
    switch (entry->return_kind) {
        case value_kind::nothing:
            lua_hook_call_original<void>(entry->original, regs);
            lua_pushnil(L);
            break;
        case value_kind::float32:
            lua_pushnumber(L, lua_hook_call_original<float>(entry->original, regs));
            break;
        case value_kind::float64:
            lua_pushnumber(L, lua_hook_call_original<double>(entry->original, regs));
            break;
        default:
            lua_hook_push_word(L, entry->return_kind, lua_hook_call_original<uint64_t>(entry->original, regs));
            break;
    }
    return 1;
}

static void register_lua_hooks(lua_State* L) {
    static const luaL_Reg functions[] = {
        {"hook_method", l_hook_method},
        {"unhook_method", l_unhook_method},
        {"disable_method_hook", l_unhook_method},
        {"unhook_all_methods", l_unhook_all_methods},
        {"has_method_hook", l_has_method_hook},
        {"hook_list", l_hook_list},
        {"hook_caller", l_hook_caller},
        {"hook_original", l_hook_original},
        {"set_hook_callback", l_set_hook_callback},
        {"get_hook", l_get_hook},
        {nullptr, nullptr}
    };
    for (const luaL_Reg* f = functions; f->name; ++f) {
        lua_pushcfunction(L, f->func);
        lua_setglobal(L, f->name);
    }
    lua_pushlightuserdata(L, &g_lua_hook_null_marker);
    lua_setglobal(L, "HOOK_NULL");
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
