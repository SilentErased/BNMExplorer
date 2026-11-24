#pragma once
#include <string>
#include <algorithm>
#include "workspace.hpp"
#include "lua_values.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

static const char* module_cache_key = "luabnm.loaded";
static int module_loading_tag = 0;

static std::string module_key_from_name(const char* name) {
    std::string n = name;
    std::replace(n.begin(), n.end(), '\\', '/');
    bool has_suffix = n.size() > 4 && n.compare(n.size() - 4, 4, ".lua") == 0;
    if (has_suffix) n.erase(n.size() - 4);
    else std::replace(n.begin(), n.end(), '.', '/');
    return n;
}

static bool module_locate(const std::string& key, std::string& path, std::string& source) {
    std::string candidates[2] = {key + ".lua", key + "/init.lua"};
    for (const auto& c : candidates) {
        std::string clean = ws_sanitize_path(c, true);
        if (!clean.empty() && ws_read_file(clean, source)) {
            path = clean;
            return true;
        }
    }
    return false;
}

static void module_cache_reset(lua_State* L) {
    lua_newtable(L);
    lua_setfield(L, LUA_REGISTRYINDEX, module_cache_key);
}

static bool require_module(lua_State* L, const char* name, char* error, size_t error_size) {
    error[0] = '\0';
    std::string key = module_key_from_name(name);
    lua_getfield(L, LUA_REGISTRYINDEX, module_cache_key);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        module_cache_reset(L);
        lua_getfield(L, LUA_REGISTRYINDEX, module_cache_key);
    }
    int cache = lua_gettop(L);
    lua_getfield(L, cache, key.c_str());
    if (lua_islightuserdata(L, -1) && lua_touserdata(L, -1) == &module_loading_tag) {
        snprintf(error, error_size, "circular require of '%s'", name);
        lua_settop(L, cache - 1);
        return false;
    }
    if (!lua_isnil(L, -1)) {
        lua_replace(L, cache);
        lua_settop(L, cache);
        return true;
    }
    lua_pop(L, 1);

    std::string path;
    std::string source;
    if (!module_locate(key, path, source)) {
        snprintf(error, error_size, "module '%s' not found in workspace (looked for %s.lua)", name, key.c_str());
        lua_settop(L, cache - 1);
        return false;
    }

    lua_pushlightuserdata(L, &module_loading_tag);
    lua_setfield(L, cache, key.c_str());

    std::string chunk_name = "=" + path;
    int status = luaL_loadbuffer(L, source.data(), source.size(), chunk_name.c_str());
    if (status == 0) {
        lua_pushstring(L, name);
        status = lua_pcall(L, 1, 1, 0);
    }
    if (status != 0) {
        const char* message = lua_tostring(L, -1);
        snprintf(error, error_size, "%s", message ? message : "error while loading module");
        lua_pushnil(L);
        lua_setfield(L, cache, key.c_str());
        lua_settop(L, cache - 1);
        return false;
    }
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_pushboolean(L, 1);
    }
    lua_pushvalue(L, -1);
    lua_setfield(L, cache, key.c_str());
    lua_replace(L, cache);
    lua_settop(L, cache);
    return true;
}

static int l_require(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    char error[512];
    if (!require_module(L, name, error, sizeof(error))) return luaL_error(L, "%s", error);
    return 1;
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
