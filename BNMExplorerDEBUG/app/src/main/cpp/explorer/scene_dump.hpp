#pragma once
#include <algorithm>
#include <cctype>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <string>
#include <vector>
#include <unordered_map>
#include "json.hpp"

struct scene_dump_state {
    std::mutex mtx;
    bool running = false;
    bool cancel = false;
    bool done = false;
    int total = 0;
    int processed = 0;
    std::string text;
    std::string error;
};

static scene_dump_state g_dump;
static const size_t dump_max_bytes = 48 * 1024 * 1024;

static std::string dump_clean_line(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '\n') out += "\\n";
        else if (c == '\r') continue;
        else out += c;
    }
    return out;
}

static std::string dump_format_value(const nlohmann::json& v) {
    if (v.is_null()) return "null";
    if (v.is_boolean()) return v.get<bool>() ? "true" : "false";
    if (v.is_number_integer() || v.is_number_unsigned()) return v.dump();
    if (v.is_number_float()) {
        char buf[48];
        snprintf(buf, sizeof(buf), "%.6g", v.get<double>());
        return buf;
    }
    if (v.is_string()) {
        std::string s = dump_clean_line(v.get<std::string>());
        return s.empty() ? "\"\"" : s;
    }
    if (v.is_array()) {
        std::string out = "(";
        bool first = true;
        for (const auto& item : v) {
            if (!first) out += ", ";
            first = false;
            out += dump_format_value(item);
        }
        return out + ")";
    }
    if (v.is_object()) {
        if (v.contains("x") && v.contains("y") && v.contains("z") && v.contains("w") && v.size() == 4)
            return "(" + dump_format_value(v["x"]) + ", " + dump_format_value(v["y"]) + ", " + dump_format_value(v["z"]) + ", " + dump_format_value(v["w"]) + ")";
        std::string out;
        bool first = true;
        for (auto it = v.begin(); it != v.end(); ++it) {
            if (!first) out += " ";
            first = false;
            out += it.key() + "=" + dump_format_value(it.value());
        }
        return out;
    }
    return v.dump();
}

static void dump_write_object(std::string& out, const nlohmann::json& info, const std::string& name, bool active, int depth) {
    std::string indent((size_t)depth * 2, ' ');
    out += indent + "[--" + dump_clean_line(name) + (active ? "" : " (inactive)") + "\n";
    if (info.contains("transform") && info["transform"].is_object() && info["transform"].contains("p")) {
        const auto& tr = info["transform"];
        out += indent + "[-Transform\n";
        out += indent + "localPosition # " + dump_format_value(tr["p"]) + "\n";
        if (tr.contains("r")) out += indent + "localEulerAngles # " + dump_format_value(tr["r"]) + "\n";
        if (tr.contains("s")) out += indent + "localScale # " + dump_format_value(tr["s"]) + "\n";
    }
    if (!info.contains("scripts") || !info["scripts"].is_array()) return;
    for (const auto& sc : info["scripts"]) {
        std::string comp = sc.contains("name") && sc["name"].is_string() ? sc["name"].get<std::string>() : "Unknown";
        bool enabled = !sc.contains("enabled") || !sc["enabled"].is_boolean() || sc["enabled"].get<bool>();
        out += indent + "[-" + dump_clean_line(comp) + (enabled ? "" : " (disabled)") + "\n";
        if (!sc.contains("fields") || !sc["fields"].is_array()) continue;
        for (const auto& f : sc["fields"]) {
            if (!f.contains("name") || !f["name"].is_string()) continue;
            std::string value = f.contains("val") ? dump_format_value(f["val"]) : "";
            out += indent + f["name"].get<std::string>() + " # " + value + "\n";
        }
    }
}

static std::string dump_lower(std::string s) {
    for (auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

struct dump_frame {
    int node;
    int depth;
};

struct dump_job {
    bool inited = false;
    int n = 0;
    int processed = 0;
    std::vector<std::string> names;
    std::vector<std::string> addrs;
    std::vector<bool> actives;
    std::vector<std::vector<int>> children;
    std::vector<bool> visited;
    std::vector<dump_frame> stack;
    std::string out;
    std::string error;
};

static void dump_finish(const std::shared_ptr<dump_job>& job) {
    std::lock_guard<std::mutex> lk(g_dump.mtx);
    g_dump.processed = job->processed;
    g_dump.text = std::move(job->out);
    g_dump.error = job->error;
    g_dump.done = true;
    g_dump.running = false;
}

static bool dump_init(const std::shared_ptr<dump_job>& job) {
    nlohmann::json list = nlohmann::json::parse(scene_json(), nullptr, false);
    if (!list.is_array()) {
        job->error = "could not read the scene";
        return false;
    }
    int n = (int)list.size();
    job->n = n;
    {
        std::lock_guard<std::mutex> lk(g_dump.mtx);
        g_dump.total = n;
    }
    std::unordered_map<std::string, int> index_of;
    std::vector<std::string> parents((size_t)n);
    job->names.resize((size_t)n);
    job->addrs.resize((size_t)n);
    job->actives.assign((size_t)n, true);
    for (int i = 0; i < n; i++) {
        const auto& g = list[(size_t)i];
        job->addrs[(size_t)i] = g.value("addr", "");
        job->names[(size_t)i] = g.value("name", "");
        job->actives[(size_t)i] = g.value("active", true);
        parents[(size_t)i] = g.value("parent", "");
        index_of[job->addrs[(size_t)i]] = i;
    }
    job->children.assign((size_t)n, {});
    std::vector<int> roots;
    for (int i = 0; i < n; i++) {
        auto it = index_of.find(parents[(size_t)i]);
        if (parents[(size_t)i].empty() || parents[(size_t)i] == "0" || it == index_of.end() || it->second == i) roots.push_back(i);
        else job->children[(size_t)it->second].push_back(i);
    }
    auto by_name = [&](int a, int b) { return dump_lower(job->names[(size_t)a]) < dump_lower(job->names[(size_t)b]); };
    std::stable_sort(roots.begin(), roots.end(), by_name);
    for (auto& c : job->children) std::stable_sort(c.begin(), c.end(), by_name);
    job->visited.assign((size_t)n, false);
    for (auto it = roots.rbegin(); it != roots.rend(); ++it) job->stack.push_back({*it, 0});
    job->inited = true;
    return true;
}

static bool dump_step(const std::shared_ptr<dump_job>& job) {
    try {
        if (!job->inited) {
            if (!dump_init(job)) {
                dump_finish(job);
                return true;
            }
            return false;
        }
        auto t0 = std::chrono::steady_clock::now();
        bool stop = false;
        while (!job->stack.empty() && !stop) {
            dump_frame f = job->stack.back();
            job->stack.pop_back();
            if (job->visited[(size_t)f.node] || f.depth > 120) continue;
            job->visited[(size_t)f.node] = true;
            {
                std::lock_guard<std::mutex> lk(g_dump.mtx);
                if (g_dump.cancel) {
                    job->error = "cancelled";
                    stop = true;
                    break;
                }
            }
            uintptr_t addr = (uintptr_t)strtoull(job->addrs[(size_t)f.node].c_str(), nullptr, 16);
            nlohmann::json info = nlohmann::json::parse(go_info_json(addr), nullptr, false);
            if (info.is_object() && !info.value("stale", false))
                dump_write_object(job->out, info, job->names[(size_t)f.node], job->actives[(size_t)f.node], f.depth);
            job->processed++;
            if ((job->processed & 3) == 0) {
                std::lock_guard<std::mutex> lk(g_dump.mtx);
                g_dump.processed = job->processed;
            }
            if (job->out.size() > dump_max_bytes) {
                job->out += "[output truncated]\n";
                job->error = "output truncated at 48 MB";
                stop = true;
                break;
            }
            const auto& kids = job->children[(size_t)f.node];
            for (auto it = kids.rbegin(); it != kids.rend(); ++it) job->stack.push_back({*it, f.depth + 1});
            if (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - t0).count() > 2000) break;
        }
        if (stop || job->stack.empty()) {
            dump_finish(job);
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        job->error = std::string("dump failed: ") + e.what();
    } catch (...) {
        job->error = "dump failed";
    }
    dump_finish(job);
    return true;
}

static void register_dump_routes(httplib::Server& svr) {
    svr.Post("/api/scene/dump/start", [](const httplib::Request&, httplib::Response& res) {
        nlohmann::json j;
        {
            std::lock_guard<std::mutex> lk(g_dump.mtx);
            if (g_dump.running) {
                j["error"] = "a dump is already running";
                res.status = 409;
                res.set_content(j.dump(), "application/json");
                return;
            }
            g_dump.running = true;
            g_dump.cancel = false;
            g_dump.done = false;
            g_dump.total = 0;
            g_dump.processed = 0;
            g_dump.text.clear();
            g_dump.error.clear();
        }
        auto job = std::make_shared<dump_job>();
        mt_post([job]() { return dump_step(job); }, false);
        j["ok"] = true;
        res.set_content(j.dump(), "application/json");
    });

    svr.Post("/api/scene/dump/cancel", [](const httplib::Request&, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lk(g_dump.mtx);
            g_dump.cancel = true;
        }
        res.set_content("{\"ok\":true}", "application/json");
    });

    svr.Get("/api/scene/dump/status", [](const httplib::Request&, httplib::Response& res) {
        nlohmann::json j;
        std::lock_guard<std::mutex> lk(g_dump.mtx);
        j["running"] = g_dump.running;
        j["done"] = g_dump.done;
        j["total"] = g_dump.total;
        j["processed"] = g_dump.processed;
        j["error"] = g_dump.error;
        j["size"] = g_dump.text.size();
        res.set_content(j.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace), "application/json");
    });

    svr.Get("/api/scene/dump/result", [](const httplib::Request& req, httplib::Response& res) {
        std::string text;
        {
            std::lock_guard<std::mutex> lk(g_dump.mtx);
            text = g_dump.text;
        }
        if (req.get_param_value("download") == "1")
            res.set_header("Content-Disposition", "attachment; filename=\"scene_dump.txt\"");
        res.set_content(text, "text/plain; charset=utf-8");
    });
}
