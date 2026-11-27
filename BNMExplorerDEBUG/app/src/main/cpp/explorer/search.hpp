#pragma once
#include "reflect.hpp"
#include "json.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

static int search_parse_kinds(const std::string& text) {
    if (text.empty()) return reflect_all;
    int kinds = 0;
    if (text.find("class") != std::string::npos) kinds |= reflect_class;
    if (text.find("method") != std::string::npos) kinds |= reflect_method;
    if (text.find("field") != std::string::npos) kinds |= reflect_field;
    if (text.find("property") != std::string::npos) kinds |= reflect_property;
    return kinds ? kinds : reflect_all;
}

static nlohmann::json search_hit_json(const reflect_hit& hit) {
    nlohmann::json item;
    item["k"] = hit.kind;
    item["a"] = hit.asm_name;
    item["ns"] = hit.ns;
    item["c"] = hit.cls;
    item["n"] = hit.name;
    item["t"] = hit.type;
    if (!hit.params.empty()) item["p"] = hit.params;
    if (hit.is_static) item["s"] = true;
    if (hit.address) {
        char buf[24];
        snprintf(buf, sizeof(buf), "%llX", (unsigned long long)hit.address);
        item["addr"] = buf;
    }
    return item;
}

static void register_search_routes(httplib::Server& svr) {
    svr.Get("/api/search", [](const httplib::Request& req, httplib::Response& res) {
        reflect_query query;
        query.text = req.get_param_value("q");
        query.path = req.get_param_value("path");
        query.kinds = search_parse_kinds(req.get_param_value("kinds"));
        int limit = req.has_param("limit") ? atoi(req.get_param_value("limit").c_str()) : 400;
        query.limit = (size_t)std::max(1, std::min(2000, limit));
        reflect_result result = reflect_search(query);
        nlohmann::json out;
        out["r"] = nlohmann::json::array();
        for (const auto& hit : result.hits) out["r"].push_back(search_hit_json(hit));
        out["truncated"] = result.truncated;
        out["scanned"] = result.scanned;
        res.set_header("Cache-Control", "no-store");
        res.set_content(out.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace), "application/json");
    });
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
