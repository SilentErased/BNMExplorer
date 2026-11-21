#pragma once
#include <vector>
#include "lua_values.hpp"

struct lua_ref {
    Il2CppClass* klass;
    uintptr_t inst;
    Il2CppGCHandle handle;
};

struct lua_method {
    MethodInfo* info;
    uintptr_t inst;
    Il2CppGCHandle handle;
};

struct member_info {
    FieldInfo* field;
    MethodInfo* getter;
    MethodInfo* setter;
    bool has_method;
    bool found;
};

static bool address_plausible(uintptr_t p) {
    return p >= 0x10000 && (p & 0x00FFFFFFFFFFFFFFULL) < 0x0001000000000000ULL;
}

static Il2CppGCHandle new_handle(Il2CppObject* obj) {
    auto& m = BNM::Internal::il2cppMethods;
    if (obj && m.il2cpp_gchandle_new) return m.il2cpp_gchandle_new(obj, false);
    return (Il2CppGCHandle)0;
}

static void free_handle(Il2CppGCHandle h) {
    auto& m = BNM::Internal::il2cppMethods;
    if (h && m.il2cpp_gchandle_free) m.il2cpp_gchandle_free(h);
}

static void* test_udata(lua_State* L, int idx, const char* mt) {
    if (!lua_isuserdata(L, idx) || lua_islightuserdata(L, idx)) return nullptr;
    if (!lua_getmetatable(L, idx)) return nullptr;
    luaL_getmetatable(L, mt);
    bool eq = lua_rawequal(L, -1, -2) != 0;
    lua_pop(L, 2);
    return eq ? lua_touserdata(L, idx) : nullptr;
}

static void push_ref(lua_State* L, Il2CppClass* klass, Il2CppObject* obj) {
    auto* r = (lua_ref*)lua_newuserdata(L, sizeof(lua_ref));
    r->klass = obj ? obj->klass : klass;
    r->inst = (uintptr_t)obj;
    r->handle = new_handle(obj);
    luaL_getmetatable(L, mt_ref);
    lua_setmetatable(L, -2);
}

static void push_object(lua_State* L, Il2CppObject* obj) {
    if (!obj) {
        lua_pushnil(L);
        return;
    }
    push_ref(L, nullptr, obj);
}

static void push_method(lua_State* L, MethodInfo* info, Il2CppObject* inst) {
    auto* m = (lua_method*)lua_newuserdata(L, sizeof(lua_method));
    m->info = info;
    m->inst = (uintptr_t)inst;
    m->handle = new_handle(inst);
    luaL_getmetatable(L, mt_method);
    lua_setmetatable(L, -2);
}

static uintptr_t ref_address_at(lua_State* L, int idx, bool& is_ref) {
    is_ref = false;
    if (lua_type(L, idx) == LUA_TNUMBER) return (uintptr_t)lua_tonumber(L, idx);
    auto* r = (lua_ref*)test_udata(L, idx, mt_ref);
    if (r) {
        is_ref = true;
        return r->inst;
    }
    luaL_error(L, "argument %d: object expected", idx);
    return 0;
}

static Il2CppClass* find_class_cached(const std::string& full_name, const std::string& asm_name) {
    static std::unordered_map<std::string, Il2CppClass*> cache;
    std::string key = asm_name + "|" + full_name;
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;
    std::string ns;
    std::string name = full_name;
    auto dot = full_name.rfind('.');
    if (dot != std::string::npos) {
        ns = full_name.substr(0, dot);
        name = full_name.substr(dot + 1);
    }
    Class c = asm_name.empty() ? Class(ns.c_str(), name.c_str()) : Class(ns.c_str(), name.c_str(), Image(asm_name.c_str()));
    Il2CppClass* k = c.IsValid() ? c.GetClass() : nullptr;
    if (k) cache[key] = k;
    return k;
}

static bool is_unity_object_class(Il2CppClass* k) {
    static std::unordered_map<Il2CppClass*, bool> cache;
    auto it = cache.find(k);
    if (it != cache.end()) return it->second;
    bool result = false;
    auto& m = BNM::Internal::il2cppMethods;
    for (Il2CppClass* cur = k; cur; cur = m.il2cpp_class_get_parent ? m.il2cpp_class_get_parent(cur) : nullptr) {
        if (cur->name && cur->namespaze && strcmp(cur->name, "Object") == 0 && strcmp(cur->namespaze, "UnityEngine") == 0) {
            result = true;
            break;
        }
    }
    cache[k] = result;
    return result;
}

static int unity_cached_ptr_offset() {
    static int offset = -2;
    if (offset == -2) {
        offset = (int)sizeof(Il2CppObject);
        Class c("UnityEngine", "Object");
        auto& m = BNM::Internal::il2cppMethods;
        if (c.IsValid() && m.il2cpp_class_get_field_from_name) {
            auto* fi = m.il2cpp_class_get_field_from_name(c.GetClass(), "m_CachedPtr");
            if (fi) offset = (int)fi->offset;
        }
    }
    return offset;
}

static bool unity_object_alive(Il2CppObject* o) {
    if (!o || !o->klass) return false;
    if (!is_unity_object_class(o->klass)) return true;
    int off = unity_cached_ptr_offset();
    if (off < 0) return true;
    return *(void**)((uint8_t*)o + off) != nullptr;
}

static bool ref_is_valid(const lua_ref* r) {
    if (!r->klass) return false;
    if (!r->inst) return true;
    if (!address_plausible(r->inst)) return false;
    return unity_object_alive((Il2CppObject*)r->inst);
}

static const std::vector<MethodInfo*>& find_candidates(Il2CppClass* klass, const char* name, int argc) {
    static std::unordered_map<std::string, std::vector<MethodInfo*>> cache;
    std::string key = std::to_string((uintptr_t)klass) + ":" + name + ":" + std::to_string(argc);
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;
    std::vector<MethodInfo*> out;
    bool ctor = strcmp(name, ".ctor") == 0;
    for (Class cur(klass); cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
        for (auto& m : cur.GetMethods(false)) {
            auto* mi = m.GetInfo();
            if (!mi || !mi->name || strcmp(mi->name, name) != 0) continue;
            if (argc >= 0 && (int)mi->parameters_count != argc) continue;
            out.push_back(mi);
        }
        if (ctor) break;
    }
    return cache.emplace(key, std::move(out)).first->second;
}

static MethodInfo* choose_overload(lua_State* L, const std::vector<MethodInfo*>& candidates, int first, int argc) {
    for (int pass = 0; pass < 2; pass++) {
        for (auto* mi : candidates) {
            bool ok = true;
            for (int i = 0; i < argc && ok; i++)
                ok = lua_matches_kind(L, first + i, kind_from_type(mi->parameters[i]), pass == 0);
            if (ok) return mi;
        }
    }
    return candidates.empty() ? nullptr : candidates.front();
}

static member_info find_member(Il2CppClass* klass, const char* name) {
    static std::unordered_map<std::string, member_info> cache;
    std::string key = std::to_string((uintptr_t)klass) + ":" + name;
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;
    member_info out = {};
    for (Class cur(klass); cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
        for (auto& p : cur.GetProperties(false)) {
            auto* pi = p._data;
            if (!pi || !pi->name || strcmp(pi->name, name) != 0) continue;
            out.getter = (MethodInfo*)pi->get;
            out.setter = (MethodInfo*)pi->set;
            out.found = true;
            break;
        }
        if (!out.found) {
            for (auto& f : cur.GetFields(false)) {
                auto* fi = f.GetInfo();
                if (!fi || !fi->name || strcmp(fi->name, name) != 0) continue;
                out.field = fi;
                out.found = true;
                break;
            }
        }
        if (!out.found) {
            for (auto& m : cur.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (mi && mi->name && strcmp(mi->name, name) == 0) {
                    out.has_method = true;
                    break;
                }
            }
        }
        if (out.found || out.has_method) break;
    }
    cache[key] = out;
    return out;
}

static void format_exception(Il2CppException* exc, char* out, size_t n) {
    snprintf(out, n, "il2cpp exception");
    if (!exc) return;
    try {
        auto* eo = (Il2CppObject*)exc;
        const char* cls_name = (eo->klass && eo->klass->name) ? eo->klass->name : "Exception";
        Class c(eo->klass);
        if (!c.IsValid()) return;
        MethodBase m = c.GetMethod("get_Message", 0);
        if (!m.IsValid()) {
            snprintf(out, n, "%s", cls_name);
            return;
        }
        auto* mi = m.GetInfo();
        if (!mi || !mi->methodPointer) return;
        Il2CppException* inner = nullptr;
        auto* ret = (BNM::Structures::Mono::String*)BNM::Internal::il2cppMethods.il2cpp_runtime_invoke(mi, exc, nullptr, &inner);
        if (ret && !inner) snprintf(out, n, "%s: %s", cls_name, ret->str().c_str());
        else snprintf(out, n, "%s", cls_name);
    } catch (...) {
    }
}

static int invoke_method_info(lua_State* L, MethodInfo* mi, Il2CppObject* inst, int first, int argc) {
    if (!mi || !mi->methodPointer) return luaL_error(L, "method has no compiled pointer");
    bool is_static = (mi->flags & 0x10) != 0;
    if (!is_static && !inst) return luaL_error(L, "method '%s' needs an object instance", mi->name);
    if (argc != (int)mi->parameters_count)
        return luaL_error(L, "method '%s' expects %d arguments, got %d", mi->name, (int)mi->parameters_count, argc);
    if (argc > max_call_args) return luaL_error(L, "too many arguments");
    value_slot slots[max_call_args];
    void* args[max_call_args];
    for (int i = 0; i < argc; i++) {
        value_kind k = kind_from_type(mi->parameters[i]);
        args[i] = lua_to_arg(L, first + i, k, slots[i]);
    }
    Il2CppException* exc = nullptr;
    void* ret = BNM::Internal::il2cppMethods.il2cpp_runtime_invoke(mi, is_static ? nullptr : (void*)inst, argc ? args : nullptr, &exc);
    if (exc) {
        char msg[384];
        format_exception(exc, msg, sizeof(msg));
        return luaL_error(L, "%s", msg);
    }
    value_kind rk = kind_from_type(mi->return_type);
    if (rk == value_kind::nothing) return 0;
    if (rk == value_kind::unsupported || rk == value_kind::unknown) {
        lua_pushnil(L);
        return 1;
    }
    push_return(L, rk, ret);
    return 1;
}

static void* invoke_raw(MethodInfo* mi, void* inst, void** args, char* error_out, size_t error_size) {
    if (!mi || !mi->methodPointer) {
        snprintf(error_out, error_size, "method has no compiled pointer");
        return nullptr;
    }
    Il2CppException* exc = nullptr;
    void* ret = BNM::Internal::il2cppMethods.il2cpp_runtime_invoke(mi, inst, args, &exc);
    if (exc) {
        format_exception(exc, error_out, error_size);
        return nullptr;
    }
    error_out[0] = '\0';
    return ret;
}

static int read_field(lua_State* L, const lua_ref* r, FieldInfo* fi) {
    value_kind k = kind_from_type(fi->type);
    if (k == value_kind::unsupported || k == value_kind::unknown || k == value_kind::nothing)
        return luaL_error(L, "field '%s' has an unsupported type", fi->name);
    bool is_static = (fi->type->attrs & 0x10) != 0;
    auto& m = BNM::Internal::il2cppMethods;
    if (is_static) {
        value_slot slot;
        memset(slot.bytes, 0, sizeof(slot.bytes));
        if (m.il2cpp_runtime_class_init && fi->parent) m.il2cpp_runtime_class_init(fi->parent);
        m.il2cpp_field_static_get_value(fi, slot.bytes);
        push_from_memory(L, k, slot.bytes);
        return 1;
    }
    if (!r->inst) return luaL_error(L, "field '%s' needs an object instance", fi->name);
    push_from_memory(L, k, (const uint8_t*)r->inst + fi->offset);
    return 1;
}

static int write_field(lua_State* L, const lua_ref* r, FieldInfo* fi, int value_idx) {
    value_kind k = kind_from_type(fi->type);
    if (k == value_kind::unsupported || k == value_kind::unknown || k == value_kind::nothing)
        return luaL_error(L, "field '%s' has an unsupported type", fi->name);
    value_slot arg_slot;
    value_slot store_slot;
    void* arg = lua_to_arg(L, value_idx, k, arg_slot);
    void* src = slot_source(k, arg, store_slot);
    bool is_static = (fi->type->attrs & 0x10) != 0;
    auto& m = BNM::Internal::il2cppMethods;
    if (is_static) {
        if (m.il2cpp_runtime_class_init && fi->parent) m.il2cpp_runtime_class_init(fi->parent);
        m.il2cpp_field_static_set_value(fi, src);
        return 0;
    }
    if (!r->inst) return luaL_error(L, "field '%s' needs an object instance", fi->name);
    memcpy((uint8_t*)r->inst + fi->offset, src, kind_size(k));
    return 0;
}

static lua_ref* closure_self(lua_State* L, int& offset) {
    auto* r = (lua_ref*)lua_touserdata(L, lua_upvalueindex(1));
    offset = (lua_gettop(L) >= 1 && lua_rawequal(L, 1, lua_upvalueindex(1))) ? 1 : 0;
    return r;
}

static Il2CppClass* class_from_arg(lua_State* L, int idx) {
    if (lua_type(L, idx) == LUA_TSTRING) return find_class_cached(lua_tostring(L, idx), "");
    auto* r = (lua_ref*)test_udata(L, idx, mt_ref);
    return r ? r->klass : nullptr;
}

static std::string lua_unity_object_name(Il2CppObject* o) {
    Il2CppClass* uo = find_class_cached("UnityEngine.Object", "");
    if (!uo || !o || !unity_object_alive(o) || !is_unity_object_class(o->klass)) return "";
    const auto& cand = find_candidates(uo, "get_name", 0);
    if (cand.empty()) return "";
    char err[128];
    void* ret = invoke_raw(cand.front(), o, nullptr, err, sizeof(err));
    if (!ret) return "";
    return ((BNM::Structures::Mono::String*)ret)->str();
}

static std::string lua_class_full_name(Il2CppClass* k) {
    if (!k || !k->name) return "?";
    std::string n = k->name;
    if (k->namespaze && k->namespaze[0] != '\0') n = std::string(k->namespaze) + "." + n;
    return n;
}

static int l_ref_call_member(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    const char* name = lua_tostring(L, lua_upvalueindex(2));
    int argc = lua_gettop(L) - off;
    if (!r->klass) return luaL_error(L, "invalid class");
    const auto& cand = find_candidates(r->klass, name, argc);
    MethodInfo* mi = choose_overload(L, cand, off + 1, argc);
    if (!mi) return luaL_error(L, "no overload of '%s' takes %d arguments", name, argc);
    return invoke_method_info(L, mi, (Il2CppObject*)r->inst, off + 1, argc);
}

static int l_ref_is_valid(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    lua_pushboolean(L, ref_is_valid(r));
    return 1;
}

static int l_ref_get_method(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    const char* name = luaL_checkstring(L, off + 1);
    int argc = lua_gettop(L) >= off + 2 ? (int)luaL_checknumber(L, off + 2) : -1;
    MethodInfo* mi = nullptr;
    if (r->klass) {
        const auto& cand = find_candidates(r->klass, name, argc);
        if (!cand.empty()) mi = cand.front();
    }
    push_method(L, mi, (Il2CppObject*)r->inst);
    return 1;
}

static int l_ref_get_field(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    const char* name = luaL_checkstring(L, off + 1);
    if (!r->klass) return luaL_error(L, "invalid class");
    member_info m = find_member(r->klass, name);
    if (!m.field) return luaL_error(L, "field '%s' not found", name);
    return read_field(L, r, m.field);
}

static int l_ref_set_field(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    const char* name = luaL_checkstring(L, off + 1);
    if (!r->klass) return luaL_error(L, "invalid class");
    member_info m = find_member(r->klass, name);
    if (!m.field) return luaL_error(L, "field '%s' not found", name);
    return write_field(L, r, m.field, off + 2);
}

static int l_ref_new(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    int argc = lua_gettop(L) - off;
    if (!r->klass) return luaL_error(L, "invalid class");
    auto& m = BNM::Internal::il2cppMethods;
    if (m.il2cpp_runtime_class_init) m.il2cpp_runtime_class_init(r->klass);
    const auto& cand = find_candidates(r->klass, ".ctor", argc);
    MethodInfo* mi = choose_overload(L, cand, off + 1, argc);
    if (!mi) return luaL_error(L, "no constructor takes %d arguments", argc);
    Il2CppObject* obj = m.il2cpp_object_new(r->klass);
    if (!obj) return luaL_error(L, "object allocation failed");
    int base = lua_gettop(L);
    invoke_method_info(L, mi, obj, off + 1, argc);
    lua_settop(L, base);
    push_object(L, obj);
    return 1;
}

static int component_call(lua_State* L, const lua_ref* r, const char* method_name, Il2CppClass* target) {
    if (!r->inst) return luaL_error(L, "object expected");
    if (!target) return luaL_error(L, "component type not found");
    Il2CppObject* type_obj = (Il2CppObject*)Class(target).GetMonoType();
    const auto& cand = find_candidates(r->klass, method_name, 1);
    MethodInfo* chosen = nullptr;
    for (auto* mi : cand) {
        if (type_full_name(mi->parameters[0]) == "System.Type") {
            chosen = mi;
            break;
        }
    }
    if (!chosen) return luaL_error(L, "%s(System.Type) not found", method_name);
    void* args[1] = {type_obj};
    char err[384];
    void* ret = invoke_raw(chosen, (void*)r->inst, args, err, sizeof(err));
    if (err[0] != '\0') return luaL_error(L, "%s", err);
    push_object(L, (Il2CppObject*)ret);
    return 1;
}

static int l_ref_get_component(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    return component_call(L, r, "GetComponent", class_from_arg(L, off + 1));
}

static int l_ref_add_component(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    return component_call(L, r, "AddComponent", class_from_arg(L, off + 1));
}

static int l_ref_get_component_in_children(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    return component_call(L, r, "GetComponentInChildren", class_from_arg(L, off + 1));
}

static int destroy_object(lua_State* L, uintptr_t addr, bool has_delay, float delay) {
    Il2CppClass* uo = find_class_cached("UnityEngine.Object", "");
    if (!uo) return luaL_error(L, "UnityEngine.Object not found");
    int argc = has_delay ? 2 : 1;
    const auto& cand = find_candidates(uo, "Destroy", argc);
    MethodInfo* mi = cand.empty() ? nullptr : cand.front();
    if (!mi) return luaL_error(L, "Object.Destroy not found");
    void* args[2] = {(void*)addr, &delay};
    char err[384];
    invoke_raw(mi, nullptr, args, err, sizeof(err));
    if (err[0] != '\0') return luaL_error(L, "%s", err);
    return 0;
}

static int l_ref_destroy(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    if (!r->inst) return luaL_error(L, "object expected");
    bool has_delay = lua_gettop(L) >= off + 1 && lua_isnumber(L, off + 1);
    float delay = has_delay ? (float)lua_tonumber(L, off + 1) : 0.0f;
    return destroy_object(L, r->inst, has_delay, delay);
}

static int l_ref_debug_info(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    char buf[320];
    void* cached = nullptr;
    bool plausible = address_plausible(r->inst);
    if (r->inst && plausible && is_unity_object_class(r->klass))
        cached = *(void**)((uint8_t*)r->inst + unity_cached_ptr_offset());
    snprintf(buf, sizeof(buf), "addr=0x%llX plausible=%d klass=%s unity_object=%d cached_ptr=%p offset=%d valid=%d",
             (unsigned long long)r->inst, plausible ? 1 : 0, lua_class_full_name(r->klass).c_str(),
             r->klass && is_unity_object_class(r->klass) ? 1 : 0, cached, unity_cached_ptr_offset(), ref_is_valid(r) ? 1 : 0);
    lua_pushstring(L, buf);
    return 1;
}

static int l_ref_get_class(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    push_ref(L, r->klass, nullptr);
    return 1;
}


static int push_objects_of_class(lua_State* L, Il2CppClass* klass);


static int l_get_builtin_resource(lua_State* L) {
    Il2CppClass* target = class_from_arg(L, 1);
    const char* path = luaL_checkstring(L, 2);
    if (!target) return luaL_error(L, "resource type class expected");

    auto mono_type = Class(target).GetMonoType();
    if (!mono_type) return luaL_error(L, "resource MonoType unavailable");

    using mono_type_t = decltype(mono_type);
    using mono_string_t = decltype(CreateMonoString(path));
    using builtin_resource_fn = Il2CppObject* (*)(mono_type_t, mono_string_t);
    static builtin_resource_fn get_builtin_resource =
            (builtin_resource_fn)BNM::GetExternMethod("UnityEngine.Resources::GetBuiltinResource");

    if (!get_builtin_resource)
        return luaL_error(L, "UnityEngine.Resources::GetBuiltinResource not found");

    Il2CppObject* resource = get_builtin_resource(mono_type, CreateMonoString(path));
    if (!resource) {
        lua_pushnil(L);
        return 1;
    }

    push_object(L, resource);
    return 1;
}

static int l_ref_find_objects(lua_State* L) {
    int off = 0;
    lua_ref* r = closure_self(L, off);
    if (!r->klass) return luaL_error(L, "invalid class");
    return push_objects_of_class(L, r->klass);
}

static void push_ref_closure(lua_State* L, int self_idx, lua_CFunction fn) {
    lua_pushvalue(L, self_idx);
    lua_pushcclosure(L, fn, 1);
}

static int l_ref_index(lua_State* L) {
    auto* r = (lua_ref*)luaL_checkudata(L, 1, mt_ref);
    const char* key = luaL_checkstring(L, 2);
    if (strcmp(key, "addr") == 0) {
        lua_pushnumber(L, (double)r->inst);
        return 1;
    }
    if (strcmp(key, "class_name") == 0) {
        lua_pushstring(L, r->klass && r->klass->name ? r->klass->name : "");
        return 1;
    }
    if (strcmp(key, "full_name") == 0) {
        lua_pushstring(L, lua_class_full_name(r->klass).c_str());
        return 1;
    }
    struct sugar_entry {
        const char* name;
        lua_CFunction fn;
    };
    static const sugar_entry sugar[] = {
            {"is_valid", l_ref_is_valid},
            {"get_method", l_ref_get_method},
            {"get_field", l_ref_get_field},
            {"set_field", l_ref_set_field},
            {"new", l_ref_new},
            {"get_component", l_ref_get_component},
            {"add_component", l_ref_add_component},
            {"get_component_in_children", l_ref_get_component_in_children},
            {"destroy", l_ref_destroy},
            {"get_class", l_ref_get_class},
            {"debug_info", l_ref_debug_info},
            {"find_objects", l_ref_find_objects}
    };
    for (const auto& s : sugar) {
        if (strcmp(key, s.name) == 0) {
            push_ref_closure(L, 1, s.fn);
            return 1;
        }
    }
    if (!r->klass) return luaL_error(L, "invalid class");
    member_info m = find_member(r->klass, key);
    if (!m.found && !m.has_method) return luaL_error(L, "member '%s' not found", key);
    if (m.getter) return invoke_method_info(L, m.getter, (Il2CppObject*)r->inst, 2, 0);
    if (m.field) return read_field(L, r, m.field);
    if (m.found) return luaL_error(L, "property '%s' has no getter", key);
    lua_pushvalue(L, 1);
    lua_pushstring(L, key);
    lua_pushcclosure(L, l_ref_call_member, 2);
    return 1;
}

static int l_ref_newindex(lua_State* L) {
    auto* r = (lua_ref*)luaL_checkudata(L, 1, mt_ref);
    const char* key = luaL_checkstring(L, 2);
    if (!r->klass) return luaL_error(L, "invalid class");
    member_info m = find_member(r->klass, key);
    if (!m.found) return luaL_error(L, "member '%s' not found", key);
    if (m.setter) {
        invoke_method_info(L, m.setter, (Il2CppObject*)r->inst, 3, 1);
        return 0;
    }
    if (m.field) return write_field(L, r, m.field, 3);
    return luaL_error(L, "property '%s' has no setter", key);
}

static int l_ref_tostring(lua_State* L) {
    auto* r = (lua_ref*)luaL_checkudata(L, 1, mt_ref);
    std::string name = r->inst && address_plausible(r->inst) ? lua_unity_object_name((Il2CppObject*)r->inst) : "";
    char buf[320];
    if (!name.empty())
        snprintf(buf, sizeof(buf), "%s<%s @ 0x%llX>", lua_class_full_name(r->klass).c_str(), name.c_str(), (unsigned long long)r->inst);
    else
        snprintf(buf, sizeof(buf), "%s<@ 0x%llX>", lua_class_full_name(r->klass).c_str(), (unsigned long long)r->inst);
    lua_pushstring(L, buf);
    return 1;
}

static int l_ref_gc(lua_State* L) {
    auto* r = (lua_ref*)lua_touserdata(L, 1);
    if (r) {
        free_handle(r->handle);
        r->handle = (Il2CppGCHandle)0;
    }
    return 0;
}

static int l_ref_eq(lua_State* L) {
    auto* a = (lua_ref*)test_udata(L, 1, mt_ref);
    auto* b = (lua_ref*)test_udata(L, 2, mt_ref);
    lua_pushboolean(L, a && b && a->inst == b->inst && (a->inst != 0 || a->klass == b->klass));
    return 1;
}

static int l_method_call_common(lua_State* L, const lua_method* m, int first, int argc) {
    if (!m->info) return luaL_error(L, "invalid method");
    return invoke_method_info(L, m->info, (Il2CppObject*)m->inst, first, argc);
}

static int l_method_call(lua_State* L) {
    auto* m = (lua_method*)luaL_checkudata(L, 1, mt_method);
    return l_method_call_common(L, m, 2, lua_gettop(L) - 1);
}

static lua_method* method_closure_self(lua_State* L, int& offset) {
    auto* m = (lua_method*)lua_touserdata(L, lua_upvalueindex(1));
    offset = (lua_gettop(L) >= 1 && lua_rawequal(L, 1, lua_upvalueindex(1))) ? 1 : 0;
    return m;
}

static int l_method_invoke(lua_State* L) {
    int off = 0;
    lua_method* m = method_closure_self(L, off);
    return l_method_call_common(L, m, off + 1, lua_gettop(L) - off);
}

static int l_method_is_valid(lua_State* L) {
    int off = 0;
    lua_method* m = method_closure_self(L, off);
    lua_pushboolean(L, m->info && m->info->methodPointer);
    return 1;
}

static int l_method_get_name(lua_State* L) {
    int off = 0;
    lua_method* m = method_closure_self(L, off);
    if (!m->info || !m->info->name) return luaL_error(L, "invalid method");
    lua_pushstring(L, m->info->name);
    return 1;
}

static int l_method_get_param_count(lua_State* L) {
    int off = 0;
    lua_method* m = method_closure_self(L, off);
    if (!m->info) return luaL_error(L, "invalid method");
    lua_pushnumber(L, m->info->parameters_count);
    return 1;
}

static int l_method_get_return_type(lua_State* L) {
    int off = 0;
    lua_method* m = method_closure_self(L, off);
    if (!m->info) return luaL_error(L, "invalid method");
    lua_pushstring(L, type_full_name(m->info->return_type).c_str());
    return 1;
}

static int l_method_set_instance(lua_State* L) {
    int off = 0;
    lua_method* m = method_closure_self(L, off);
    bool is_ref = false;
    uintptr_t addr = lua_isnil(L, off + 1) ? 0 : ref_address_at(L, off + 1, is_ref);
    free_handle(m->handle);
    m->inst = addr;
    m->handle = new_handle((Il2CppObject*)addr);
    return 0;
}

static int l_method_index(lua_State* L) {
    luaL_checkudata(L, 1, mt_method);
    const char* key = luaL_checkstring(L, 2);
    struct sugar_entry {
        const char* name;
        lua_CFunction fn;
    };
    static const sugar_entry sugar[] = {
            {"invoke", l_method_invoke},
            {"is_valid", l_method_is_valid},
            {"get_name", l_method_get_name},
            {"get_param_count", l_method_get_param_count},
            {"get_return_type", l_method_get_return_type},
            {"set_instance", l_method_set_instance}
    };
    for (const auto& s : sugar) {
        if (strcmp(key, s.name) == 0) {
            push_ref_closure(L, 1, s.fn);
            return 1;
        }
    }
    return luaL_error(L, "method has no member '%s'", key);
}

static int l_method_gc(lua_State* L) {
    auto* m = (lua_method*)lua_touserdata(L, 1);
    if (m) {
        free_handle(m->handle);
        m->handle = (Il2CppGCHandle)0;
    }
    return 0;
}

static int l_method_tostring(lua_State* L) {
    auto* m = (lua_method*)luaL_checkudata(L, 1, mt_method);
    char buf[256];
    snprintf(buf, sizeof(buf), "method<%s>", m->info && m->info->name ? m->info->name : "invalid");
    lua_pushstring(L, buf);
    return 1;
}
