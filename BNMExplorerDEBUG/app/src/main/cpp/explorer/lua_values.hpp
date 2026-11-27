#pragma once
#include <string>
#include <unordered_map>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

#include "autodetect/variant_namespace.hpp"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

using namespace BNM;
using namespace BNM::IL2CPP;
using namespace BNM::Structures::Unity;

enum class value_kind {
    unknown,
    nothing,
    boolean,
    int8,
    uint8,
    int16,
    uint16,
    int32,
    uint32,
    int64,
    uint64,
    float32,
    float64,
    string,
    vec2,
    vec3,
    vec4,
    color,
    enumeration,
    object,
    unsupported
};

struct value_slot {
    alignas(16) uint8_t bytes[16];
};

struct vec_value {
    float c[4];
    int n;
    bool is_color;
};

static const int max_call_args = 16;
static const char* mt_vec = "luabnm.vec";
static const char* mt_ref = "luabnm.ref";
static const char* mt_method = "luabnm.method";
static const char* vec_keys_xyzw[4] = {"x", "y", "z", "w"};
static const char* vec_keys_rgba[4] = {"r", "g", "b", "a"};

static void push_object(lua_State* L, Il2CppObject* obj);
static uintptr_t ref_address_at(lua_State* L, int idx, bool& is_ref);

static int absolute_index(lua_State* L, int idx) {
    return (idx < 0 && idx > LUA_REGISTRYINDEX) ? lua_gettop(L) + idx + 1 : idx;
}

static std::string type_full_name(const Il2CppType* t) {
    if (!t) return "";
    try {
        Class c(t);
        if (!c.IsValid()) return "";
        auto* k = c.GetClass();
        if (!k || !k->name) return "";
        std::string n = k->name;
        if (k->namespaze && k->namespaze[0] != '\0') n = std::string(k->namespaze) + "." + n;
        return n;
    } catch (...) {
        return "";
    }
}

static value_kind kind_from_name(const std::string& n) {
    static const std::unordered_map<std::string, value_kind> table = {
        {"System.Void", value_kind::nothing},
        {"System.Boolean", value_kind::boolean},
        {"System.SByte", value_kind::int8},
        {"System.Byte", value_kind::uint8},
        {"System.Int16", value_kind::int16},
        {"System.UInt16", value_kind::uint16},
        {"System.Char", value_kind::uint16},
        {"System.Int32", value_kind::int32},
        {"System.UInt32", value_kind::uint32},
        {"System.Int64", value_kind::int64},
        {"System.UInt64", value_kind::uint64},
        {"System.IntPtr", value_kind::uint64},
        {"System.UIntPtr", value_kind::uint64},
        {"System.Single", value_kind::float32},
        {"System.Double", value_kind::float64},
        {"System.String", value_kind::string},
        {"System.Object", value_kind::object},
        {"UnityEngine.Vector2", value_kind::vec2},
        {"UnityEngine.Vector3", value_kind::vec3},
        {"UnityEngine.Vector4", value_kind::vec4},
        {"UnityEngine.Quaternion", value_kind::vec4},
        {"UnityEngine.Color", value_kind::color}
    };
    auto it = table.find(n);
    return it == table.end() ? value_kind::unknown : it->second;
}

static value_kind kind_from_type(const Il2CppType* t) {
    static std::unordered_map<const Il2CppType*, value_kind> cache;
    if (!t) return value_kind::unsupported;
    auto it = cache.find(t);
    if (it != cache.end()) return it->second;
    value_kind k = kind_from_name(type_full_name(t));
    if (k == value_kind::unknown) {
        Class c(t);
        Il2CppClass* kl = c.IsValid() ? c.GetClass() : nullptr;
        auto& m = BNM::Internal::il2cppMethods;
        if (kl && m.il2cpp_class_is_enum && m.il2cpp_class_is_enum(kl)) k = value_kind::enumeration;
        else if (kl && m.il2cpp_class_is_valuetype && m.il2cpp_class_is_valuetype(kl)) k = value_kind::unsupported;
        else k = value_kind::object;
    }
    if (t->byref && k != value_kind::nothing) k = value_kind::unsupported;
    cache[t] = k;
    return k;
}

static size_t kind_size(value_kind k) {
    switch (k) {
        case value_kind::boolean:
        case value_kind::int8:
        case value_kind::uint8: return 1;
        case value_kind::int16:
        case value_kind::uint16: return 2;
        case value_kind::int32:
        case value_kind::uint32:
        case value_kind::float32:
        case value_kind::enumeration: return 4;
        case value_kind::int64:
        case value_kind::uint64:
        case value_kind::float64:
        case value_kind::string:
        case value_kind::object:
        case value_kind::vec2: return 8;
        case value_kind::vec3: return 12;
        case value_kind::vec4:
        case value_kind::color: return 16;
        default: return 0;
    }
}

static bool kind_is_numeric(value_kind k) {
    switch (k) {
        case value_kind::int8:
        case value_kind::uint8:
        case value_kind::int16:
        case value_kind::uint16:
        case value_kind::int32:
        case value_kind::uint32:
        case value_kind::int64:
        case value_kind::uint64:
        case value_kind::float32:
        case value_kind::float64:
        case value_kind::enumeration: return true;
        default: return false;
    }
}

static bool kind_is_vector(value_kind k) {
    return k == value_kind::vec2 || k == value_kind::vec3 || k == value_kind::vec4 || k == value_kind::color;
}

static bool address_plausible(uintptr_t p) {
    return p >= 0x10000 && (p & 0x00FFFFFFFFFFFFFFULL) < 0x0001000000000000ULL;
}

struct value_pipe_holder {
    int fds[2] = {-1, -1};
    bool failed = false;
    ~value_pipe_holder() {
        if (fds[0] >= 0) close(fds[0]);
        if (fds[1] >= 0) close(fds[1]);
    }
};

static bool is_readable_bytes(const void* ptr, size_t size) {
    if (!ptr || size == 0 || size > 4096) return false;
    if (!address_plausible((uintptr_t)ptr)) return false;
    static thread_local value_pipe_holder holder;
    if (holder.failed) return false;
    if (holder.fds[0] < 0) {
        if (pipe2(holder.fds, O_CLOEXEC | O_NONBLOCK) != 0) {
            holder.failed = true;
            return true;
        }
    }
    ssize_t w = write(holder.fds[1], ptr, size);
    if (w < 0) return false;
    char sink[256];
    ssize_t left = w;
    while (left > 0) {
        ssize_t r = read(holder.fds[0], sink, (size_t)left < sizeof(sink) ? (size_t)left : sizeof(sink));
        if (r <= 0) break;
        left -= r;
    }
    return (size_t)w == size;
}

static const size_t max_struct_bytes = 1024;

static size_t struct_data_size(const Il2CppType* t) {
    if (!t || !bnm_type_is_value_type(t)) return 0;
    try {
        Class c(t);
        auto* k = c.IsValid() ? c.GetClass() : nullptr;
        if (!k || k->instance_size <= sizeof(Il2CppObject)) return 0;
        size_t n = (size_t)k->instance_size - sizeof(Il2CppObject);
        return n <= max_struct_bytes ? n : 0;
    } catch (...) {
        return 0;
    }
}

enum class arg_shape {
    normal,
    byref,
    inplace
};

static arg_shape arg_shape_of(const Il2CppType* t) {
    if (!t) return arg_shape::normal;
    if (t->byref) return arg_shape::byref;
    value_kind k = kind_from_type(t);
    if ((k == value_kind::unsupported || k == value_kind::unknown) && bnm_type_is_value_type(t)) return arg_shape::inplace;
    return arg_shape::normal;
}

static int kind_components(value_kind k) {
    switch (k) {
        case value_kind::vec2: return 2;
        case value_kind::vec3: return 3;
        case value_kind::vec4:
        case value_kind::color: return 4;
        default: return 0;
    }
}

static bool read_vec(lua_State* L, int idx, vec_value& out) {
    idx = absolute_index(L, idx);
    out = {};
    if (!lua_istable(L, idx)) return false;
    lua_getfield(L, idx, "r");
    bool color = lua_isnumber(L, -1) != 0;
    lua_pop(L, 1);
    const char** keys = color ? vec_keys_rgba : vec_keys_xyzw;
    int count = 0;
    for (int i = 0; i < 4; i++) {
        lua_getfield(L, idx, keys[i]);
        if (lua_isnumber(L, -1)) {
            out.c[i] = (float)lua_tonumber(L, -1);
            count = i + 1;
        }
        lua_pop(L, 1);
    }
    if (count == 0) {
        for (int i = 0; i < 4; i++) {
            lua_rawgeti(L, idx, i + 1);
            if (lua_isnumber(L, -1)) {
                out.c[i] = (float)lua_tonumber(L, -1);
                count = i + 1;
            }
            lua_pop(L, 1);
        }
    }
    out.n = count;
    out.is_color = color;
    return count > 0;
}

static void push_vec(lua_State* L, const float* c, int n, bool is_color) {
    lua_createtable(L, 0, n);
    const char** keys = is_color ? vec_keys_rgba : vec_keys_xyzw;
    for (int i = 0; i < n; i++) {
        lua_pushnumber(L, c[i]);
        lua_setfield(L, -2, keys[i]);
    }
    luaL_getmetatable(L, mt_vec);
    lua_setmetatable(L, -2);
}

static int vec_arith(lua_State* L, char op) {
    vec_value a, b;
    bool a_vec = read_vec(L, 1, a);
    bool b_vec = read_vec(L, 2, b);
    float out[4] = {0, 0, 0, 0};
    int n = 0;
    bool is_color = false;
    if (a_vec && b_vec) {
        n = a.n < b.n ? a.n : b.n;
        is_color = a.is_color;
        for (int i = 0; i < n; i++) {
            switch (op) {
                case '+': out[i] = a.c[i] + b.c[i]; break;
                case '-': out[i] = a.c[i] - b.c[i]; break;
                case '*': out[i] = a.c[i] * b.c[i]; break;
                default: out[i] = b.c[i] != 0.0f ? a.c[i] / b.c[i] : 0.0f; break;
            }
        }
    } else if (a_vec && lua_isnumber(L, 2) && (op == '*' || op == '/')) {
        float s = (float)lua_tonumber(L, 2);
        n = a.n;
        is_color = a.is_color;
        for (int i = 0; i < n; i++) out[i] = op == '*' ? a.c[i] * s : (s != 0.0f ? a.c[i] / s : 0.0f);
    } else if (b_vec && lua_isnumber(L, 1) && op == '*') {
        float s = (float)lua_tonumber(L, 1);
        n = b.n;
        is_color = b.is_color;
        for (int i = 0; i < n; i++) out[i] = b.c[i] * s;
    } else {
        return luaL_error(L, "invalid vector operation");
    }
    push_vec(L, out, n, is_color);
    return 1;
}

static int l_vec_add(lua_State* L) { return vec_arith(L, '+'); }
static int l_vec_sub(lua_State* L) { return vec_arith(L, '-'); }
static int l_vec_mul(lua_State* L) { return vec_arith(L, '*'); }
static int l_vec_div(lua_State* L) { return vec_arith(L, '/'); }

static int l_vec_unm(lua_State* L) {
    vec_value a;
    if (!read_vec(L, 1, a)) return luaL_error(L, "vector expected");
    float out[4] = {0, 0, 0, 0};
    for (int i = 0; i < a.n; i++) out[i] = -a.c[i];
    push_vec(L, out, a.n, a.is_color);
    return 1;
}

static int l_vec_eq(lua_State* L) {
    vec_value a, b;
    if (!read_vec(L, 1, a) || !read_vec(L, 2, b) || a.n != b.n) {
        lua_pushboolean(L, 0);
        return 1;
    }
    for (int i = 0; i < a.n; i++) {
        if (a.c[i] != b.c[i]) {
            lua_pushboolean(L, 0);
            return 1;
        }
    }
    lua_pushboolean(L, 1);
    return 1;
}

static int l_vec_tostring(lua_State* L) {
    vec_value a;
    if (!read_vec(L, 1, a)) {
        lua_pushstring(L, "vec(?)");
        return 1;
    }
    char buf[128];
    int len = snprintf(buf, sizeof(buf), "(");
    for (int i = 0; i < a.n && len < (int)sizeof(buf) - 24; i++)
        len += snprintf(buf + len, sizeof(buf) - len, i ? ", %g" : "%g", a.c[i]);
    snprintf(buf + len, sizeof(buf) - len, ")");
    lua_pushstring(L, buf);
    return 1;
}

static int l_vec_length(lua_State* L) {
    vec_value a;
    if (!read_vec(L, 1, a)) return luaL_error(L, "vector expected");
    double sum = 0;
    for (int i = 0; i < a.n; i++) sum += (double)a.c[i] * a.c[i];
    lua_pushnumber(L, std::sqrt(sum));
    return 1;
}

static int l_vec_normalized(lua_State* L) {
    vec_value a;
    if (!read_vec(L, 1, a)) return luaL_error(L, "vector expected");
    double sum = 0;
    for (int i = 0; i < a.n; i++) sum += (double)a.c[i] * a.c[i];
    double len = std::sqrt(sum);
    float out[4] = {0, 0, 0, 0};
    for (int i = 0; i < a.n; i++) out[i] = len > 0 ? (float)(a.c[i] / len) : 0.0f;
    push_vec(L, out, a.n, a.is_color);
    return 1;
}

static int l_vec_dot(lua_State* L) {
    vec_value a, b;
    if (!read_vec(L, 1, a) || !read_vec(L, 2, b)) return luaL_error(L, "vector expected");
    int n = a.n < b.n ? a.n : b.n;
    double sum = 0;
    for (int i = 0; i < n; i++) sum += (double)a.c[i] * b.c[i];
    lua_pushnumber(L, sum);
    return 1;
}

static int l_vec_distance(lua_State* L) {
    vec_value a, b;
    if (!read_vec(L, 1, a) || !read_vec(L, 2, b)) return luaL_error(L, "vector expected");
    int n = a.n < b.n ? a.n : b.n;
    double sum = 0;
    for (int i = 0; i < n; i++) {
        double d = (double)a.c[i] - b.c[i];
        sum += d * d;
    }
    lua_pushnumber(L, std::sqrt(sum));
    return 1;
}

static int make_vec_from_args(lua_State* L, int n, bool is_color, float last_default) {
    float c[4] = {0, 0, 0, 0};
    c[n - 1] = last_default;
    int argc = lua_gettop(L);
    for (int i = 0; i < n && i < argc; i++) c[i] = (float)luaL_checknumber(L, i + 1);
    push_vec(L, c, n, is_color);
    return 1;
}

static int l_vec2(lua_State* L) { return make_vec_from_args(L, 2, false, 0.0f); }
static int l_vec3(lua_State* L) { return make_vec_from_args(L, 3, false, 0.0f); }
static int l_vec4(lua_State* L) { return make_vec_from_args(L, 4, false, 0.0f); }
static int l_quat(lua_State* L) { return make_vec_from_args(L, 4, false, 1.0f); }
static int l_color(lua_State* L) { return make_vec_from_args(L, 4, true, 1.0f); }

static void* lua_to_arg(lua_State* L, int idx, value_kind k, value_slot& slot) {
    memset(slot.bytes, 0, sizeof(slot.bytes));
    switch (k) {
        case value_kind::boolean:
            slot.bytes[0] = lua_toboolean(L, idx) ? 1 : 0;
            return slot.bytes;
        case value_kind::int8: {
            int8_t v = (int8_t)luaL_checknumber(L, idx);
            memcpy(slot.bytes, &v, sizeof(v));
            return slot.bytes;
        }
        case value_kind::uint8: {
            uint8_t v = (uint8_t)luaL_checknumber(L, idx);
            memcpy(slot.bytes, &v, sizeof(v));
            return slot.bytes;
        }
        case value_kind::int16: {
            int16_t v = (int16_t)luaL_checknumber(L, idx);
            memcpy(slot.bytes, &v, sizeof(v));
            return slot.bytes;
        }
        case value_kind::uint16: {
            uint16_t v = (uint16_t)luaL_checknumber(L, idx);
            memcpy(slot.bytes, &v, sizeof(v));
            return slot.bytes;
        }
        case value_kind::int32:
        case value_kind::enumeration: {
            int32_t v = (int32_t)luaL_checknumber(L, idx);
            memcpy(slot.bytes, &v, sizeof(v));
            return slot.bytes;
        }
        case value_kind::uint32: {
            uint32_t v = (uint32_t)luaL_checknumber(L, idx);
            memcpy(slot.bytes, &v, sizeof(v));
            return slot.bytes;
        }
        case value_kind::int64: {
            int64_t v = (int64_t)luaL_checknumber(L, idx);
            memcpy(slot.bytes, &v, sizeof(v));
            return slot.bytes;
        }
        case value_kind::uint64: {
            uint64_t v = (uint64_t)luaL_checknumber(L, idx);
            memcpy(slot.bytes, &v, sizeof(v));
            return slot.bytes;
        }
        case value_kind::float32: {
            float v = (float)luaL_checknumber(L, idx);
            memcpy(slot.bytes, &v, sizeof(v));
            return slot.bytes;
        }
        case value_kind::float64: {
            double v = luaL_checknumber(L, idx);
            memcpy(slot.bytes, &v, sizeof(v));
            return slot.bytes;
        }
        case value_kind::string:
            if (lua_isnil(L, idx)) return nullptr;
            return CreateMonoString(luaL_checkstring(L, idx));
        case value_kind::vec2:
        case value_kind::vec3:
        case value_kind::vec4:
        case value_kind::color: {
            vec_value v;
            if (!read_vec(L, idx, v)) luaL_error(L, "argument %d: vector table expected", idx);
            if (k == value_kind::color && v.n < 4) v.c[3] = 1.0f;
            memcpy(slot.bytes, v.c, kind_size(k));
            return slot.bytes;
        }
        case value_kind::object: {
            if (lua_isnil(L, idx)) return nullptr;
            if (lua_type(L, idx) == LUA_TSTRING) return CreateMonoString(lua_tostring(L, idx));
            bool is_ref = false;
            uintptr_t addr = ref_address_at(L, idx, is_ref);
            return (void*)addr;
        }
        default:
            luaL_error(L, "argument %d: unsupported parameter type", idx);
            return nullptr;
    }
}

static void* slot_source(value_kind k, void* arg, value_slot& slot) {
    if (k == value_kind::string || k == value_kind::object) {
        memset(slot.bytes, 0, sizeof(slot.bytes));
        memcpy(slot.bytes, &arg, sizeof(arg));
        return slot.bytes;
    }
    return arg;
}

static void push_from_memory(lua_State* L, value_kind k, const void* mem) {
    switch (k) {
        case value_kind::boolean: lua_pushboolean(L, *(const uint8_t*)mem != 0); break;
        case value_kind::int8: lua_pushnumber(L, *(const int8_t*)mem); break;
        case value_kind::uint8: lua_pushnumber(L, *(const uint8_t*)mem); break;
        case value_kind::int16: lua_pushnumber(L, *(const int16_t*)mem); break;
        case value_kind::uint16: lua_pushnumber(L, *(const uint16_t*)mem); break;
        case value_kind::int32:
        case value_kind::enumeration: lua_pushnumber(L, *(const int32_t*)mem); break;
        case value_kind::uint32: lua_pushnumber(L, *(const uint32_t*)mem); break;
        case value_kind::int64: lua_pushnumber(L, (double)*(const int64_t*)mem); break;
        case value_kind::uint64: lua_pushnumber(L, (double)*(const uint64_t*)mem); break;
        case value_kind::float32: lua_pushnumber(L, *(const float*)mem); break;
        case value_kind::float64: lua_pushnumber(L, *(const double*)mem); break;
        case value_kind::string: {
            auto* s = *(BNM::Structures::Mono::String* const*)mem;
            if (s) lua_pushstring(L, s->str().c_str());
            else lua_pushnil(L);
            break;
        }
        case value_kind::vec2: push_vec(L, (const float*)mem, 2, false); break;
        case value_kind::vec3: push_vec(L, (const float*)mem, 3, false); break;
        case value_kind::vec4: push_vec(L, (const float*)mem, 4, false); break;
        case value_kind::color: push_vec(L, (const float*)mem, 4, true); break;
        case value_kind::object: push_object(L, *(Il2CppObject* const*)mem); break;
        default: lua_pushnil(L); break;
    }
}

static void push_return(lua_State* L, value_kind k, void* ret) {
    if (!ret) {
        lua_pushnil(L);
        return;
    }
    if (k == value_kind::string) {
        auto* s = (BNM::Structures::Mono::String*)ret;
        lua_pushstring(L, s->str().c_str());
        return;
    }
    if (k == value_kind::object) {
        push_object(L, (Il2CppObject*)ret);
        return;
    }
    push_from_memory(L, k, (const uint8_t*)ret + sizeof(Il2CppObject));
}

static bool lua_matches_kind(lua_State* L, int idx, value_kind k, bool strict) {
    int t = lua_type(L, idx);
    if (k == value_kind::boolean) return t == LUA_TBOOLEAN;
    if (kind_is_numeric(k)) return t == LUA_TNUMBER;
    if (kind_is_vector(k)) return t == LUA_TTABLE;
    if (k == value_kind::string) return t == LUA_TSTRING || t == LUA_TNIL;
    if (k == value_kind::object) {
        if (t == LUA_TUSERDATA || t == LUA_TNIL) return true;
        return !strict && (t == LUA_TNUMBER || t == LUA_TSTRING);
    }
    return false;
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
