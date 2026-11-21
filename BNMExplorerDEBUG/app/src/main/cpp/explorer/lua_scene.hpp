#pragma once
#include "lua_objects.hpp"

static Array<Il2CppObject*>* find_objects_array(Il2CppClass* klass, char* err, size_t err_size) {
    err[0] = '\0';
    Il2CppClass* uo = find_class_cached("UnityEngine.Object", "");
    if (!uo) {
        snprintf(err, err_size, "UnityEngine.Object not found");
        return nullptr;
    }
    Il2CppObject* type_obj = (Il2CppObject*)Class(klass).GetMonoType();
    if (!type_obj) {
        snprintf(err, err_size, "type object unavailable");
        return nullptr;
    }
    struct attempt {
        const char* name;
        int argc;
    };
    static const attempt attempts[] = {
        {"FindObjectsOfType", 1},
        {"FindObjectsByType", 1},
        {"FindObjectsByType", 2},
        {"FindObjectsOfType", 2}
    };
    for (const auto& a : attempts) {
        const auto& cand = find_candidates(uo, a.name, a.argc);
        for (auto* mi : cand) {
            if (type_full_name(mi->parameters[0]) != "System.Type") continue;
            value_slot extra;
            memset(extra.bytes, 0, sizeof(extra.bytes));
            void* args[2] = {type_obj, extra.bytes};
            char call_err[256];
            void* ret = invoke_raw(mi, nullptr, args, call_err, sizeof(call_err));
            if (call_err[0] != '\0') {
                snprintf(err, err_size, "%s", call_err);
                return nullptr;
            }
            return (Array<Il2CppObject*>*)ret;
        }
    }
    snprintf(err, err_size, "FindObjectsOfType not found");
    return nullptr;
}

static int push_objects_of_class(lua_State* L, Il2CppClass* klass) {
    char err[256];
    auto* arr = find_objects_array(klass, err, sizeof(err));
    if (err[0] != '\0') return luaL_error(L, "%s", err);
    lua_newtable(L);
    if (arr) {
        int n = 0;
        for (int i = 0; i < (int)arr->capacity; i++) {
            if (!arr->m_Items[i]) continue;
            push_object(L, arr->m_Items[i]);
            lua_rawseti(L, -2, ++n);
        }
    }
    return 1;
}

static int l_find_class(lua_State* L) {
    const char* full_name = luaL_checkstring(L, 1);
    const char* asm_name = lua_gettop(L) >= 2 && lua_isstring(L, 2) ? lua_tostring(L, 2) : "";
    Il2CppClass* k = find_class_cached(full_name, asm_name);
    push_ref(L, k, nullptr);
    return 1;
}

static int l_wrap(lua_State* L) {
    uintptr_t addr = (uintptr_t)luaL_checknumber(L, 1);
    if (!address_plausible(addr)) {
        lua_pushnil(L);
        return 1;
    }
    auto* obj = (Il2CppObject*)addr;
    if (!address_plausible((uintptr_t)obj->klass)) {
        lua_pushnil(L);
        return 1;
    }
    push_object(L, obj);
    return 1;
}

static int l_type_name(lua_State* L) {
    auto* r = (lua_ref*)test_udata(L, 1, mt_ref);
    if (r) {
        lua_pushstring(L, lua_class_full_name(r->klass).c_str());
        return 1;
    }
    if (test_udata(L, 1, mt_method)) {
        lua_pushstring(L, "method");
        return 1;
    }
    vec_value v;
    if (lua_istable(L, 1) && read_vec(L, 1, v)) {
        lua_pushstring(L, "vec");
        return 1;
    }
    lua_pushstring(L, luaL_typename(L, 1));
    return 1;
}

static int l_is_valid(lua_State* L) {
    if (lua_isnoneornil(L, 1)) {
        lua_pushboolean(L, 0);
        return 1;
    }
    auto* r = (lua_ref*)test_udata(L, 1, mt_ref);
    lua_pushboolean(L, r ? ref_is_valid(r) : 1);
    return 1;
}

static int l_destroy(lua_State* L) {
    bool is_ref = false;
    uintptr_t addr = ref_address_at(L, 1, is_ref);
    if (!addr) return luaL_error(L, "object expected");
    bool has_delay = lua_gettop(L) >= 2 && lua_isnumber(L, 2);
    float delay = has_delay ? (float)lua_tonumber(L, 2) : 0.0f;
    return destroy_object(L, addr, has_delay, delay);
}

static int l_scene_get_objects(lua_State* L) {
    Il2CppClass* go = find_class_cached("UnityEngine.GameObject", "");
    if (!go) return luaL_error(L, "UnityEngine.GameObject not found");
    return push_objects_of_class(L, go);
}

static int l_scene_find_objects_of_type(lua_State* L) {
    Il2CppClass* k = class_from_arg(L, 1);
    if (!k) return luaL_error(L, "type not found");
    return push_objects_of_class(L, k);
}

static int l_scene_find_object(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    Il2CppClass* go = find_class_cached("UnityEngine.GameObject", "");
    if (!go) return luaL_error(L, "UnityEngine.GameObject not found");
    const auto& cand = find_candidates(go, "Find", 1);
    if (cand.empty()) return luaL_error(L, "GameObject.Find not found");
    void* args[1] = {CreateMonoString(name)};
    char err[384];
    void* ret = invoke_raw(cand.front(), nullptr, args, err, sizeof(err));
    if (err[0] != '\0') return luaL_error(L, "%s", err);
    push_object(L, (Il2CppObject*)ret);
    return 1;
}

static int l_scene_index(lua_State* L) {
    const char* key = luaL_checkstring(L, 2);
    lua_pushstring(L, key);
    lua_replace(L, 1);
    lua_settop(L, 1);
    return l_scene_find_object(L);
}
