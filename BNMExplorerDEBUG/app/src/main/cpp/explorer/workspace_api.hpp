#pragma once
#include <cctype>
#include "json.hpp"
#include "workspace.hpp"
#include "luabnm.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

static void ws_reply(httplib::Response& res, const nlohmann::json& j, int status = 200) {
    res.status = status;
    res.set_content(j.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace), "application/json");
}

static void ws_reply_error(httplib::Response& res, const std::string& message, int status = 400) {
    nlohmann::json j;
    j["error"] = message;
    ws_reply(res, j, status);
}

static bool ws_parse_body(const httplib::Request& req, nlohmann::json& out) {
    out = nlohmann::json::parse(req.body, nullptr, false);
    return out.is_object();
}

static std::string ws_json_string(const nlohmann::json& j, const char* key) {
    auto it = j.find(key);
    if (it == j.end() || !it->is_string()) return "";
    return it->get<std::string>();
}

static void ws_reply_import(httplib::Response& res, const std::string& data) {
    std::vector<ws_file_data> files;
    int skipped = 0;
    std::string error;
    if (!ws_parse_zip(data, files, skipped, error)) {
        ws_reply_error(res, error);
        return;
    }
    if (!ws_replace_with(files, error)) {
        ws_reply_error(res, error);
        return;
    }
    nlohmann::json j;
    j["ok"] = true;
    j["imported"] = files.size();
    j["skipped"] = skipped;
    ws_reply(res, j);
}

static void register_workspace_routes(httplib::Server& svr) {
    ws_init();
    svr.set_payload_max_length(64 * 1024 * 1024);

    svr.Get("/api/ws/info", [](const httplib::Request&, httplib::Response& res) {
        nlohmann::json j;
        j["base"] = g_ws_base;
        j["workspace"] = ws_workspace_dir();
        j["zips"] = ws_zips_dir();
        j["zips_global"] = g_ws_zips_global;
        j["entry"] = "main.lua";
        ws_reply(res, j);
    });

    svr.Get("/api/ws/tree", [](const httplib::Request&, httplib::Response& res) {
        nlohmann::json list = nlohmann::json::array();
        for (const auto& e : ws_list()) {
            nlohmann::json item;
            item["path"] = e.path;
            item["dir"] = e.is_dir;
            item["lua"] = e.lua;
            item["size"] = e.size;
            list.push_back(item);
        }
        nlohmann::json j;
        j["entries"] = list;
        ws_reply(res, j);
    });

    svr.Get("/api/ws/file", [](const httplib::Request& req, httplib::Response& res) {
        std::string path = ws_sanitize_path(req.get_param_value("path"), false);
        if (path.empty()) {
            ws_reply_error(res, "invalid file path");
            return;
        }
        std::string content;
        if (!ws_read_file(path, content)) {
            ws_reply_error(res, "file not found", 404);
            return;
        }
        nlohmann::json j;
        j["path"] = path;
        j["content"] = content;
        ws_reply(res, j);
    });

    svr.Post("/api/ws/file", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body;
        if (!ws_parse_body(req, body)) {
            ws_reply_error(res, "invalid request");
            return;
        }
        std::string path = ws_sanitize_path(ws_json_string(body, "path"), false);
        if (path.empty()) {
            ws_reply_error(res, "invalid file name: use letters, numbers, . _ -");
            return;
        }
        if (!ws_write_file(path, ws_json_string(body, "content"))) {
            ws_reply_error(res, "failed to write file (limit is 1 MB)");
            return;
        }
        nlohmann::json j;
        j["ok"] = true;
        j["path"] = path;
        ws_reply(res, j);
    });

    svr.Get("/api/ws/raw", [](const httplib::Request& req, httplib::Response& res) {
        std::string path = ws_sanitize_path(req.get_param_value("path"), false);
        if (path.empty()) {
            ws_reply_error(res, "invalid file path");
            return;
        }
        std::string content;
        if (!ws_read_file(path, content)) {
            ws_reply_error(res, "file not found", 404);
            return;
        }
        std::string name = path;
        size_t slash = name.rfind('/');
        if (slash != std::string::npos) name = name.substr(slash + 1);
        std::string ext;
        size_t dot = name.rfind('.');
        if (dot != std::string::npos) {
            for (size_t i = dot + 1; i < name.size(); i++) ext += (char)tolower((unsigned char)name[i]);
        }
        const char* type = "application/octet-stream";
        if (ext == "png") type = "image/png";
        else if (ext == "jpg" || ext == "jpeg") type = "image/jpeg";
        else if (ext == "gif") type = "image/gif";
        else if (ext == "webp") type = "image/webp";
        else if (ext == "bmp") type = "image/bmp";
        else if (ext == "svg") type = "image/svg+xml";
        else if (ext == "wav") type = "audio/wav";
        else if (ext == "ogg" || ext == "oga") type = "audio/ogg";
        else if (ext == "mp3") type = "audio/mpeg";
        else if (ext == "m4a" || ext == "aac") type = "audio/mp4";
        else if (ext == "flac") type = "audio/flac";
        else if (ext == "mp4" || ext == "webm") type = "video/mp4";
        else if (ext == "txt" || ext == "log" || ext == "md" || ext == "ini" || ext == "cfg" || ext == "conf" || ext == "csv") type = "text/plain";
        else if (ext == "json") type = "application/json";
        else if (ext == "xml" || ext == "yaml" || ext == "yml") type = "text/plain";
        else if (ext == "html") type = "text/html";
        else if (ext == "css") type = "text/css";
        else if (ext == "js") type = "text/javascript";
        else if (ext == "lua") type = "text/plain";
        bool inline_view = req.has_param("view") && req.get_param_value("view") == "1";
        res.set_header("Content-Disposition", std::string(inline_view ? "inline" : "attachment") + "; filename=\"" + name + "\"");
        res.set_content(content, type);
    });

    svr.Post("/api/ws/upload", [](const httplib::Request& req, httplib::Response& res) {
        std::string path = ws_sanitize_path(req.get_param_value("path"), false);
        if (path.empty()) {
            ws_reply_error(res, "invalid file path");
            return;
        }
        if (!ws_write_file(path, req.body)) {
            ws_reply_error(res, "failed to write file (limit is 1 MB)");
            return;
        }
        nlohmann::json j;
        j["ok"] = true;
        j["path"] = path;
        ws_reply(res, j);
    });

    svr.Post("/api/ws/mkdir", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body;
        if (!ws_parse_body(req, body)) {
            ws_reply_error(res, "invalid request");
            return;
        }
        std::string path = ws_sanitize_path(ws_json_string(body, "path"), false);
        if (path.empty() || !ws_make_folder(path)) {
            ws_reply_error(res, "invalid folder name");
            return;
        }
        nlohmann::json j;
        j["ok"] = true;
        j["path"] = path;
        ws_reply(res, j);
    });

    svr.Post("/api/ws/delete", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body;
        if (!ws_parse_body(req, body)) {
            ws_reply_error(res, "invalid request");
            return;
        }
        std::string path = ws_sanitize_path(ws_json_string(body, "path"), false);
        if (path.empty() || !ws_delete(path)) {
            ws_reply_error(res, "failed to delete");
            return;
        }
        nlohmann::json j;
        j["ok"] = true;
        ws_reply(res, j);
    });

    svr.Post("/api/ws/rename", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body;
        if (!ws_parse_body(req, body)) {
            ws_reply_error(res, "invalid request");
            return;
        }
        std::string from = ws_sanitize_path(ws_json_string(body, "from"), false);
        if (from.empty()) {
            ws_reply_error(res, "invalid source path");
            return;
        }
        bool is_file = ws_is_file(ws_workspace_dir() + "/" + from);
        std::string to = ws_sanitize_path(ws_json_string(body, "to"), is_file);
        if (to.empty()) {
            ws_reply_error(res, "invalid target name");
            return;
        }
        if (!ws_rename(from, to)) {
            ws_reply_error(res, "rename failed: target already exists or source is missing");
            return;
        }
        nlohmann::json j;
        j["ok"] = true;
        j["path"] = to;
        ws_reply(res, j);
    });

    svr.Post("/api/ws/run", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body;
        std::string path = "main.lua";
        bool main_thread = true;
        if (ws_parse_body(req, body)) {
            std::string requested = ws_json_string(body, "path");
            if (!requested.empty()) path = requested;
            if (body.contains("main") && body["main"].is_boolean()) main_thread = body["main"].get<bool>();
        }
        nlohmann::json j;
        j["output"] = execute_lua_workspace(path, main_thread);
        ws_reply(res, j);
    });

    svr.Post("/api/ws/import", [](const httplib::Request& req, httplib::Response& res) {
        ws_reply_import(res, req.body);
    });

    svr.Get("/api/ws/export", [](const httplib::Request&, httplib::Response& res) {
        std::string data;
        ws_build_zip(data);
        res.set_header("Content-Disposition", "attachment; filename=\"workspace.zip\"");
        res.set_content(data, "application/zip");
    });

    svr.Get("/api/ws/zips", [](const httplib::Request&, httplib::Response& res) {
        nlohmann::json list = nlohmann::json::array();
        for (const auto& z : ws_list_zips()) {
            nlohmann::json item;
            item["name"] = z.name;
            item["size"] = z.size;
            item["mtime"] = z.mtime;
            list.push_back(item);
        }
        nlohmann::json j;
        j["zips"] = list;
        ws_reply(res, j);
    });

    svr.Post("/api/ws/save_zip", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body;
        if (!ws_parse_body(req, body)) {
            ws_reply_error(res, "invalid request");
            return;
        }
        std::string name = ws_sanitize_zip_name(ws_json_string(body, "name"));
        if (name.empty()) {
            ws_reply_error(res, "invalid zip name: use letters, numbers, . _ - ( ) and spaces");
            return;
        }
        std::string error;
        if (!ws_save_zip(name, error)) {
            ws_reply_error(res, error, 500);
            return;
        }
        nlohmann::json j;
        j["ok"] = true;
        j["name"] = name;
        ws_reply(res, j);
    });

    svr.Post("/api/ws/load_zip", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body;
        if (!ws_parse_body(req, body)) {
            ws_reply_error(res, "invalid request");
            return;
        }
        std::string name = ws_sanitize_zip_name(ws_json_string(body, "name"));
        std::string data;
        if (name.empty() || !ws_read_zip(name, data)) {
            ws_reply_error(res, "zip not found on device", 404);
            return;
        }
        ws_reply_import(res, data);
    });

    svr.Post("/api/ws/delete_zip", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json body;
        if (!ws_parse_body(req, body)) {
            ws_reply_error(res, "invalid request");
            return;
        }
        std::string name = ws_sanitize_zip_name(ws_json_string(body, "name"));
        if (name.empty() || !ws_delete_zip(name)) {
            ws_reply_error(res, "failed to delete zip");
            return;
        }
        nlohmann::json j;
        j["ok"] = true;
        ws_reply(res, j);
    });

    svr.Get("/api/ws/zip", [](const httplib::Request& req, httplib::Response& res) {
        std::string name = ws_sanitize_zip_name(req.get_param_value("name"));
        std::string data;
        if (name.empty() || !ws_read_zip(name, data)) {
            ws_reply_error(res, "zip not found on device", 404);
            return;
        }
        res.set_header("Content-Disposition", "attachment; filename=\"" + name + "\"");
        res.set_content(data, "application/zip");
    });
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
