#pragma once
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>
#include "lua_values.hpp"
#include "symbols.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

enum reflect_kind : int {
    reflect_class = 1,
    reflect_method = 2,
    reflect_field = 4,
    reflect_property = 8,
    reflect_all = 15
};

struct reflect_hit {
    std::string kind;
    std::string asm_name;
    std::string ns;
    std::string cls;
    std::string name;
    std::string type;
    std::string params;
    std::string decl;
    bool is_static = false;
    uintptr_t address = 0;
    Il2CppClass* klass = nullptr;
};

struct reflect_result {
    std::vector<reflect_hit> hits;
    size_t scanned = 0;
    bool truncated = false;
};

static std::string reflect_lower(std::string text) {
    for (auto& c : text) c = (char)tolower((unsigned char)c);
    return text;
}

static bool reflect_contains(const char* haystack, const std::string& needle_lower) {
    if (needle_lower.empty()) return true;
    if (!haystack) return false;
    const size_t n = needle_lower.size();
    for (const char* p = haystack; *p; ++p) {
        size_t i = 0;
        while (i < n && p[i] && tolower((unsigned char)p[i]) == (unsigned char)needle_lower[i]) ++i;
        if (i == n) return true;
    }
    return false;
}

static std::vector<std::string> reflect_split_path(const std::string& text) {
    std::vector<std::string> parts;
    size_t start = 0;
    while (start <= text.size()) {
        size_t next = text.find('/', start);
        if (next == std::string::npos) next = text.size();
        parts.push_back(reflect_lower(text.substr(start, next - start)));
        start = next + 1;
    }
    while (!parts.empty() && parts.back().empty()) parts.pop_back();
    return parts;
}

static bool reflect_path_match(const char* asm_name, const char* ns, const char* cls, const std::vector<std::string>& p) {
    switch (p.size()) {
        case 0: return true;
        case 1: return reflect_contains(asm_name, p[0]) || reflect_contains(ns, p[0]) || reflect_contains(cls, p[0]);
        case 2: return reflect_contains(asm_name, p[0]) && (reflect_contains(ns, p[1]) || reflect_contains(cls, p[1]));
        default: return reflect_contains(asm_name, p[0]) && reflect_contains(ns, p[1]) && reflect_contains(cls, p[2]);
    }
}

static std::string reflect_type_name(const Il2CppType* type) {
    if (!type || !symbol_plausible(type)) return "?";
    std::string full = type_full_name(type);
    static const char* aliases[][2] = {
        {"System.Void", "void"}, {"System.Int32", "int"}, {"System.UInt32", "uint"}, {"System.Int64", "long"},
        {"System.UInt64", "ulong"}, {"System.Int16", "short"}, {"System.UInt16", "ushort"}, {"System.Byte", "byte"},
        {"System.SByte", "sbyte"}, {"System.Single", "float"}, {"System.Double", "double"}, {"System.Boolean", "bool"},
        {"System.String", "string"}, {"System.Object", "object"}, {"System.Char", "char"}
    };
    for (auto& a : aliases)
        if (full == a[0]) return a[1];
    return full;
}

static std::string reflect_param_name(const MethodInfo* mi, int index) {
    auto& methods = BNM::Internal::il2cppMethods;
    if (methods.il2cpp_method_get_param_name) {
        const char* name = methods.il2cpp_method_get_param_name(mi, (uint32_t)index);
        if (name && symbol_plausible(name) && name[0]) return name;
    }
    return "arg" + std::to_string(index);
}

static std::string reflect_method_params(const MethodInfo* mi) {
    std::string out;
    for (int i = 0; i < (int)mi->parameters_count; ++i) {
        if (i) out += ", ";
        out += reflect_type_name(bnm_method_parameter_type(mi, static_cast<uint32_t>(i))) + " " + reflect_param_name(mi, i);
    }
    return out;
}

static const char* reflect_class_kind(Il2CppClass* k) {
    if (k->enumtype) return "enum";
    if (bnm_class_is_value_type(k)) return "struct";
    if (k->flags & 0x20) return "interface";
    return "class";
}

static const char* reflect_ns(Il2CppClass* k) {
    return symbol_plausible(k->namespaze) ? k->namespaze : "";
}

static std::string reflect_full_name(Il2CppClass* k) {
    if (!k || !symbol_plausible(k) || !symbol_plausible(k->name)) return "?";
    std::string ns = reflect_ns(k);
    return ns.empty() ? k->name : ns + "." + k->name;
}

static const char* reflect_class_asm(Il2CppClass* k) {
    auto* image = k ? k->image : nullptr;
    return (image && symbol_plausible(image) && symbol_plausible(image->name)) ? image->name : "";
}

static reflect_hit reflect_make_hit(Il2CppClass* k, const char* kind, const char* name) {
    reflect_hit hit;
    hit.kind = kind;
    hit.asm_name = reflect_class_asm(k);
    hit.ns = reflect_ns(k);
    hit.cls = k->name;
    hit.name = name;
    hit.klass = k;
    return hit;
}

static reflect_hit reflect_method_hit(Il2CppClass* k, const MethodInfo* mi) {
    reflect_hit hit = reflect_make_hit(k, "method", mi->name);
    hit.type = (mi->return_type && symbol_plausible(mi->return_type)) ? reflect_type_name(mi->return_type) : "?";
    hit.params = reflect_method_params(mi);
    hit.is_static = (mi->flags & 0x10) != 0;
    hit.address = mi->methodPointer ? (uintptr_t)mi->methodPointer & 0x00FFFFFFFFFFFFFFULL : 0;
    return hit;
}

static reflect_hit reflect_field_hit(Il2CppClass* k, FieldInfo* fi) {
    reflect_hit hit = reflect_make_hit(k, "field", fi->name);
    hit.type = (fi->type && symbol_plausible(fi->type)) ? reflect_type_name(fi->type) : "?";
    hit.is_static = fi->type && symbol_plausible(fi->type) && (fi->type->attrs & 0x10) != 0;
    return hit;
}

static reflect_hit reflect_property_hit(Il2CppClass* k, const PropertyInfo* pi) {
    reflect_hit hit = reflect_make_hit(k, "property", pi->name);
    const MethodInfo* getter = pi->get && symbol_plausible(pi->get) ? pi->get : nullptr;
    const MethodInfo* setter = pi->set && symbol_plausible(pi->set) ? pi->set : nullptr;
    if (getter && getter->return_type) hit.type = reflect_type_name(getter->return_type);
    else if (setter && setter->parameters_count) hit.type = reflect_type_name(bnm_method_parameter_type(setter, 0));
    else hit.type = "?";
    hit.params = std::string(getter ? "get" : "") + (getter && setter ? ";" : "") + (setter ? "set" : "");
    hit.is_static = (getter && (getter->flags & 0x10)) || (setter && (setter->flags & 0x10));
    return hit;
}

struct reflect_query {
    std::string text;
    std::string path;
    int kinds = reflect_all;
    size_t limit = 400;
};

static std::mutex g_reflect_mutex;
static std::vector<Il2CppClass*> g_reflect_classes;
static size_t g_reflect_image_count = 0;

static std::vector<Il2CppClass*> reflect_classes() {
    BNM::AttachIl2Cpp();
    std::lock_guard<std::mutex> lock(g_reflect_mutex);
    auto images = BNM::Image::GetImages();
    if (!g_reflect_classes.empty() && images.size() == g_reflect_image_count) return g_reflect_classes;
    std::vector<Il2CppClass*> out;
    out.reserve(g_reflect_classes.size() + 1024);
    for (auto& img : images) {
        if (!img.IsValid()) continue;
        for (auto& cls : img.GetClasses(true)) {
            Il2CppClass* k = cls.IsValid() ? cls.GetClass() : nullptr;
            if (k && symbol_plausible(k) && symbol_plausible(k->name) && k->name[0] != '<') out.push_back(k);
        }
    }
    g_reflect_classes = std::move(out);
    g_reflect_image_count = images.size();
    return g_reflect_classes;
}

static reflect_result reflect_search(const reflect_query& query) {
    reflect_result result;
    std::string member_part = reflect_lower(query.text);
    std::string class_part;
    size_t split_at = member_part.find("::");
    size_t skip = 2;
    if (split_at == std::string::npos) {
        split_at = member_part.rfind('.');
        skip = 1;
    }
    if (split_at != std::string::npos && split_at > 0 && split_at + skip < member_part.size()) {
        class_part = member_part.substr(0, split_at);
        member_part = member_part.substr(split_at + skip);
    }
    const bool qualified = !class_part.empty();
    const auto path_parts = reflect_split_path(query.path);

    auto push = [&](reflect_hit&& hit) {
        if (result.hits.size() >= query.limit) {
            result.truncated = true;
            return false;
        }
        result.hits.push_back(std::move(hit));
        return true;
    };

    for (Il2CppClass* k : reflect_classes()) {
        if (!reflect_path_match(reflect_class_asm(k), reflect_ns(k), k->name, path_parts)) continue;
        if (qualified && !reflect_contains(k->name, class_part)) continue;
        ++result.scanned;

        if (!qualified && (query.kinds & reflect_class) && reflect_contains(k->name, member_part)) {
            reflect_hit hit = reflect_make_hit(k, "class", k->name);
            hit.type = reflect_class_kind(k);
            if (!push(std::move(hit))) return result;
        }
        if (member_part.empty()) continue;

        if ((query.kinds & reflect_method) && k->methods) {
            for (uint16_t i = 0; i < k->method_count; ++i) {
                const MethodInfo* mi = k->methods[i];
                if (!mi || !symbol_plausible(mi) || !symbol_plausible(mi->name) || !reflect_contains(mi->name, member_part)) continue;
                if (!push(reflect_method_hit(k, mi))) return result;
            }
        }
        if ((query.kinds & reflect_field) && k->fields) {
            for (uint16_t i = 0; i < k->field_count; ++i) {
                FieldInfo* fi = &k->fields[i];
                if (!symbol_plausible(fi->name) || !reflect_contains(fi->name, member_part)) continue;
                if (!push(reflect_field_hit(k, fi))) return result;
            }
        }
        if ((query.kinds & reflect_property) && k->properties) {
            const PropertyInfo* properties = (const PropertyInfo*)k->properties;
            for (uint16_t i = 0; i < k->property_count; ++i) {
                const PropertyInfo* pi = &properties[i];
                if (!symbol_plausible(pi->name) || !reflect_contains(pi->name, member_part)) continue;
                if (!push(reflect_property_hit(k, pi))) return result;
            }
        }
    }
    return result;
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
