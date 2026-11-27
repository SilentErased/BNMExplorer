#pragma once
#include "lua_values.hpp"
#include "lua_objects.hpp"
#include "lua_scene.hpp"
#include "lua_xr.hpp"
#include "lua_runtime.hpp"
#include "reflect.hpp"
#include "lua_hooks.hpp"
#include "lua_reflect.hpp"
#include "lua_modules.hpp"
#include "lua_stdlib.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

static const char* lua_prelude_source = R"lua(
game_object = find_class("UnityEngine.GameObject")
unity_object = find_class("UnityEngine.Object")
time = find_class("UnityEngine.Time")
shader = find_class("UnityEngine.Shader")
quaternion = find_class("UnityEngine.Quaternion")
mathf = find_class("UnityEngine.Mathf")
physics = find_class("UnityEngine.Physics")
force_mode = { force = 0, impulse = 1, velocity_change = 2, acceleration = 5 }
primitive_type = { sphere = 0, capsule = 1, cylinder = 2, cube = 3, plane = 4, quad = 5 }

local function make_namespace(asm_name, ns_name)
  local ns = {}
  function ns:class(name, addr)
    if addr and addr ~= 0 then
      return wrap(addr)
    end
    local full = ns_name ~= "" and (ns_name .. "." .. name) or name
    return find_class(full, asm_name)
  end
  return ns
end

local shader_candidates = { "Standard", "GorillaTag/UberShader", "Universal Render Pipeline/Lit", "Unlit/Color" }

function find_shader(name)
  if name then return shader.Find(name) end
  for _, candidate in ipairs(shader_candidates) do
    local found = shader.Find(candidate)
    if found then return found end
  end
  return nil
end

function create_primitive(kind, col, shader_name)
  local obj = game_object.CreatePrimitive(kind)
  local rend = obj:get_component("UnityEngine.Renderer")
  if rend then
    local mat = rend.material
    local found = find_shader(shader_name)
    if found then mat.shader = found end
    if col then mat.color = col end
    rend.enabled = true
  end
  return obj
end

function assembly(asm_name)
  local a = {}
  function a:namespace(ns_name)
    return make_namespace(asm_name, ns_name)
  end
  function a:class(name, addr)
    return make_namespace(asm_name, ""):class(name, addr)
  end
  return a
end
)lua";

static void register_function(lua_State* L, const char* name, lua_CFunction fn) {
    lua_pushcfunction(L, fn);
    lua_setglobal(L, name);
}

static void register_metatable(lua_State* L, const char* name, const luaL_Reg* entries) {
    luaL_newmetatable(L, name);
    for (const luaL_Reg* e = entries; e->name; e++) {
        lua_pushcfunction(L, e->func);
        lua_setfield(L, -2, e->name);
    }
    lua_pop(L, 1);
}

static void remove_unsafe_globals(lua_State* L) {
    static const char* names[] = {
            "dofile", "loadfile", "module", "require", "package", "io", "os", "debug", "newproxy", "gcinfo"
    };
    for (const char* n : names) {
        lua_pushnil(L);
        lua_setglobal(L, n);
    }
}

static void luabnm_openlibs(lua_State* L) {
    luaL_openlibs(L);
    remove_unsafe_globals(L);

    static const luaL_Reg ref_entries[] = {
            {"__index", l_ref_index},
            {"__newindex", l_ref_newindex},
            {"__tostring", l_ref_tostring},
            {"__gc", l_ref_gc},
            {"__eq", l_ref_eq},
            {nullptr, nullptr}
    };
    register_metatable(L, mt_ref, ref_entries);

    static const luaL_Reg method_entries[] = {
            {"__index", l_method_index},
            {"__call", l_method_call},
            {"__tostring", l_method_tostring},
            {"__gc", l_method_gc},
            {nullptr, nullptr}
    };
    register_metatable(L, mt_method, method_entries);

    static const luaL_Reg vec_entries[] = {
            {"__add", l_vec_add},
            {"__sub", l_vec_sub},
            {"__mul", l_vec_mul},
            {"__div", l_vec_div},
            {"__unm", l_vec_unm},
            {"__eq", l_vec_eq},
            {"__tostring", l_vec_tostring},
            {nullptr, nullptr}
    };
    register_metatable(L, mt_vec, vec_entries);

    luaL_getmetatable(L, mt_vec);
    lua_newtable(L);
    lua_pushcfunction(L, l_vec_length);
    lua_setfield(L, -2, "length");
    lua_pushcfunction(L, l_vec_normalized);
    lua_setfield(L, -2, "normalized");
    lua_pushcfunction(L, l_vec_dot);
    lua_setfield(L, -2, "dot");
    lua_pushcfunction(L, l_vec_distance);
    lua_setfield(L, -2, "distance");
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    module_cache_reset(L);
    register_function(L, "require", l_require);
    register_function(L, "print", l_print);
    register_function(L, "vec2", l_vec2);
    register_function(L, "vec3", l_vec3);
    register_function(L, "vec4", l_vec4);
    register_function(L, "quat", l_quat);
    register_function(L, "color", l_color);
    register_function(L, "find_class", l_find_class);
    register_function(L, "wrap", l_wrap);
    register_function(L, "type_name", l_type_name);
    register_function(L, "is_valid", l_is_valid);
    register_function(L, "destroy", l_destroy);
    register_function(L, "find_object", l_scene_find_object);
    register_function(L, "find_objects_of_type", l_scene_find_objects_of_type);
    register_function(L, "get_builtin_resource", l_get_builtin_resource);
    register_function(L, "every", l_every);
    register_function(L, "after", l_after);
    register_function(L, "on_update", l_on_update);
    register_function(L, "cancel", l_cancel);
    register_function(L, "cancel_all", l_cancel_all);
    register_function(L, "get_time", l_get_time);
    register_function(L, "set_tick_hook", l_set_tick_hook);
    register_function(L, "has_tick_hook", l_has_tick_hook);
    register_function(L, "main_thread", l_main_thread);
    register_function(L, "on_game_start", l_on_game_start);
    register_lua_hooks(L);
    register_lua_reflect(L);
    l_register_stdlib(L);

    lua_newtable(L);
    lua_pushcfunction(L, l_scene_get_objects);
    lua_setfield(L, -2, "get_objects");
    lua_pushcfunction(L, l_scene_find_object);
    lua_setfield(L, -2, "find_object");
    lua_pushcfunction(L, l_scene_find_objects_of_type);
    lua_setfield(L, -2, "find_objects_of_type");
    lua_newtable(L);
    lua_pushcfunction(L, l_scene_index);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    lua_setglobal(L, "scene");

    register_xr(L);
}

static lua_State* rt_ensure_state() {
    if (g_rt.state) return g_rt.state;
    lua_State* L = luaL_newstate();
    if (!L) return nullptr;
    luabnm_openlibs(L);
    lua_sethook(L, rt_deadline_hook, LUA_MASKCOUNT, 10000);
    g_rt.state = L;
    g_rt.deadline = rt_now() + exec_timeout_seconds;
    int err = luaL_loadbuffer(L, lua_prelude_source, strlen(lua_prelude_source), "=prelude");
    if (!err) err = lua_pcall(L, 0, 0, 0);
    g_rt.deadline = 0.0;
    if (err) {
        const char* msg = lua_tostring(L, -1);
        rt_append_output(std::string("PreludeError: ") + (msg ? msg : "unknown"));
        lua_pop(L, 1);
    }
    return L;
}

static std::string execute_lua_chunk(const std::string& code, const std::string& chunk_name, bool restart) {
    try {
        std::lock_guard<std::recursive_mutex> lk(g_rt.mtx);
        BNM::AttachIl2Cpp();
        lua_State* L = rt_ensure_state();
        if (!L) return "Error: failed to create Lua state";
        if (restart) {
            for (auto& t : g_rt.tasks) t.dead = true;
            lua_hooks_deactivate_all();
            module_cache_reset(L);
        }
        {
            std::lock_guard<std::mutex> out_lk(g_rt.out_mtx);
            g_rt.exec_output.clear();
            g_rt.exec_depth++;
        }
        int base = lua_gettop(L);
        std::string error_text;
        g_rt.deadline = rt_now() + exec_timeout_seconds;
        int err = luaL_loadbuffer(L, code.c_str(), code.size(), chunk_name.c_str());
        if (err) {
            const char* msg = lua_tostring(L, -1);
            error_text = std::string("SyntaxError: ") + (msg ? msg : "unknown");
        } else {
            err = lua_pcall(L, 0, LUA_MULTRET, 0);
            if (err) {
                const char* msg = lua_tostring(L, -1);
                error_text = std::string("RuntimeError: ") + (msg ? msg : "unknown");
            } else {
                int top = lua_gettop(L);
                for (int i = base + 1; i <= top; i++) rt_append_output(describe_value(L, i));
            }
        }
        g_rt.deadline = 0.0;
        lua_settop(L, base);
        std::string out;
        {
            std::lock_guard<std::mutex> out_lk(g_rt.out_mtx);
            g_rt.exec_depth--;
            out = g_rt.exec_output;
        }
        if (!error_text.empty()) out += (out.empty() ? "" : "\n") + error_text;
        return out;
    } catch (const std::exception& e) {
        return std::string("C++ Exception: ") + e.what();
    } catch (...) {
        return "C++ Exception: unknown";
    }
}

static std::string execute_lua_bnm(const std::string& code) {
    return execute_lua_chunk(code, "=lua", false);
}

static std::string execute_lua_workspace(const std::string& rel_path) {
    std::string clean = ws_sanitize_path(rel_path.empty() ? "main.lua" : rel_path, true);
    if (clean.empty()) return "Error: invalid file path";
    std::string source;
    if (!ws_read_file(clean, source)) return "Error: file not found: " + clean;
    return execute_lua_chunk(source, "=" + clean, true);
}

static void stop_lua_bnm() {
    std::lock_guard<std::recursive_mutex> lk(g_rt.mtx);
    for (auto& t : g_rt.tasks) t.dead = true;
    lua_hooks_deactivate_all();
}

static void reset_lua_bnm() {
    std::lock_guard<std::recursive_mutex> lk(g_rt.mtx);
    BNM::AttachIl2Cpp();
    g_rt.tasks.clear();
    lua_hooks_deactivate_all();
    if (g_rt.state) {
        lua_close(g_rt.state);
        g_rt.state = nullptr;
    }
}

static std::string poll_lua_bnm_output() {
    return rt_take_async_output();
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
