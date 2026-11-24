#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <utility>
#include <dobby.h>
#include "lua_runtime.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

static constexpr int lua_hook_slot_count = 32;
static constexpr int lua_hook_max_args = 8;

struct lua_hook_entry {
    std::atomic<bool> active{false};
    bool installed = false;
    MethodInfo* method = nullptr;
    void* address = nullptr;
    void* original = nullptr;
    int callback_ref = LUA_NOREF;
    int native_arg_count = 0;
    bool is_static = false;
    value_kind return_kind = value_kind::unsupported;
    value_kind arg_kinds[lua_hook_max_args]{};
};

struct lua_hook_result {
    bool call_original = true;
    uint64_t word = 0;
    float f32 = 0.0f;
    double f64 = 0.0;
};

static lua_hook_entry g_lua_hook_entries[lua_hook_slot_count];
static std::mutex g_lua_hook_install_mutex;
static int g_lua_hook_null_marker = 0;

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

static bool lua_hook_is_arg_kind(value_kind kind) {
    return lua_hook_is_word_kind(kind);
}

static void lua_hook_push_arg(lua_State* L, value_kind kind, uint64_t raw) {
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
            if (lua_islightuserdata(L, index) && lua_touserdata(L, index) == &g_lua_hook_null_marker) return 0;
            if (lua_isnil(L, index)) return 0;
            if (lua_type(L, index) == LUA_TSTRING) {
                auto* value = CreateMonoString(lua_tostring(L, index));
                return (uint64_t)(uintptr_t)value;
            }
            return 0;
        case value_kind::object: {
            if (lua_islightuserdata(L, index) && lua_touserdata(L, index) == &g_lua_hook_null_marker) return 0;
            if (lua_isnil(L, index)) return 0;
            auto* ref = (lua_ref*)test_udata(L, index, mt_ref);
            if (ref) return (uint64_t)ref->inst;
            if (lua_islightuserdata(L, index)) return (uint64_t)(uintptr_t)lua_touserdata(L, index);
            if (lua_isnumber(L, index)) return (uint64_t)lua_tonumber(L, index);
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

static lua_hook_result lua_hook_dispatch(int slot, const uint64_t* raw_args) {
    lua_hook_result result{};
    result.call_original = true;
    if (slot < 0 || slot >= lua_hook_slot_count) return result;
    lua_hook_entry& entry = g_lua_hook_entries[slot];
    if (!entry.installed || !entry.active.load(std::memory_order_acquire)) return result;

    std::lock_guard<std::recursive_mutex> lock(g_rt.mtx);
    if (!entry.active.load(std::memory_order_acquire) || !g_rt.state || entry.callback_ref == LUA_NOREF)
        return result;

    BNM::AttachIl2Cpp();
    lua_State* L = g_rt.state;
    int base = lua_gettop(L);
    double previous_deadline = g_rt.deadline;
    lua_rawgeti(L, LUA_REGISTRYINDEX, entry.callback_ref);
    for (int i = 0; i < entry.native_arg_count; ++i) {
        if (!entry.is_static && i == 0) {
            push_object(L, (Il2CppObject*)(uintptr_t)raw_args[i]);
        } else {
            int parameter_index = i - (entry.is_static ? 0 : 1);
            lua_hook_push_arg(L, entry.arg_kinds[parameter_index], raw_args[i]);
        }
    }

    g_rt.deadline = rt_now() + callback_timeout_seconds;
    int call_status = lua_pcall(L, entry.native_arg_count, 1, 0);
    g_rt.deadline = previous_deadline;

    if (call_status != 0) {
        const char* error = lua_tostring(L, -1);
        rt_append_output(std::string("[Hook ") + std::to_string(slot + 1) + "] " + (error ? error : "Lua callback failed"));
        lua_settop(L, base);
        return result;
    }

    int value_index = lua_gettop(L);
    if (entry.return_kind == value_kind::nothing) {
        bool allow_original = (lua_isboolean(L, value_index) && lua_toboolean(L, value_index)) || lua_hook_is_original_token(L, value_index);
        result.call_original = allow_original;
    } else if (lua_isnil(L, value_index) || lua_hook_is_original_token(L, value_index)) {
        result.call_original = true;
    } else {
        result.call_original = false;
        if (entry.return_kind == value_kind::float32) {
            result.f32 = (float)lua_tonumber(L, value_index);
        } else if (entry.return_kind == value_kind::float64) {
            result.f64 = (double)lua_tonumber(L, value_index);
        } else {
            result.word = lua_hook_read_word(L, value_index, entry.return_kind);
        }
    }

    lua_settop(L, base);
    return result;
}

using lua_hook_word_fn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
using lua_hook_float_fn = float (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
using lua_hook_double_fn = double (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
using lua_hook_void_fn = void (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

static uint64_t lua_hook_call_original_word(void* original, const uint64_t* a) {
    auto fn = (lua_hook_word_fn)original;
    return fn ? fn(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]) : 0;
}

static void lua_hook_call_original_void(void* original, const uint64_t* a) {
    auto fn = (lua_hook_void_fn)original;
    if (fn) fn(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
}

static float lua_hook_call_original_float(void* original, const uint64_t* a) {
    auto fn = (lua_hook_float_fn)original;
    return fn ? fn(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]) : 0.0f;
}

static double lua_hook_call_original_double(void* original, const uint64_t* a) {
    auto fn = (lua_hook_double_fn)original;
    return fn ? fn(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]) : 0.0;
}

template<int Slot>
static uint64_t lua_hook_word(uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7) {
    uint64_t args[8] = {a0, a1, a2, a3, a4, a5, a6, a7};
    lua_hook_entry& entry = g_lua_hook_entries[Slot];
    if (!entry.installed) return 0;
    lua_hook_result result = lua_hook_dispatch(Slot, args);
    if (result.call_original) {
        if (entry.return_kind == value_kind::nothing) {
            lua_hook_call_original_void(entry.original, args);
            return 0;
        }
        return lua_hook_call_original_word(entry.original, args);
    }
    return result.word;
}

template<int Slot>
static float lua_hook_float(uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7) {
    uint64_t args[8] = {a0, a1, a2, a3, a4, a5, a6, a7};
    lua_hook_entry& entry = g_lua_hook_entries[Slot];
    if (!entry.installed) return 0.0f;
    lua_hook_result result = lua_hook_dispatch(Slot, args);
    return result.call_original ? lua_hook_call_original_float(entry.original, args) : result.f32;
}

template<int Slot>
static double lua_hook_double(uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7) {
    uint64_t args[8] = {a0, a1, a2, a3, a4, a5, a6, a7};
    lua_hook_entry& entry = g_lua_hook_entries[Slot];
    if (!entry.installed) return 0.0;
    lua_hook_result result = lua_hook_dispatch(Slot, args);
    return result.call_original ? lua_hook_call_original_double(entry.original, args) : result.f64;
}

template<int... I>
static constexpr std::array<lua_hook_word_fn, sizeof...(I)> lua_hook_make_word_table(std::integer_sequence<int, I...>) {
    return {{lua_hook_word<I>...}};
}

template<int... I>
static constexpr std::array<lua_hook_float_fn, sizeof...(I)> lua_hook_make_float_table(std::integer_sequence<int, I...>) {
    return {{lua_hook_float<I>...}};
}

template<int... I>
static constexpr std::array<lua_hook_double_fn, sizeof...(I)> lua_hook_make_double_table(std::integer_sequence<int, I...>) {
    return {{lua_hook_double<I>...}};
}

static const auto g_lua_hook_word_thunks = lua_hook_make_word_table(std::make_integer_sequence<int, lua_hook_slot_count>{});
static const auto g_lua_hook_float_thunks = lua_hook_make_float_table(std::make_integer_sequence<int, lua_hook_slot_count>{});
static const auto g_lua_hook_double_thunks = lua_hook_make_double_table(std::make_integer_sequence<int, lua_hook_slot_count>{});

static int l_hook_method(lua_State* L) {
    const char* class_name = luaL_checkstring(L, 1);
    const char* method_name = luaL_checkstring(L, 2);
    int parameter_count = (int)luaL_checkinteger(L, 3);
    luaL_checktype(L, 4, LUA_TFUNCTION);
    const char* assembly_name = lua_gettop(L) >= 5 && lua_isstring(L, 5) ? lua_tostring(L, 5) : "";

#if !defined(__aarch64__) && !defined(__arm64__)
    return luaL_error(L, "hook_method currently supports Android ARM64 only");
#else
    if (parameter_count < 0 || parameter_count > lua_hook_max_args - 1)
        return luaL_error(L, "parameter count must be between 0 and %d", lua_hook_max_args - 1);

    BNM::AttachIl2Cpp();
    Il2CppClass* klass = find_class_cached(class_name, assembly_name);
    if (!klass) return luaL_error(L, "class '%s' not found", class_name);

    const auto& candidates = find_candidates(klass, method_name, parameter_count);
    if (candidates.empty()) return luaL_error(L, "method '%s' with %d parameters not found", method_name, parameter_count);
    if (candidates.size() != 1) return luaL_error(L, "method '%s' has multiple overloads with %d parameters", method_name, parameter_count);

    MethodInfo* method = candidates.front();
    if (!method || !method->methodPointer || method->is_generic)
        return luaL_error(L, "method has no native body or is generic");

    bool is_static = (method->flags & 0x10) != 0;
    int native_count = parameter_count + (is_static ? 0 : 1);
    if (native_count >= lua_hook_max_args) return luaL_error(L, "method has too many native arguments for the ARM64 thunk");

    value_kind return_kind = kind_from_type(method->return_type);
    if (!(return_kind == value_kind::nothing || lua_hook_is_word_kind(return_kind) || return_kind == value_kind::float32 || return_kind == value_kind::float64))
        return luaL_error(L, "unsupported return type for hook_method");

    value_kind parameter_kinds[lua_hook_max_args]{};
    for (int i = 0; i < parameter_count; ++i) {
        if (!method->parameters) return luaL_error(L, "method parameter metadata is unavailable");
        parameter_kinds[i] = kind_from_type(bnm_method_parameter_type(method, static_cast<uint32_t>(i)));
        if (!lua_hook_is_arg_kind(parameter_kinds[i]))
            return luaL_error(L, "parameter %d has unsupported ABI type", i + 1);
    }

    std::unique_lock<std::mutex> install_lock(g_lua_hook_install_mutex);
    int slot = -1;
    for (int i = 0; i < lua_hook_slot_count; ++i) {
        if (!g_lua_hook_entries[i].installed) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        install_lock.unlock();
        return luaL_error(L, "hook slot limit reached (%d)", lua_hook_slot_count);
    }

    for (int i = 0; i < lua_hook_slot_count; ++i) {
        if (g_lua_hook_entries[i].installed && g_lua_hook_entries[i].address == (void*)method->methodPointer) {
            install_lock.unlock();
            return luaL_error(L, "method is already hooked");
        }
    }

    lua_hook_entry& entry = g_lua_hook_entries[slot];
    entry.method = method;
    entry.address = (void*)method->methodPointer;
    entry.native_arg_count = native_count;
    entry.is_static = is_static;
    entry.return_kind = return_kind;
    for (int i = 0; i < parameter_count; ++i) entry.arg_kinds[i] = parameter_kinds[i];
    lua_pushvalue(L, 4);
    entry.callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    dobby_dummy_func_t replacement = (dobby_dummy_func_t)g_lua_hook_word_thunks[slot];
    if (return_kind == value_kind::float32) replacement = (dobby_dummy_func_t)g_lua_hook_float_thunks[slot];
    else if (return_kind == value_kind::float64) replacement = (dobby_dummy_func_t)g_lua_hook_double_thunks[slot];

    dobby_dummy_func_t original = nullptr;
    int status = DobbyHook(entry.address, replacement, &original);
    if (status != RT_SUCCESS || !original) {
        luaL_unref(L, LUA_REGISTRYINDEX, entry.callback_ref);
        entry.callback_ref = LUA_NOREF;
        entry.method = nullptr;
        entry.address = nullptr;
        entry.original = nullptr;
        entry.native_arg_count = 0;
        install_lock.unlock();
        return luaL_error(L, "DobbyHook failed (%d)", status);
    }

    entry.original = (void*)original;
    entry.installed = true;
    entry.active.store(true, std::memory_order_release);
    lua_pushinteger(L, slot + 1);
    return 1;
#endif
}

static int l_unhook_method(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    if (id < 1 || id > lua_hook_slot_count) {
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_hook_entry& entry = g_lua_hook_entries[id - 1];
    if (!entry.installed || !entry.active.load()) {
        lua_pushboolean(L, 0);
        return 1;
    }
    std::lock_guard<std::recursive_mutex> lock(g_rt.mtx);
    entry.active.store(false, std::memory_order_release);
    if (g_rt.state && entry.callback_ref != LUA_NOREF) luaL_unref(g_rt.state, LUA_REGISTRYINDEX, entry.callback_ref);
    entry.callback_ref = LUA_NOREF;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_unhook_all_methods(lua_State* L) {
    std::lock_guard<std::recursive_mutex> lock(g_rt.mtx);
    int disabled = 0;
    for (auto& entry : g_lua_hook_entries) {
        if (!entry.installed || !entry.active.exchange(false)) continue;
        if (g_rt.state && entry.callback_ref != LUA_NOREF) luaL_unref(g_rt.state, LUA_REGISTRYINDEX, entry.callback_ref);
        entry.callback_ref = LUA_NOREF;
        ++disabled;
    }
    lua_pushinteger(L, disabled);
    return 1;
}

static int l_has_method_hook(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    bool active = id >= 1 && id <= lua_hook_slot_count && g_lua_hook_entries[id - 1].installed && g_lua_hook_entries[id - 1].active.load();
    lua_pushboolean(L, active);
    return 1;
}

static void register_lua_hooks(lua_State* L) {
    lua_pushcfunction(L, l_hook_method);
    lua_setglobal(L, "hook_method");
    lua_pushcfunction(L, l_unhook_method);
    lua_setglobal(L, "unhook_method");
    lua_pushcfunction(L, l_unhook_method);
    lua_setglobal(L, "disable_method_hook");
    lua_pushcfunction(L, l_unhook_all_methods);
    lua_setglobal(L, "unhook_all_methods");
    lua_pushcfunction(L, l_has_method_hook);
    lua_setglobal(L, "has_method_hook");
    lua_pushlightuserdata(L, &g_lua_hook_null_marker);
    lua_setglobal(L, "HOOK_NULL");
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
