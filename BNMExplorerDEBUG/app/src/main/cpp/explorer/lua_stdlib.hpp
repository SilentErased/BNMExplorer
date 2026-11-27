#pragma once
#include <cstring>
#include <cctype>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <android/log.h>
#include "json.hpp"
#include "httplib.h"
#include "workspace.hpp"
#include "lua_objects.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

static const char* l_http_default_ua = "LuaBNM";

static void l_std_register(lua_State* L, const char* name, lua_CFunction fn) {
    lua_pushcfunction(L, fn);
    lua_setglobal(L, name);
}

static const Il2CppType* l_class_type(Il2CppClass* klass) {
    auto& m = BNM::Internal::il2cppMethods;
    return klass && m.il2cpp_class_get_type ? m.il2cpp_class_get_type(klass) : nullptr;
}

static bool l_http_parse_url(const std::string& url, bool& https, std::string& host, int& port, std::string& path) {
    https = false;
    port = 0;
    path = "/";
    std::string rest = url;
    if (rest.rfind("https://", 0) == 0) {
        https = true;
        rest = rest.substr(8);
    } else if (rest.rfind("http://", 0) == 0) {
        rest = rest.substr(7);
    } else {
        return false;
    }
    size_t slash = rest.find('/');
    std::string authority = slash == std::string::npos ? rest : rest.substr(0, slash);
    path = slash == std::string::npos ? "/" : rest.substr(slash);
    std::string userinfo_less = authority;
    size_t at = authority.rfind('@');
    if (at != std::string::npos) userinfo_less = authority.substr(at + 1);
    size_t colon = userinfo_less.rfind(':');
    if (colon != std::string::npos && colon + 1 < userinfo_less.size()) {
        host = userinfo_less.substr(0, colon);
        port = atoi(userinfo_less.c_str() + colon + 1);
    } else {
        host = userinfo_less;
        port = https ? 443 : 80;
    }
    if (host.empty() || port <= 0 || port > 65535) return false;
    if (path.empty()) path = "/";
    return true;
}

static void l_http_read_options(lua_State* L, int idx, std::string& url, std::string& method,
                                std::string& body, std::string& content_type,
                                httplib::Headers& headers, double& timeout, bool& follow) {
    if (lua_isstring(L, idx)) {
        url = lua_tostring(L, idx);
        method = "GET";
        return;
    }
    if (!lua_istable(L, idx)) luaL_error(L, "http_request: url string or options table expected");
    lua_getfield(L, idx, "url");
    if (lua_isstring(L, -1)) url = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, idx, "method");
    method = lua_isstring(L, -1) ? lua_tostring(L, -1) : "GET";
    lua_pop(L, 1);
    lua_getfield(L, idx, "body");
    if (lua_isstring(L, -1)) body = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, idx, "content_type");
    if (lua_isstring(L, -1)) content_type = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, idx, "timeout");
    timeout = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : 10.0;
    lua_pop(L, 1);
    lua_getfield(L, idx, "follow_redirects");
    follow = lua_isboolean(L, -1) ? lua_toboolean(L, -1) != 0 : true;
    lua_pop(L, 1);
    lua_getfield(L, idx, "headers");
    if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            if (lua_isstring(L, -2) && lua_isstring(L, -1))
                headers.emplace(lua_tostring(L, -2), lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    if (method.empty()) method = "GET";
    for (auto& c : method) c = (char)toupper((unsigned char)c);
}

static void l_http_push_body(lua_State* L, const httplib::Response& r) {
    lua_pushlstring(L, r.body.data(), r.body.size());
}

static int l_http_request(lua_State* L) {
    std::string url, method, body, content_type;
    httplib::Headers headers;
    double timeout = 10.0;
    bool follow = true;
    l_http_read_options(L, 1, url, method, body, content_type, headers, timeout, follow);
    if (url.empty()) return luaL_error(L, "http_request: url is required");
    bool https = false;
    std::string host, path;
    int port = 0;
    if (!l_http_parse_url(url, https, host, port, path))
        return luaL_error(L, "http_request: invalid url '%s'", url.c_str());
#ifndef BNM_ENABLE_HTTPS
    if (https) return luaL_error(L, "http_request: https requires the HTTPS build (BNM_ENABLE_HTTPS)");
#endif
    std::string scheme_host = (https ? "https://" : "http://") + host + ":" + std::to_string(port);
    if (headers.find("User-Agent") == headers.end()) headers.emplace("User-Agent", l_http_default_ua);
    if (!content_type.empty() && headers.find("Content-Type") == headers.end())
        headers.emplace("Content-Type", content_type);
    try {
        httplib::Client cli(scheme_host);
        time_t t_sec = (time_t)timeout;
        time_t t_usec = (time_t)((timeout - (double)t_sec) * 1000000.0);
        cli.set_connection_timeout(t_sec, t_usec);
        cli.set_read_timeout(t_sec, t_usec);
        cli.set_write_timeout(t_sec, t_usec);
        cli.set_follow_location(follow);
        httplib::Response r;
        httplib::Error err = httplib::Error::Success;
        if (method == "GET") {
            auto result = cli.Get(path, headers);
            if (!result) err = result.error();
            else r = std::move(*result);
        } else if (method == "POST") {
            auto result = cli.Post(path, headers, body, content_type.empty() ? "application/octet-stream" : content_type.c_str());
            if (!result) err = result.error();
            else r = std::move(*result);
        } else if (method == "PUT") {
            auto result = cli.Put(path, headers, body, content_type.empty() ? "application/octet-stream" : content_type.c_str());
            if (!result) err = result.error();
            else r = std::move(*result);
        } else if (method == "DELETE") {
            auto result = cli.Delete(path, headers);
            if (!result) err = result.error();
            else r = std::move(*result);
        } else if (method == "HEAD") {
            auto result = cli.Head(path, headers);
            if (!result) err = result.error();
            else r = std::move(*result);
        } else if (method == "PATCH") {
            auto result = cli.Patch(path, headers, body, content_type.empty() ? "application/octet-stream" : content_type.c_str());
            if (!result) err = result.error();
            else r = std::move(*result);
        } else if (method == "OPTIONS") {
            auto result = cli.Options(path);
            if (!result) err = result.error();
            else r = std::move(*result);
        } else {
            return luaL_error(L, "http_request: unsupported method '%s'", method.c_str());
        }
        if (err != httplib::Error::Success) {
            lua_newtable(L);
            lua_pushboolean(L, 0);
            lua_setfield(L, -2, "ok");
            lua_pushinteger(L, (lua_Integer)err);
            lua_setfield(L, -2, "error_code");
            lua_pushstring(L, httplib::to_string(err).c_str());
            lua_setfield(L, -2, "error");
            return 1;
        }
        lua_newtable(L);
        lua_pushboolean(L, 1);
        lua_setfield(L, -2, "ok");
        lua_pushinteger(L, r.status);
        lua_setfield(L, -2, "status");
        l_http_push_body(L, r);
        lua_setfield(L, -2, "body");
        lua_newtable(L);
        for (auto& h : r.headers) {
            lua_pushstring(L, h.second.c_str());
            lua_setfield(L, -2, h.first.c_str());
        }
        lua_setfield(L, -2, "headers");
        return 1;
    } catch (const std::exception& e) {
        return luaL_error(L, "http_request failed: %s", e.what());
    } catch (...) {
        return luaL_error(L, "http_request failed");
    }
}

static int l_http_get(lua_State* L) {
    if (lua_gettop(L) >= 2 && !lua_istable(L, 2)) luaL_error(L, "http_get: headers must be a table");
    lua_newtable(L);
    lua_pushvalue(L, 1);
    lua_setfield(L, -2, "url");
    lua_pushstring(L, "GET");
    lua_setfield(L, -2, "method");
    if (lua_istable(L, 2)) {
        lua_pushvalue(L, 2);
        lua_setfield(L, -2, "headers");
    }
    if (lua_isnumber(L, 3)) {
        lua_pushvalue(L, 3);
        lua_setfield(L, -2, "timeout");
    }
    lua_insert(L, 1);
    lua_settop(L, 1);
    return l_http_request(L);
}

static int l_http_post(lua_State* L) {
    lua_newtable(L);
    lua_pushvalue(L, 1);
    lua_setfield(L, -2, "url");
    lua_pushstring(L, "POST");
    lua_setfield(L, -2, "method");
    if (lua_isstring(L, 2)) {
        lua_pushvalue(L, 2);
        lua_setfield(L, -2, "body");
    }
    if (lua_istable(L, 3)) {
        lua_pushvalue(L, 3);
        lua_setfield(L, -2, "headers");
    }
    if (lua_isstring(L, 4)) {
        lua_pushvalue(L, 4);
        lua_setfield(L, -2, "content_type");
    } else if (lua_isstring(L, 2)) {
        lua_pushstring(L, "application/json");
        lua_setfield(L, -2, "content_type");
    }
    lua_insert(L, 1);
    lua_settop(L, 1);
    return l_http_request(L);
}

static void l_json_to_lua(lua_State* L, const nlohmann::json& j, int depth) {
    if (depth > 64) {
        lua_pushnil(L);
        return;
    }
    switch (j.type()) {
        case nlohmann::json::value_t::null:
        case nlohmann::json::value_t::discarded:
            lua_pushnil(L);
            break;
        case nlohmann::json::value_t::boolean:
            lua_pushboolean(L, j.get<bool>() ? 1 : 0);
            break;
        case nlohmann::json::value_t::number_integer:
            lua_pushnumber(L, (lua_Number)j.get<lua_Integer>());
            break;
        case nlohmann::json::value_t::number_unsigned:
            lua_pushnumber(L, (lua_Number)j.get<uint64_t>());
            break;
        case nlohmann::json::value_t::number_float:
            lua_pushnumber(L, j.get<double>());
            break;
        case nlohmann::json::value_t::string:
            lua_pushstring(L, j.get<std::string>().c_str());
            break;
        case nlohmann::json::value_t::array: {
            lua_newtable(L);
            int i = 0;
            for (const auto& item : j) {
                l_json_to_lua(L, item, depth + 1);
                lua_rawseti(L, -2, ++i);
            }
            break;
        }
        case nlohmann::json::value_t::object: {
            lua_newtable(L);
            for (auto it = j.begin(); it != j.end(); ++it) {
                lua_pushstring(L, it.key().c_str());
                l_json_to_lua(L, it.value(), depth + 1);
                lua_rawset(L, -3);
            }
            break;
        }
        default:
            lua_pushnil(L);
            break;
    }
}

static bool l_lua_to_json(lua_State* L, int idx, nlohmann::json& out, int depth);

static bool l_lua_table_is_array(lua_State* L, int idx) {
    lua_Integer n = 0;
    bool has_non_seq = false;
    lua_pushnil(L);
    while (lua_next(L, idx)) {
        if (lua_type(L, -2) == LUA_TNUMBER) {
            lua_Integer k = lua_tointeger(L, -2);
            if (k >= 1 && k == (lua_Integer)(lua_Number)k) {
                if (k > n) n = k;
                lua_pop(L, 1);
                continue;
            }
        }
        has_non_seq = true;
        lua_pop(L, 2);
        break;
    }
    (void)has_non_seq;
    return n > 0;
}

static bool l_lua_to_json(lua_State* L, int idx, nlohmann::json& out, int depth) {
    if (depth > 64) return false;
    idx = idx < 0 && idx > LUA_REGISTRYINDEX ? lua_gettop(L) + idx + 1 : idx;
    switch (lua_type(L, idx)) {
        case LUA_TNIL:
            out = nullptr;
            return true;
        case LUA_TBOOLEAN:
            out = lua_toboolean(L, idx) != 0;
            return true;
        case LUA_TNUMBER:
            out = lua_tonumber(L, idx);
            return true;
        case LUA_TSTRING:
            out = std::string(lua_tostring(L, idx));
            return true;
        case LUA_TTABLE: {
            if (l_lua_table_is_array(L, idx)) {
                out = nlohmann::json::array();
                size_t n = (size_t)lua_objlen(L, idx);
                for (size_t i = 1; i <= n; i++) {
                    lua_rawgeti(L, idx, (lua_Integer)i);
                    nlohmann::json item;
                    if (!l_lua_to_json(L, -1, item, depth + 1)) {
                        lua_pop(L, 1);
                        return false;
                    }
                    lua_pop(L, 1);
                    out.push_back(std::move(item));
                }
                return true;
            }
            out = nlohmann::json::object();
            lua_pushnil(L);
            while (lua_next(L, idx)) {
                if (lua_type(L, -2) == LUA_TSTRING) {
                    nlohmann::json item;
                    bool ok = l_lua_to_json(L, -1, item, depth + 1);
                    if (ok) out[lua_tostring(L, -2)] = std::move(item);
                    if (!ok) {
                        lua_pop(L, 2);
                        return false;
                    }
                }
                lua_pop(L, 1);
            }
            return true;
        }
        default:
            return false;
    }
}

static int l_json_encode(lua_State* L) {
    nlohmann::json out;
    if (!l_lua_to_json(L, 1, out, 0)) return luaL_error(L, "json.encode: unsupported value");
    lua_pushstring(L, out.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace).c_str());
    return 1;
}

static int l_json_decode(lua_State* L) {
    const char* text = luaL_checkstring(L, 1);
    nlohmann::json parsed = nlohmann::json::parse(text, nullptr, false);
    if (parsed.is_discarded()) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid json");
        return 2;
    }
    l_json_to_lua(L, parsed, 0);
    return 1;
}

static int l_json_index(lua_State* L) {
    const char* key = luaL_checkstring(L, 2);
    if (!strcmp(key, "encode") || !strcmp(key, "stringify")) {
        lua_pushcfunction(L, l_json_encode);
        return 1;
    }
    if (!strcmp(key, "decode") || !strcmp(key, "parse")) {
        lua_pushcfunction(L, l_json_decode);
        return 1;
    }
    return luaL_error(L, "json has no member '%s'", key);
}

static int l_fs_list(lua_State* L) {
    const char* dir = lua_gettop(L) >= 1 && lua_isstring(L, 1) ? lua_tostring(L, 1) : "";
    std::string prefix;
    if (*dir) {
        prefix = ws_sanitize_path(dir, false);
        if (prefix.empty()) return luaL_error(L, "fs.list: invalid directory");
        prefix += "/";
    }
    lua_newtable(L);
    int row = 0;
    for (const auto& e : ws_list()) {
        if (!prefix.empty() && e.path.rfind(prefix, 0) != 0) continue;
        std::string rel = e.path.substr(prefix.size());
        if (rel.find('/') != std::string::npos) continue;
        lua_newtable(L);
        lua_pushstring(L, e.path.c_str());
        lua_setfield(L, -2, "path");
        lua_pushstring(L, rel.c_str());
        lua_setfield(L, -2, "name");
        lua_pushboolean(L, e.is_dir ? 1 : 0);
        lua_setfield(L, -2, "dir");
        lua_pushboolean(L, e.path.size() > 4 && e.path.compare(e.path.size() - 4, 4, ".lua") == 0 ? 1 : 0);
        lua_setfield(L, -2, "lua");
        lua_pushnumber(L, (double)e.size);
        lua_setfield(L, -2, "size");
        lua_rawseti(L, -2, ++row);
    }
    return 1;
}

static int l_fs_read(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    std::string clean = ws_sanitize_path(path, false);
    if (clean.empty()) return luaL_error(L, "fs.read: invalid path");
    std::string data;
    if (!ws_read_file(clean, data)) {
        lua_pushnil(L);
        lua_pushfstring(L, "file not found: %s", clean.c_str());
        return 2;
    }
    lua_pushlstring(L, data.data(), data.size());
    return 1;
}

static int l_fs_write(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    size_t size = 0;
    const char* data = luaL_optlstring(L, 2, "", &size);
    std::string clean = ws_sanitize_path(path, false);
    if (clean.empty()) return luaL_error(L, "fs.write: invalid path");
    if (size > ws_max_file_size) return luaL_error(L, "fs.write: file exceeds 1 MB limit");
    if (!ws_write_file(clean, std::string(data, size))) return luaL_error(L, "fs.write: failed to write");
    lua_pushboolean(L, 1);
    return 1;
}

static int l_fs_exists(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    std::string clean = ws_sanitize_path(path, false);
    bool ok = false;
    if (!clean.empty()) {
        std::string full = ws_workspace_dir() + "/" + clean;
        struct stat st;
        ok = stat(full.c_str(), &st) == 0;
    }
    lua_pushboolean(L, ok ? 1 : 0);
    return 1;
}

static int l_fs_size(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    std::string clean = ws_sanitize_path(path, false);
    long size = -1;
    if (!clean.empty()) {
        std::string full = ws_workspace_dir() + "/" + clean;
        struct stat st;
        if (stat(full.c_str(), &st) == 0 && S_ISREG(st.st_mode)) size = (long)st.st_size;
    }
    if (size < 0) return luaL_error(L, "fs.size: file not found");
    lua_pushnumber(L, (double)size);
    return 1;
}

static int l_fs_delete(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    std::string clean = ws_sanitize_path(path, false);
    if (clean.empty() || !ws_delete(clean)) {
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_pushboolean(L, 1);
    return 1;
}

static int l_fs_mkdir(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    std::string clean = ws_sanitize_path(path, false);
    if (clean.empty() || !ws_make_folder(clean)) return luaL_error(L, "fs.mkdir: invalid folder name");
    lua_pushboolean(L, 1);
    return 1;
}

static int l_bit_band(lua_State* L) {
    uint64_t acc = ~0ULL;
    for (int i = 1; i <= lua_gettop(L); i++) acc &= (uint64_t)(int64_t)luaL_checknumber(L, i);
    lua_pushnumber(L, (lua_Number)(int64_t)acc);
    return 1;
}

static int l_bit_bor(lua_State* L) {
    uint64_t acc = 0;
    for (int i = 1; i <= lua_gettop(L); i++) acc |= (uint64_t)(int64_t)luaL_checknumber(L, i);
    lua_pushnumber(L, (lua_Number)(int64_t)acc);
    return 1;
}

static int l_bit_bxor(lua_State* L) {
    uint64_t acc = 0;
    for (int i = 1; i <= lua_gettop(L); i++) acc ^= (uint64_t)(int64_t)luaL_checknumber(L, i);
    lua_pushnumber(L, (lua_Number)(int64_t)acc);
    return 1;
}

static int l_bit_lshift(lua_State* L) {
    uint64_t v = (uint64_t)(int64_t)luaL_checknumber(L, 1);
    int n = (int)luaL_checknumber(L, 2);
    lua_pushnumber(L, (lua_Number)(int64_t)(n >= 64 ? 0 : v << n));
    return 1;
}

static int l_bit_rshift(lua_State* L) {
    uint64_t v = (uint64_t)(int64_t)luaL_checknumber(L, 1);
    int n = (int)luaL_checknumber(L, 2);
    lua_pushnumber(L, (lua_Number)(int64_t)(n >= 64 ? 0 : v >> n));
    return 1;
}

static int l_bit_arshift(lua_State* L) {
    int64_t v = (int64_t)luaL_checknumber(L, 1);
    int n = (int)luaL_checknumber(L, 2);
    lua_pushnumber(L, (lua_Number)(n >= 64 ? (v < 0 ? -1 : 0) : v >> n));
    return 1;
}

static int l_bit_bnot(lua_State* L) {
    uint64_t v = (uint64_t)(int64_t)luaL_checknumber(L, 1);
    lua_pushnumber(L, (lua_Number)(int64_t)~v);
    return 1;
}

static void l_b64_encode(lua_State* L, const uint8_t* data, size_t size) {
    static const char* table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve((size + 2) / 3 * 4);
    for (size_t i = 0; i < size; i += 3) {
        uint32_t chunk = (uint32_t)data[i] << 16;
        if (i + 1 < size) chunk |= (uint32_t)data[i + 1] << 8;
        if (i + 2 < size) chunk |= data[i + 2];
        out += table[(chunk >> 18) & 63];
        out += table[(chunk >> 12) & 63];
        out += i + 1 < size ? table[(chunk >> 6) & 63] : '=';
        out += i + 2 < size ? table[chunk & 63] : '=';
    }
    lua_pushlstring(L, out.data(), out.size());
}

static int l_b64_encode_fn(lua_State* L) {
    size_t size = 0;
    const char* data = luaL_checklstring(L, 1, &size);
    l_b64_encode(L, (const uint8_t*)data, size);
    return 1;
}

static int l_b64_decode_fn(lua_State* L) {
    size_t size = 0;
    const char* data = luaL_checklstring(L, 1, &size);
    auto value_of = [](char c) -> int {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        return -1;
    };
    std::string out;
    out.reserve(size / 4 * 3 + 3);
    uint32_t chunk = 0;
    int bits = 0;
    for (size_t i = 0; i < size; i++) {
        char c = data[i];
        if (c == '=' || c == '\n' || c == '\r') break;
        int v = value_of(c);
        if (v < 0) continue;
        chunk = (chunk << 6) | (uint32_t)v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out += (char)((chunk >> bits) & 0xFF);
        }
    }
    lua_pushlstring(L, out.data(), out.size());
    return 1;
}

static int l_mem_read_int(lua_State* L, int size, bool floating, bool is_signed) {
    uintptr_t addr = (uintptr_t)(uint64_t)luaL_checknumber(L, 1);
    if (!address_plausible(addr) || !is_readable_bytes((const void*)addr, (size_t)size)) {
        lua_pushnil(L);
        return 1;
    }
    if (floating && size == 4) {
        float v;
        memcpy(&v, (const void*)addr, 4);
        lua_pushnumber(L, v);
        return 1;
    }
    if (floating && size == 8) {
        double v;
        memcpy(&v, (const void*)addr, 8);
        lua_pushnumber(L, v);
        return 1;
    }
    uint64_t v = 0;
    memcpy(&v, (const void*)addr, (size_t)size);
    if (is_signed && size < 8) {
        uint64_t sign = 1ULL << (size * 8 - 1);
        if (v & sign) v |= ~((sign << 1) - 1);
    }
    lua_pushnumber(L, (lua_Number)(int64_t)v);
    return 1;
}

static int l_mem_read8(lua_State* L) { return l_mem_read_int(L, 1, false, true); }
static int l_mem_read16(lua_State* L) { return l_mem_read_int(L, 2, false, true); }
static int l_mem_read32(lua_State* L) { return l_mem_read_int(L, 4, false, true); }
static int l_mem_read64(lua_State* L) { return l_mem_read_int(L, 8, false, true); }
static int l_mem_read_float(lua_State* L) { return l_mem_read_int(L, 4, true, false); }
static int l_mem_read_double(lua_State* L) { return l_mem_read_int(L, 8, true, false); }

static int l_mem_read_bytes(lua_State* L) {
    uintptr_t addr = (uintptr_t)(uint64_t)luaL_checknumber(L, 1);
    lua_Integer len = (lua_Integer)luaL_checknumber(L, 2);
    if (len < 0 || (size_t)len > 65536) return luaL_error(L, "memory.read_bytes: length must be 0..65536");
    if (!address_plausible(addr) || !is_readable_bytes((const void*)addr, (size_t)len)) {
        lua_pushnil(L);
        return 1;
    }
    lua_pushlstring(L, (const char*)addr, (size_t)len);
    return 1;
}

static int l_mem_read_string(lua_State* L) {
    uintptr_t addr = (uintptr_t)(uint64_t)luaL_checknumber(L, 1);
    lua_Integer max_len = (lua_Integer)luaL_optnumber(L, 2, 256);
    if (max_len < 0 || (size_t)max_len > 65536) max_len = 65536;
    if (!address_plausible(addr)) {
        lua_pushnil(L);
        return 1;
    }
    std::string out;
    for (lua_Integer i = 0; i < max_len; i++) {
        if (!is_readable_bytes((const void*)(addr + (size_t)i), 1)) break;
        char c = *(const char*)(addr + (size_t)i);
        if (c == '\0') break;
        out += c;
    }
    lua_pushlstring(L, out.data(), out.size());
    return 1;
}

static void l_mem_write_raw(lua_State* L, uintptr_t addr, const void* data, size_t size) {
    if (!address_plausible(addr)) luaL_error(L, "memory.write: invalid address");
    memcpy((void*)addr, data, size);
}

static int l_mem_write_int(lua_State* L, int size, bool floating) {
    uintptr_t addr = (uintptr_t)(uint64_t)luaL_checknumber(L, 1);
    if (floating) {
        double n = luaL_checknumber(L, 2);
        if (size == 4) {
            float f = (float)n;
            l_mem_write_raw(L, addr, &f, 4);
        } else {
            l_mem_write_raw(L, addr, &n, 8);
        }
        return 0;
    }
    int64_t n = (int64_t)luaL_checknumber(L, 2);
    l_mem_write_raw(L, addr, &n, (size_t)size);
    return 0;
}

static int l_mem_write8(lua_State* L) { return l_mem_write_int(L, 1, false); }
static int l_mem_write16(lua_State* L) { return l_mem_write_int(L, 2, false); }
static int l_mem_write32(lua_State* L) { return l_mem_write_int(L, 4, false); }
static int l_mem_write64(lua_State* L) { return l_mem_write_int(L, 8, false); }
static int l_mem_write_float(lua_State* L) { return l_mem_write_int(L, 4, true); }
static int l_mem_write_double(lua_State* L) { return l_mem_write_int(L, 8, true); }

static int l_mem_write_bytes(lua_State* L) {
    uintptr_t addr = (uintptr_t)(uint64_t)luaL_checknumber(L, 1);
    size_t size = 0;
    const char* data = luaL_checklstring(L, 2, &size);
    if (size > 65536) return luaL_error(L, "memory.write_bytes: data too large");
    l_mem_write_raw(L, addr, data, size);
    lua_pushboolean(L, 1);
    return 1;
}

static int l_mem_nop(lua_State* L) {
    uintptr_t addr = (uintptr_t)(uint64_t)luaL_checknumber(L, 1);
    lua_Integer len = (lua_Integer)luaL_checknumber(L, 2);
    if (len <= 0 || len > 1024) return luaL_error(L, "memory.nop: length must be 1..1024");
    static const uint8_t arm64_nop[4] = {0x1F, 0x20, 0x03, 0xD5};
    uintptr_t page_start = addr & ~(uintptr_t)(getpagesize() - 1);
    size_t span = (size_t)(((addr + (size_t)len) - page_start + getpagesize() - 1) / getpagesize()) * getpagesize();
    if (mprotect((void*)page_start, span, PROT_READ | PROT_WRITE) != 0)
        return luaL_error(L, "memory.nop: mprotect failed");
    for (lua_Integer i = 0; i + 4 <= len; i += 4)
        memcpy((void*)(addr + (size_t)i), arm64_nop, 4);
    mprotect((void*)page_start, span, PROT_READ | PROT_EXEC);
    __builtin___clear_cache((char*)addr, (char*)(addr + (size_t)len));
    lua_pushboolean(L, 1);
    return 1;
}

static int l_mem_index(lua_State* L) {
    const char* key = luaL_checkstring(L, 2);
    struct entry {
        const char* name;
        lua_CFunction fn;
    };
    static const entry table[] = {
            {"read8", l_mem_read8}, {"read16", l_mem_read16}, {"read32", l_mem_read32}, {"read64", l_mem_read64},
            {"read_float", l_mem_read_float}, {"read_double", l_mem_read_double},
            {"read_bytes", l_mem_read_bytes}, {"read_string", l_mem_read_string},
            {"write8", l_mem_write8}, {"write16", l_mem_write16}, {"write32", l_mem_write32}, {"write64", l_mem_write64},
            {"write_float", l_mem_write_float}, {"write_double", l_mem_write_double},
            {"write_bytes", l_mem_write_bytes}, {"nop", l_mem_nop},
            {nullptr, nullptr}
    };
    for (const entry* e = table; e->name; e++) {
        if (!strcmp(key, e->name)) {
            lua_pushcfunction(L, e->fn);
            return 1;
        }
    }
    return luaL_error(L, "memory has no member '%s'", key);
}

static int l_enum_values(lua_State* L) {
    Il2CppClass* klass = class_from_arg(L, 1);
    if (!klass) return luaL_error(L, "enum class not found");
    auto& m = BNM::Internal::il2cppMethods;
    if (m.il2cpp_runtime_class_init) m.il2cpp_runtime_class_init(klass);
    lua_newtable(L);
    try {
        Class c(klass);
        for (auto& f : c.GetFields(false)) {
            auto* fi = f.GetInfo();
            if (!fi || !fi->name || !fi->type) continue;
            if (!(fi->type->attrs & 0x10)) continue;
            if (!strcmp(fi->name, "value__")) continue;
            int32_t v = 0;
            m.il2cpp_field_static_get_value(fi, &v);
            lua_pushnumber(L, v);
            lua_setfield(L, -2, fi->name);
        }
    } catch (...) {
    }
    return 1;
}

static int l_enum_name(lua_State* L) {
    Il2CppClass* klass = class_from_arg(L, 1);
    if (!klass) return luaL_error(L, "enum class not found");
    int32_t wanted = (int32_t)luaL_checknumber(L, 2);
    auto& m = BNM::Internal::il2cppMethods;
    if (m.il2cpp_runtime_class_init) m.il2cpp_runtime_class_init(klass);
    try {
        Class c(klass);
        for (auto& f : c.GetFields(false)) {
            auto* fi = f.GetInfo();
            if (!fi || !fi->name || !fi->type) continue;
            if (!(fi->type->attrs & 0x10)) continue;
            if (!strcmp(fi->name, "value__")) continue;
            int32_t v = 0;
            m.il2cpp_field_static_get_value(fi, &v);
            if (v == wanted) {
                lua_pushstring(L, fi->name);
                return 1;
            }
        }
    } catch (...) {
    }
    lua_pushnil(L);
    return 1;
}

static void l_push_array_element(lua_State* L, Il2CppClass* elem_class, const uint8_t* data, size_t elem_size) {
    value_kind k = kind_from_type(l_class_type(elem_class));
    if (k == value_kind::boolean) {
        lua_pushboolean(L, *data != 0);
        return;
    }
    if (kind_is_numeric(k) || kind_is_vector(k)) {
        push_from_memory(L, k, data);
        return;
    }
    if (bnm_type_is_value_type(l_class_type(elem_class))) {
        push_object(L, box_struct_value(elem_class, data));
        return;
    }
    Il2CppObject* ref = nullptr;
    memcpy(&ref, data, sizeof(ref));
    push_object(L, ref);
}

static int l_to_table(lua_State* L) {
    bool is_ref = false;
    uintptr_t addr = ref_address_at(L, 1, is_ref);
    if (!addr || !address_plausible(addr)) {
        lua_pushnil(L);
        return 1;
    }
    auto* obj = (Il2CppObject*)addr;
    Il2CppClass* klass = obj->klass;
    if (!klass || !address_plausible((uintptr_t)klass)) {
        lua_pushnil(L);
        return 1;
    }
    Il2CppObject* items = obj;
    int count = 0;
    bool count_known = false;
    std::string name = klass->name ? klass->name : "";
    if (name.size() > 2 && name.compare(name.size() - 2, 2, "[]") == 0) {
        count_known = false;
    } else {
        member_info items_m = find_member(klass, "_items");
        member_info size_m = find_member(klass, "_size");
        if (items_m.field && size_m.field) {
            items = *(Il2CppObject**)((uint8_t*)addr + items_m.field->offset);
            count = *(int32_t*)((uint8_t*)addr + size_m.field->offset);
            count_known = true;
            if (!items || !address_plausible((uintptr_t)items) || count < 0) {
                lua_pushnil(L);
                return 1;
            }
        } else {
            lua_pushnil(L);
            return 1;
        }
    }
    Il2CppClass* arr_klass = items->klass;
    if (!arr_klass || !arr_klass->element_class) {
        lua_pushnil(L);
        return 1;
    }
    Il2CppClass* elem_class = arr_klass->element_class;
    int32_t capacity = *(int32_t*)((uint8_t*)items + 0x18);
    if (!count_known || count > capacity) count = capacity;
    if (count < 0 || count > 1000000) count = 0;
    size_t elem_size = arr_klass->element_size ? arr_klass->element_size : sizeof(void*);
    const uint8_t* base = (const uint8_t*)items + 0x20;
    if (count > 0 && !is_readable_bytes(base, elem_size)) {
        lua_pushnil(L);
        return 1;
    }
    lua_newtable(L);
    for (int i = 0; i < count; i++) {
        l_push_array_element(L, elem_class, base + (size_t)i * elem_size, elem_size);
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

static int l_make_array(lua_State* L) {
    Il2CppClass* elem_class = class_from_arg(L, 1);
    if (!elem_class) return luaL_error(L, "element class not found");
    luaL_checktype(L, 2, LUA_TTABLE);
    int n = (int)lua_objlen(L, 2);
    auto& m = BNM::Internal::il2cppMethods;
    if (!m.il2cpp_array_new) return luaL_error(L, "il2cpp_array_new unavailable");
    auto* arr = (Array<Il2CppObject*>*)m.il2cpp_array_new(elem_class, (il2cpp_array_size_t)n);
    if (!arr) return luaL_error(L, "array allocation failed");
    const Il2CppType* elem_type = l_class_type(elem_class);
    bool elem_is_string = elem_class == find_class_cached("System.String", "");
    bool elem_is_value = bnm_type_is_value_type(elem_type);
    value_kind elem_kind = elem_is_value ? kind_from_type(elem_type) : value_kind::object;
    size_t elem_size = arr->klass->element_size ? arr->klass->element_size : sizeof(void*);
    uint8_t* base = (uint8_t*)arr->m_Items;
    for (int i = 0; i < n; i++) {
        lua_rawgeti(L, 2, i + 1);
        uint8_t* slot = base + (size_t)i * elem_size;
        if (lua_isnil(L, -1)) {
            memset(slot, 0, elem_size);
        } else if (elem_is_string && lua_isstring(L, -1)) {
            auto* s = CreateMonoString(lua_tostring(L, -1));
            memcpy(slot, &s, sizeof(s));
        } else if (lua_isnumber(L, -1) && (kind_is_numeric(elem_kind) || kind_is_vector(elem_kind))) {
            double num = lua_tonumber(L, -1);
            if (kind_is_vector(elem_kind)) {
                float f = (float)num;
                for (size_t c = 0; c < kind_components(elem_kind) && c * 4 < elem_size; c++)
                    memcpy(slot + c * 4, &f, 4);
            } else {
                int64_t v = (int64_t)num;
                size_t w = kind_size(elem_kind);
                if (w == 4) {
                    int32_t v32 = (int32_t)v;
                    memcpy(slot, &v32, 4);
                } else if (w == 8) {
                    memcpy(slot, &v, 8);
                } else if (w == 2) {
                    int16_t v16 = (int16_t)v;
                    memcpy(slot, &v16, 2);
                } else if (w == 1) {
                    int8_t v8 = (int8_t)v;
                    memcpy(slot, &v8, 1);
                }
            }
        } else {
            bool is_ref = false;
            uintptr_t target = ref_address_at(L, -1, is_ref);
            if (elem_is_value) {
                const uint8_t* src = nullptr;
                if (auto* r = (lua_ref*)test_udata(L, -1, mt_ref)) {
                    if (r->inst && address_plausible(r->inst)) src = (const uint8_t*)r->inst + sizeof(Il2CppObject);
                } else if (target && address_plausible(target)) {
                    src = (const uint8_t*)target;
                }
                if (!src || !is_readable_bytes(src, elem_size)) {
                    lua_pop(L, 1);
                    return luaL_error(L, "make_array: element %d is not a boxed struct", i + 1);
                }
                memcpy(slot, src, elem_size);
            } else {
                memcpy(slot, &target, sizeof(target));
            }
        }
        lua_pop(L, 1);
    }
    push_object(L, (Il2CppObject*)arr);
    return 1;
}

static int l_make_byte_array(lua_State* L) {
    auto& m = BNM::Internal::il2cppMethods;
    if (!m.il2cpp_array_new) return luaL_error(L, "il2cpp_array_new unavailable");
    Il2CppClass* byte_class = find_class_cached("System.Byte", "");
    if (!byte_class) return luaL_error(L, "System.Byte not found");
    std::string data;
    if (lua_isstring(L, 1)) {
        size_t size = 0;
        const char* raw = lua_tolstring(L, 1, &size);
        data.assign(raw, size);
    } else if (lua_istable(L, 1)) {
        int n = (int)lua_objlen(L, 1);
        data.resize((size_t)n);
        for (int i = 0; i < n; i++) {
            lua_rawgeti(L, 1, i + 1);
            data[(size_t)i] = lua_isnumber(L, -1) ? (char)(uint8_t)lua_tonumber(L, -1) : 0;
            lua_pop(L, 1);
        }
    } else {
        return luaL_error(L, "make_byte_array: string or table expected");
    }
    auto* arr = (Array<uint8_t>*)m.il2cpp_array_new(byte_class, (il2cpp_array_size_t)data.size());
    if (!arr) return luaL_error(L, "array allocation failed");
    if (!data.empty()) memcpy(arr->m_Items, data.data(), data.size());
    push_object(L, (Il2CppObject*)arr);
    return 1;
}

static int l_hex(lua_State* L) {
    lua_pushfstring(L, "0x%llX", (unsigned long long)(uint64_t)(int64_t)luaL_checknumber(L, 1));
    return 1;
}

static int l_parse_hex(lua_State* L) {
    const char* text = luaL_checkstring(L, 1);
    while (*text == ' ' || *text == '\t') text++;
    if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) text += 2;
    lua_pushnumber(L, (lua_Number)(int64_t)strtoull(text, nullptr, 16));
    return 1;
}

static int l_set_timescale(lua_State* L) {
    float v = (float)luaL_checknumber(L, 1);
    Class time_cls("UnityEngine", "Time");
    if (!time_cls.IsValid()) return luaL_error(L, "UnityEngine.Time not found");
    Method<void> setter = time_cls.GetMethod("set_timeScale", 1);
    if (!setter.IsValid()) return luaL_error(L, "Time.timeScale setter not found");
    setter(v);
    return 0;
}

static int l_get_timescale(lua_State* L) {
    Class time_cls("UnityEngine", "Time");
    if (!time_cls.IsValid()) return luaL_error(L, "UnityEngine.Time not found");
    Method<float> getter = time_cls.GetMethod("get_timeScale", 0);
    lua_pushnumber(L, getter.IsValid() ? getter() : 1.0f);
    return 1;
}

static int l_log_with_level(lua_State* L, android_LogPriority priority) {
    int n = lua_gettop(L);
    std::string line;
    for (int i = 1; i <= n; i++) {
        if (i > 1) line += " ";
        line += describe_value(L, i);
    }
    __android_log_print(priority, "LuaBNM", "%s", line.c_str());
    rt_append_output(line);
    return 0;
}

static int l_log_info(lua_State* L) { return l_log_with_level(L, ANDROID_LOG_INFO); }
static int l_log_warn(lua_State* L) { return l_log_with_level(L, ANDROID_LOG_WARN); }
static int l_log_error(lua_State* L) { return l_log_with_level(L, ANDROID_LOG_ERROR); }

static int l_il2cpp_loaded(lua_State* L) {
    lua_pushboolean(L, BNM::Loading::IsLoaded() ? 1 : 0);
    return 1;
}

static void l_register_stdlib(lua_State* L) {
    lua_newtable(L);
    lua_pushcfunction(L, l_http_request);
    lua_setfield(L, -2, "request");
    lua_pushcfunction(L, l_http_get);
    lua_setfield(L, -2, "get");
    lua_pushcfunction(L, l_http_post);
    lua_setfield(L, -2, "post");
    lua_setglobal(L, "http");

    lua_newtable(L);
    lua_pushcfunction(L, l_json_encode);
    lua_setfield(L, -2, "encode");
    lua_pushcfunction(L, l_json_decode);
    lua_setfield(L, -2, "decode");
    lua_newtable(L);
    lua_pushcfunction(L, l_json_index);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    lua_setglobal(L, "json");

    lua_newtable(L);
    lua_pushcfunction(L, l_fs_list);
    lua_setfield(L, -2, "list");
    lua_pushcfunction(L, l_fs_read);
    lua_setfield(L, -2, "read");
    lua_pushcfunction(L, l_fs_write);
    lua_setfield(L, -2, "write");
    lua_pushcfunction(L, l_fs_exists);
    lua_setfield(L, -2, "exists");
    lua_pushcfunction(L, l_fs_size);
    lua_setfield(L, -2, "size");
    lua_pushcfunction(L, l_fs_delete);
    lua_setfield(L, -2, "delete");
    lua_pushcfunction(L, l_fs_mkdir);
    lua_setfield(L, -2, "mkdir");
    lua_setglobal(L, "fs");

    lua_newtable(L);
    lua_pushcfunction(L, l_bit_band);
    lua_setfield(L, -2, "band");
    lua_pushcfunction(L, l_bit_bor);
    lua_setfield(L, -2, "bor");
    lua_pushcfunction(L, l_bit_bxor);
    lua_setfield(L, -2, "bxor");
    lua_pushcfunction(L, l_bit_lshift);
    lua_setfield(L, -2, "lshift");
    lua_pushcfunction(L, l_bit_rshift);
    lua_setfield(L, -2, "rshift");
    lua_pushcfunction(L, l_bit_arshift);
    lua_setfield(L, -2, "arshift");
    lua_pushcfunction(L, l_bit_bnot);
    lua_setfield(L, -2, "bnot");
    lua_setglobal(L, "bit");

    lua_newtable(L);
    lua_pushcfunction(L, l_b64_encode_fn);
    lua_setfield(L, -2, "encode");
    lua_pushcfunction(L, l_b64_decode_fn);
    lua_setfield(L, -2, "decode");
    lua_setglobal(L, "base64");

    lua_newtable(L);
    lua_newtable(L);
    lua_pushcfunction(L, l_mem_index);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    lua_setglobal(L, "memory");

    l_std_register(L, "enum_values", l_enum_values);
    l_std_register(L, "enum_name", l_enum_name);
    l_std_register(L, "to_table", l_to_table);
    l_std_register(L, "make_array", l_make_array);
    l_std_register(L, "make_byte_array", l_make_byte_array);
    l_std_register(L, "hex", l_hex);
    l_std_register(L, "parse_hex", l_parse_hex);
    l_std_register(L, "set_timescale", l_set_timescale);
    l_std_register(L, "get_timescale", l_get_timescale);
    l_std_register(L, "log_info", l_log_info);
    l_std_register(L, "log_warn", l_log_warn);
    l_std_register(L, "log_error", l_log_error);
    l_std_register(L, "il2cpp_loaded", l_il2cpp_loaded);
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
