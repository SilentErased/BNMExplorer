#pragma once
#include "reflect.hpp"
#include "lua_objects.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

    static void lua_reflect_set_string(lua_State* L, const char* key, const std::string& value) {
        lua_pushstring(L, value.c_str());
        lua_setfield(L, -2, key);
    }

    static void lua_reflect_push_hit(lua_State* L, const reflect_hit& hit) {
        lua_newtable(L);
        lua_reflect_set_string(L, "kind", hit.kind);
        lua_reflect_set_string(L, "name", hit.name);
        lua_reflect_set_string(L, "type", hit.type);
        lua_reflect_set_string(L, "params", hit.params);
        lua_reflect_set_string(L, "class", hit.decl.empty() ? hit.cls : hit.decl);
        lua_reflect_set_string(L, "namespace", hit.ns);
        lua_reflect_set_string(L, "assembly", hit.asm_name);
        lua_pushboolean(L, hit.is_static);
        lua_setfield(L, -2, "static");
        lua_pushnumber(L, (double)hit.address);
        lua_setfield(L, -2, "address");
    }

    static int lua_reflect_push_hits(lua_State* L, const reflect_result& result) {
        lua_newtable(L);
        int row = 0;
        for (const auto& hit : result.hits) {
            if (hit.kind == "class") push_ref(L, hit.klass, nullptr);
            else lua_reflect_push_hit(L, hit);
            lua_rawseti(L, -2, ++row);
        }
        lua_pushboolean(L, result.truncated);
        return 2;
    }

    static int lua_reflect_find(lua_State* L, int kinds) {
        reflect_query query;
        query.text = luaL_optstring(L, 1, "");
        query.path = luaL_optstring(L, 2, "");
        query.kinds = kinds;
        query.limit = (size_t)std::max<lua_Integer>(1, std::min<lua_Integer>(5000, luaL_optinteger(L, 3, 200)));
        return lua_reflect_push_hits(L, reflect_search(query));
    }

    static int l_find_classes(lua_State* L) { return lua_reflect_find(L, reflect_class); }
    static int l_find_methods(lua_State* L) { return lua_reflect_find(L, reflect_method); }
    static int l_find_fields(lua_State* L) { return lua_reflect_find(L, reflect_field); }
    static int l_find_properties(lua_State* L) { return lua_reflect_find(L, reflect_property); }
    static int l_find_members(lua_State* L) { return lua_reflect_find(L, reflect_method | reflect_field | reflect_property); }

    static int l_list_assemblies(lua_State* L) {
        BNM::AttachIl2Cpp();
        lua_newtable(L);
        int row = 0;
        for (auto& img : BNM::Image::GetImages()) {
            auto* info = img.IsValid() ? img.GetInfo() : nullptr;
            if (!info || !symbol_plausible(info->name)) continue;
            lua_pushstring(L, info->name);
            lua_rawseti(L, -2, ++row);
        }
        return 1;
    }

    static int l_list_classes(lua_State* L) {
        reflect_query query;
        query.path = luaL_optstring(L, 1, "");
        query.kinds = reflect_class;
        query.limit = (size_t)std::max<lua_Integer>(1, std::min<lua_Integer>(20000, luaL_optinteger(L, 2, 2000)));
        return lua_reflect_push_hits(L, reflect_search(query));
    }

    static void lua_reflect_collect(Il2CppClass* k, int what, const std::string& filter, bool inherited, std::vector<reflect_hit>& out) {
        for (Il2CppClass* cur = k; cur && symbol_plausible(cur) && symbol_plausible(cur->name); cur = inherited ? cur->parent : nullptr) {
            Class cls(cur);
            std::string decl = reflect_full_name(cur);
            if (what & reflect_field) {
                for (auto& f : cls.GetFields(false)) {
                    FieldInfo* fi = f.GetInfo();
                    if (!fi || !symbol_plausible(fi) || !symbol_plausible(fi->name) || !reflect_contains(fi->name, filter)) continue;
                    reflect_hit hit = reflect_field_hit(cur, fi);
                    hit.decl = decl;
                    out.push_back(std::move(hit));
                }
            }
            if (what & reflect_property) {
                for (auto& p : cls.GetProperties(false)) {
                    const PropertyInfo* pi = p._data;
                    if (!pi || !symbol_plausible(pi) || !symbol_plausible(pi->name) || !reflect_contains(pi->name, filter)) continue;
                    reflect_hit hit = reflect_property_hit(cur, pi);
                    hit.decl = decl;
                    out.push_back(std::move(hit));
                }
            }
            if (what & reflect_method) {
                for (auto& m : cls.GetMethods(false)) {
                    MethodInfo* mi = m.GetInfo();
                    if (!mi || !symbol_plausible(mi) || !symbol_plausible(mi->name) || !reflect_contains(mi->name, filter)) continue;
                    reflect_hit hit = reflect_method_hit(cur, mi);
                    hit.decl = decl;
                    out.push_back(std::move(hit));
                }
            }
        }
    }

    static int lua_reflect_list(lua_State* L, int what) {
        int off = 0;
        lua_ref* r = closure_self(L, off);
        if (!r->klass) return luaL_error(L, "invalid class");
        std::string filter;
        int flag_index = off + 1;
        if (lua_type(L, off + 1) == LUA_TSTRING) {
            filter = reflect_lower(lua_tostring(L, off + 1));
            flag_index = off + 2;
        }
        std::vector<reflect_hit> hits;
        lua_reflect_collect(r->klass, what, filter, lua_toboolean(L, flag_index) != 0, hits);
        lua_newtable(L);
        int row = 0;
        for (const auto& hit : hits) {
            lua_reflect_push_hit(L, hit);
            lua_rawseti(L, -2, ++row);
        }
        return 1;
    }

    static int l_ref_list_methods(lua_State* L) { return lua_reflect_list(L, reflect_method); }
    static int l_ref_list_fields(lua_State* L) { return lua_reflect_list(L, reflect_field); }
    static int l_ref_list_properties(lua_State* L) { return lua_reflect_list(L, reflect_property); }
    static int l_ref_list_members(lua_State* L) { return lua_reflect_list(L, reflect_method | reflect_field | reflect_property); }

    static int l_ref_list_parents(lua_State* L) {
        int off = 0;
        lua_ref* r = closure_self(L, off);
        lua_newtable(L);
        int row = 0;
        for (Il2CppClass* k = r->klass ? r->klass->parent : nullptr; k && symbol_plausible(k) && row < 64; k = k->parent) {
            lua_pushstring(L, reflect_full_name(k).c_str());
            lua_rawseti(L, -2, ++row);
        }
        return 1;
    }

    static int l_ref_list_interfaces(lua_State* L) {
        int off = 0;
        lua_ref* r = closure_self(L, off);
        lua_newtable(L);

        Il2CppClass* klass = r->klass;
        if (!klass || !klass->interfaces_count) return 1;

        int row = 0;
#if UNITY_VER >= 606
        if (!klass->interfaces) return 1;
    for (uint16_t i = 0; i < klass->interfaces_count; ++i) {
        Il2CppClass* interface_class = klass->interfaces[i].interfaceType;
        if (!symbol_plausible(interface_class)) continue;
        lua_pushstring(L, reflect_full_name(interface_class).c_str());
        lua_rawseti(L, -2, ++row);
    }
#else
        if (!klass->implementedInterfaces) return 1;
        for (uint16_t i = 0; i < klass->interfaces_count; ++i) {
            Il2CppClass* interface_class = klass->implementedInterfaces[i];
            if (!symbol_plausible(interface_class)) continue;
            lua_pushstring(L, reflect_full_name(interface_class).c_str());
            lua_rawseti(L, -2, ++row);
        }
#endif
        return 1;
    }

    static int l_ref_list_nested(lua_State* L) {
        int off = 0;
        lua_ref* r = closure_self(L, off);
        lua_newtable(L);

        Il2CppClass* klass = r->klass;
        if (!klass) return 1;

        int row = 0;
        for (const Class& nested_class : Class(klass).GetInnerClasses(false)) {
            Il2CppClass* nested = nested_class.GetClass();
            if (!symbol_plausible(nested)) continue;
            push_ref(L, nested, nullptr);
            lua_rawseti(L, -2, ++row);
        }
        return 1;
    }

    static int l_ref_class_info(lua_State* L) {
        int off = 0;
        lua_ref* r = closure_self(L, off);
        Il2CppClass* k = r->klass;
        if (!k || !symbol_plausible(k)) return luaL_error(L, "invalid class");
        lua_newtable(L);
        lua_reflect_set_string(L, "name", k->name);
        lua_reflect_set_string(L, "namespace", reflect_ns(k));
        lua_reflect_set_string(L, "full_name", reflect_full_name(k));
        lua_reflect_set_string(L, "assembly", reflect_class_asm(k));
        lua_reflect_set_string(L, "kind", reflect_class_kind(k));
        lua_reflect_set_string(L, "parent", k->parent ? reflect_full_name(k->parent) : "");
        lua_pushboolean(L, (k->flags & 0x80) != 0);
        lua_setfield(L, -2, "abstract");
        lua_pushinteger(L, k->method_count);
        lua_setfield(L, -2, "method_count");
        lua_pushinteger(L, k->field_count);
        lua_setfield(L, -2, "field_count");
        lua_pushinteger(L, k->property_count);
        lua_setfield(L, -2, "property_count");
        lua_pushinteger(L, k->interfaces_count);
        lua_setfield(L, -2, "interface_count");
        return 1;
    }

    static std::string lua_reflect_dump_class(Il2CppClass* k) {
        std::vector<reflect_hit> hits;
        lua_reflect_collect(k, reflect_all & ~reflect_class, "", false, hits);
        std::string out = "// " + std::string(reflect_class_asm(k)) + "\n";
        out += std::string(reflect_class_kind(k)) + " " + reflect_full_name(k);
        if (k->parent && std::string(reflect_class_kind(k)) == "class") out += " : " + reflect_full_name(k->parent);
        out += "\n{\n";
        const char* sections[] = {"field", "property", "method"};
        for (const char* section : sections) {
            bool first = true;
            for (const auto& hit : hits) {
                if (hit.kind != section) continue;
                if (first) {
                    out += std::string("    // ") + section + "s\n";
                    first = false;
                }
                out += std::string("    ") + (hit.is_static ? "static " : "") + hit.type + " " + hit.name;
                if (hit.kind == "method") {
                    char address[32];
                    snprintf(address, sizeof(address), "%llX", (unsigned long long)hit.address);
                    out += "(" + hit.params + ");  // 0x" + address;
                }
                else if (hit.kind == "property") out += " { " + hit.params + " }";
                else out += ";";
                out += "\n";
            }
            if (!first) out += "\n";
        }
        return out + "}";
    }

    static int l_ref_dump(lua_State* L) {
        int off = 0;
        lua_ref* r = closure_self(L, off);
        if (!r->klass) return luaL_error(L, "invalid class");
        lua_pushstring(L, lua_reflect_dump_class(r->klass).c_str());
        return 1;
    }

    static int l_dump_class(lua_State* L) {
        Il2CppClass* k = class_from_arg(L, 1);
        if (!k) return luaL_error(L, "class not found");
        lua_pushstring(L, lua_reflect_dump_class(k).c_str());
        return 1;
    }

    static lua_CFunction lua_reflect_sugar(const char* key) {
        struct entry {
            const char* name;
            lua_CFunction fn;
        };
        static const entry table[] = {
                {"list_methods", l_ref_list_methods},
                {"list_fields", l_ref_list_fields},
                {"list_properties", l_ref_list_properties},
                {"list_members", l_ref_list_members},
                {"list_parents", l_ref_list_parents},
                {"list_interfaces", l_ref_list_interfaces},
                {"list_nested", l_ref_list_nested},
                {"class_info", l_ref_class_info},
                {"dump", l_ref_dump}
        };
        for (const auto& e : table)
            if (strcmp(key, e.name) == 0) return e.fn;
        return nullptr;
    }

    static void register_lua_reflect(lua_State* L) {
        static const luaL_Reg functions[] = {
                {"find_classes", l_find_classes},
                {"find_methods", l_find_methods},
                {"find_fields", l_find_fields},
                {"find_properties", l_find_properties},
                {"find_members", l_find_members},
                {"list_assemblies", l_list_assemblies},
                {"list_classes", l_list_classes},
                {"dump_class", l_dump_class},
                {nullptr, nullptr}
        };
        for (const luaL_Reg* f = functions; f->name; ++f) {
            lua_pushcfunction(L, f->func);
            lua_setglobal(L, f->name);
        }
    }

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
