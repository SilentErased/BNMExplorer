#pragma once
#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <android/log.h>
#include "json.hpp"
#include "workspace.hpp"
#include "luabnm.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

static void mgr_logi(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_INFO, "BNMExplorer", fmt, args);
    va_end(args);
}

static void mgr_logw(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_WARN, "BNMExplorer", fmt, args);
    va_end(args);
}

struct manager_state {
    std::atomic<bool> hold{false};
    std::atomic<bool> holding{false};
    std::atomic<bool> auto_lua{false};
    std::atomic<int> hold_timeout{0};
    std::atomic<int> resumed{0};
    std::atomic<int> boot_count{0};
    std::atomic<double> holding_since{0.0};
    std::atomic<double> last_hold_seconds{0.0};
    std::mutex cfg_mtx;
    std::string auto_lua_path = "main.lua";
    std::string last_output;
    std::string last_error;
};

static manager_state g_mgr;
static const char* manager_config_path_suffix = "/manager.json";

static std::string manager_config_file() {
    return g_ws_base + manager_config_path_suffix;
}

static double mgr_now() {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

static void manager_config_save_locked() {
    nlohmann::json j;
    j["hold"] = g_mgr.hold.load();
    j["auto_lua"] = g_mgr.auto_lua.load();
    j["auto_lua_path"] = g_mgr.auto_lua_path;
    j["hold_timeout"] = g_mgr.hold_timeout.load();
    ws_write_raw(manager_config_file(), j.dump());
}

static void manager_config_load() {
    std::string data;
    {
        std::string full = manager_config_file();
        FILE* f = fopen(full.c_str(), "rb");
        if (!f) return;
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);
        if (size < 0 || (size_t)size > 16384) { fclose(f); return; }
        data.assign((size_t)size, '\0');
        size_t got = size > 0 ? fread(&data[0], 1, (size_t)size, f) : 0;
        fclose(f);
        if (got != (size_t)size) return;
    }
    nlohmann::json j = nlohmann::json::parse(data, nullptr, false);
    if (!j.is_object()) return;
    if (j.contains("hold") && j["hold"].is_boolean()) g_mgr.hold.store(j["hold"].get<bool>());
    if (j.contains("auto_lua") && j["auto_lua"].is_boolean()) g_mgr.auto_lua.store(j["auto_lua"].get<bool>());
    if (j.contains("auto_lua_path") && j["auto_lua_path"].is_string()) g_mgr.auto_lua_path = j["auto_lua_path"].get<std::string>();
    if (j.contains("hold_timeout") && j["hold_timeout"].is_number_integer()) g_mgr.hold_timeout.store(j["hold_timeout"].get<int>());
}

static void manager_set_hold(bool on) {
    std::lock_guard<std::mutex> lk(g_mgr.cfg_mtx);
    g_mgr.hold.store(on);
    if (!on) g_mgr.last_hold_seconds.store(0.0);
    manager_config_save_locked();
}

static void manager_set_options(bool auto_lua, const std::string& path, int timeout) {
    std::lock_guard<std::mutex> lk(g_mgr.cfg_mtx);
    g_mgr.auto_lua.store(auto_lua);
    if (!path.empty()) {
        std::string clean = ws_sanitize_path(path, false);
        if (!clean.empty()) g_mgr.auto_lua_path = clean;
    }
    g_mgr.hold_timeout.store(timeout < 0 ? 0 : timeout);
    manager_config_save_locked();
}

static void manager_run_auto_lua() {
    std::string path = g_mgr.auto_lua_path.empty() ? "main.lua" : g_mgr.auto_lua_path;
    mgr_logi("[Manager] Auto Lua running workspace file: %s", path.c_str());
    std::string out = execute_lua_workspace(path);
    {
        std::lock_guard<std::mutex> lk(g_mgr.cfg_mtx);
        g_mgr.last_output = out;
    }
    mgr_logi("[Manager] Auto Lua finished: %s", out.substr(0, 512).c_str());
}

static void manager_boot() {
    ws_init();
    manager_config_load();
    g_mgr.boot_count.fetch_add(1);
    mgr_logi("[Manager] boot #%d hold=%d auto_lua=%d path=%s timeout=%ds",
         g_mgr.boot_count.load(), g_mgr.hold.load() ? 1 : 0, g_mgr.auto_lua.load() ? 1 : 0,
         g_mgr.auto_lua_path.c_str(), g_mgr.hold_timeout.load());
    if (g_mgr.hold.load()) {
        g_mgr.holding.store(true);
        g_mgr.holding_since.store(mgr_now());
        mgr_logi("[Manager] Game startup is frozen for hook setup. Open the web UI and press Resume. Timeout=%ds (0=none)",
             g_mgr.hold_timeout.load());
        double deadline = g_mgr.hold_timeout.load() > 0 ? mgr_now() + g_mgr.hold_timeout.load() : 0.0;
        while (g_mgr.hold.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (deadline > 0.0 && mgr_now() > deadline) {
                mgr_logw("[Manager] Hold timeout reached, resuming automatically");
                g_mgr.hold.store(false);
            }
        }
        double held = mgr_now() - g_mgr.holding_since.load();
        g_mgr.last_hold_seconds.store(held);
        g_mgr.holding.store(false);
        g_mgr.resumed.fetch_add(1);
        manager_set_hold(false);
        mgr_logi("[Manager] Resumed after %.1f s", held);
    }
    if (g_mgr.auto_lua.load()) manager_run_auto_lua();
}

static void manager_reply(httplib::Response& res, const nlohmann::json& j, int status = 200) {
    res.status = status;
    res.set_content(j.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace), "application/json");
}

static void register_manager_routes(httplib::Server& svr) {
    svr.Get("/api/manager", [](const httplib::Request&, httplib::Response& res) {
        nlohmann::json j;
        j["hold"] = g_mgr.hold.load();
        j["holding"] = g_mgr.holding.load();
        j["auto_lua"] = g_mgr.auto_lua.load();
        {
            std::lock_guard<std::mutex> lk(g_mgr.cfg_mtx);
            j["auto_lua_path"] = g_mgr.auto_lua_path;
            j["last_output"] = g_mgr.last_output;
            j["last_error"] = g_mgr.last_error;
        }
        j["hold_timeout"] = g_mgr.hold_timeout.load();
        j["resumed"] = g_mgr.resumed.load();
        j["boot_count"] = g_mgr.boot_count.load();
        j["last_hold_seconds"] = g_mgr.last_hold_seconds.load();
        if (g_mgr.holding.load()) j["holding_for"] = mgr_now() - g_mgr.holding_since.load();
        j["config"] = manager_config_file();
        manager_reply(res, j);
    });

    svr.Post("/api/manager", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body = nlohmann::json::parse(req.body, nullptr, false);
        if (!body.is_object()) {
            manager_reply(res, {{"error", "invalid request"}}, 400);
            return;
        }
        if (body.contains("hold") && body["hold"].is_boolean()) manager_set_hold(body["hold"].get<bool>());
        if (body.contains("auto_lua") || body.contains("auto_lua_path") || body.contains("hold_timeout")) {
            bool auto_lua = body.contains("auto_lua") && body["auto_lua"].is_boolean() ? body["auto_lua"].get<bool>() : g_mgr.auto_lua.load();
            std::string path = body.contains("auto_lua_path") && body["auto_lua_path"].is_string() ? body["auto_lua_path"].get<std::string>() : g_mgr.auto_lua_path;
            int timeout = body.contains("hold_timeout") && body["hold_timeout"].is_number_integer() ? body["hold_timeout"].get<int>() : g_mgr.hold_timeout.load();
            manager_set_options(auto_lua, path, timeout);
        }
        nlohmann::json j;
        j["ok"] = true;
        j["hold"] = g_mgr.hold.load();
        j["auto_lua"] = g_mgr.auto_lua.load();
        {
            std::lock_guard<std::mutex> lk(g_mgr.cfg_mtx);
            j["auto_lua_path"] = g_mgr.auto_lua_path;
        }
        j["hold_timeout"] = g_mgr.hold_timeout.load();
        manager_reply(res, j);
    });

    svr.Post("/api/manager/resume", [](const httplib::Request&, httplib::Response& res) {
        manager_set_hold(false);
        nlohmann::json j;
        j["ok"] = true;
        j["was_holding"] = g_mgr.holding.load();
        manager_reply(res, j);
    });

    svr.Post("/api/manager/run_auto_lua", [](const httplib::Request&, httplib::Response& res) {
        if (g_mgr.holding.load()) {
            manager_reply(res, {{"error", "game is frozen; resume first"}}, 409);
            return;
        }
        std::string out;
        std::string path;
        {
            std::lock_guard<std::mutex> lk(g_mgr.cfg_mtx);
            path = g_mgr.auto_lua_path.empty() ? "main.lua" : g_mgr.auto_lua_path;
            out = execute_lua_workspace(path);
            g_mgr.last_output = out;
        }
        nlohmann::json j;
        j["ok"] = true;
        j["path"] = path;
        j["output"] = out;
        manager_reply(res, j);
    });
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
