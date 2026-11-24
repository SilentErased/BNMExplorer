#include <jni.h>
#include <thread>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <initializer_list>
#include <deque>
#include <memory>
#include <shared_mutex>
#include <cstddef>
#include <cctype>
#include <array>
#include <cstdio>
#include <cstdint>
#include <utility>
#include <sys/syscall.h>
#include <dobby.h>
#include <android/log.h>
#include "Include/BNMResolve.hpp"
#include "Include/autodetect/il2cpp_runtime.hpp"
#include "autodetect/variant_namespace.hpp"
#include "extern/BNM-Android/src/private/Internals.hpp"
#include "Include/httplib.h"
#include "explorer/html_page.hpp"
#include "explorer/luabnm.hpp"
#include "explorer/workspace_api.hpp"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "BNMExplorer", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "BNMExplorer", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "BNMExplorer", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "BNMExplorer", __VA_ARGS__)

namespace BNM_APP_NAMESPACE {

    using namespace BNM;
    using namespace BNM::IL2CPP;
    using namespace BNM::Structures::Unity;

    struct vector3_int { int32_t x; int32_t y; int32_t z; };
    struct vector2_int { int32_t x; int32_t y; };

    static int g_port = 8080;

    static double now_seconds() {
        return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    static void attach_thread() {
        BNM::AttachIl2Cpp();
    }

    static inline bool is_valid_pointer(const void* ptr) {
        uintptr_t p = (uintptr_t)ptr;
        if (p < 0x10000) return false;
        if (sizeof(void*) == 8) return (p & 0x00FFFFFFFFFFFFFFULL) < 0x0001000000000000ULL;
        return true;
    }

    static int g_probe_fds[2] = {-1, -1};
    static std::mutex g_probe_mtx;

    static bool is_readable(const void* ptr, size_t size) {
        if (!is_valid_pointer(ptr) || size == 0 || size > 4096) return false;
        std::lock_guard<std::mutex> lk(g_probe_mtx);
        if (g_probe_fds[0] < 0) {
            if (pipe2(g_probe_fds, O_CLOEXEC | O_NONBLOCK) != 0) {
                g_probe_fds[0] = -1;
                g_probe_fds[1] = -1;
                return true;
            }
        }
        ssize_t w = write(g_probe_fds[1], ptr, size);
        if (w < 0) return false;
        char sink[256];
        ssize_t left = w;
        while (left > 0) {
            ssize_t r = read(g_probe_fds[0], sink, std::min<size_t>(sizeof(sink), (size_t)left));
            if (r <= 0) break;
            left -= r;
        }
        return (size_t)w == size;
    }

    static std::string js_esc(const char* s) {
        if (!s) return "";
        std::string o;
        for (const char* p = s; *p; ++p) {
            unsigned char c = (unsigned char)*p;
            if      (c == '"')  o += "\\\"";
            else if (c == '\\') o += "\\\\";
            else if (c == '\n') o += "\\n";
            else if (c == '\r') o += "\\r";
            else if (c == '\t') o += "\\t";
            else if (c < 0x20) { char buf[8]; snprintf(buf, sizeof(buf), "\\u%04x", c); o += buf; }
            else                 o += *p;
        }
        return o;
    }

    static std::string js_esc(const std::string& s) { return js_esc(s.c_str()); }

    static std::string type_name(const Il2CppType* t) {
        if (!t || !is_valid_pointer(t)) return "?";
        try {
            Class c(t);
            if (!c.IsValid()) return "?";
            auto* r = c.GetClass();
            if (!r || !is_valid_pointer(r) || !is_valid_pointer(r->name)) return "?";
            std::string n = r->name;
            if (r->namespaze && is_valid_pointer(r->namespaze) && r->namespaze[0] != '\0')
                n = std::string(r->namespaze) + "." + n;
            return n;
        } catch (...) { return "?"; }
    }

    typedef const Il2CppType* (*Il2CppMethodGetParamFn)(const MethodInfo*, uint32_t);
    static Il2CppMethodGetParamFn g_il2cpp_method_get_param = nullptr;
    static bool g_il2cpp_method_get_param_searched = false;

    static const Il2CppType* get_param_type(const MethodInfo* mi, int i) {
        if (!mi || !is_valid_pointer(mi) || i < 0 || i >= (int)mi->parameters_count) return nullptr;
        const Il2CppType* parameter_type = bnm_method_parameter_type(mi, static_cast<uint32_t>(i));
        if (is_valid_pointer(parameter_type)) return parameter_type;
        if (!g_il2cpp_method_get_param_searched) {
            g_il2cpp_method_get_param_searched = true;
            g_il2cpp_method_get_param = (Il2CppMethodGetParamFn)dlsym(RTLD_DEFAULT, "il2cpp_method_get_param");
        }
        if (g_il2cpp_method_get_param) {
            const Il2CppType* t = g_il2cpp_method_get_param(mi, (uint32_t)i);
            if (is_valid_pointer(t)) return t;
        }
        return nullptr;
    }

    static std::string get_param_type_name(const MethodInfo* mi, int i) {
        const Il2CppType* t = get_param_type(mi, i);
        if (!t) return "?";
        return type_name(t);
    }

    static std::string get_param_name(const MethodInfo* mi, int i) {
        if (!mi || !is_valid_pointer(mi)) return "arg" + std::to_string(i);
        try {
            if (BNM::Internal::il2cppMethods.il2cpp_method_get_param_name) {
                const char* n = BNM::Internal::il2cppMethods.il2cpp_method_get_param_name(mi, (uint32_t)i);
                if (n && is_valid_pointer(n) && n[0] != '\0') return n;
            }
        } catch(...) {}
        return "arg" + std::to_string(i);
    }

    struct loop_entry {
        std::string addr;
        std::string asm_;
        std::string ns;
        std::string cls;
        std::string name;
        std::string ftype;
        bool is_prop;
        std::string val;
        int interval_ms;
        std::atomic<bool> active{true};
    };

    static std::mutex g_loop_mtx;
    static std::map<std::string, std::shared_ptr<loop_entry>> g_loops;

    struct log_entry {
        std::string msg;
        int level;
        double timestamp;
    };
    static std::mutex g_log_mtx;
    static std::vector<log_entry> g_log_buffer;
    static const size_t g_log_max = 500;
    static std::atomic<bool> g_log_hooked{false};

    static void add_log_entry(const std::string& msg, int level) {
        std::lock_guard<std::mutex> lk(g_log_mtx);
        if (g_log_buffer.size() >= g_log_max) g_log_buffer.erase(g_log_buffer.begin());
        g_log_buffer.push_back({msg, level, now_seconds()});
    }

    static void hook_debug_log() {
        if (g_log_hooked.exchange(true)) return;
        try {
            Class debug_cls("UnityEngine", "Debug");
            if (!debug_cls.IsValid()) return;
        } catch(...) {}
    }

    struct catcher_arg {
        std::string type;
        std::string val;
    };

    struct catcher_call {
        double ts;
        std::string instance;
        std::vector<catcher_arg> args;
        std::string ret;
    };

    struct catcher_entry {
        std::string id;
        std::string asm_;
        std::string ns;
        std::string cls;
        std::string method;
        std::string ret_type;
        std::vector<std::string> param_types;
        std::vector<std::string> param_names;
        std::atomic<bool> active{true};
        std::mutex calls_mtx;
        std::vector<catcher_call> calls;
        void* orig_ptr = nullptr;
        int slot_idx = -1;
        static const size_t max_calls = 100;
    };

    static std::mutex g_catcher_mtx;
    static std::map<std::string, std::shared_ptr<catcher_entry>> g_catchers;

    static std::string logs_json() {
        std::lock_guard<std::mutex> lk(g_log_mtx);
        std::ostringstream j;
        j << "[";
        bool first = true;
        for (auto& e : g_log_buffer) {
            if (!first) j << ","; first = false;
            j << "{\"msg\":\"" << js_esc(e.msg) << "\",\"level\":" << e.level << ",\"t\":" << std::fixed << e.timestamp << "}";
        }
        j << "]";
        return j.str();
    }

    static std::string json_val(const std::string& json, const std::string& key) {
        std::string qk = "\"" + key + "\":";
        auto pos = json.find(qk);
        if (pos == std::string::npos) return "";
        pos += qk.length();
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\r' || json[pos] == '\n')) pos++;
        if (pos >= json.length()) return "";
        if (json[pos] == '"') {
            pos++;
            size_t end = pos;
            bool esc = false;
            while (end < json.length()) {
                if (json[end] == '\\' && !esc) esc = true;
                else if (json[end] == '"' && !esc) break;
                else esc = false;
                end++;
            }
            if (end >= json.length()) return "";
            std::string raw = json.substr(pos, end - pos);
            std::string out;
            for (size_t i = 0; i < raw.length(); i++) {
                if (raw[i] == '\\' && i + 1 < raw.length()) {
                    char n = raw[i + 1];
                    if      (n == '"')  { out += '"';  i++; }
                    else if (n == '\\') { out += '\\'; i++; }
                    else if (n == 'n')  { out += '\n'; i++; }
                    else if (n == 'r')  { out += '\r'; i++; }
                    else if (n == 't')  { out += '\t'; i++; }
                    else out += raw[i];
                } else out += raw[i];
            }
            return out;
        }
        if (json[pos] == '[' || json[pos] == '{') {
            char open_ch = json[pos];
            char close_ch = (open_ch == '[') ? ']' : '}';
            int depth = 0;
            size_t end = pos;
            bool in_q = false, esc = false;
            while (end < json.length()) {
                char c = json[end];
                if (c == '"' && !esc) in_q = !in_q;
                else if (!in_q) {
                    if (c == open_ch) depth++;
                    else if (c == close_ch) {
                        depth--;
                        if (depth == 0) { end++; break; }
                    }
                }
                if (c == '\\' && !esc) esc = true; else esc = false;
                end++;
            }
            return json.substr(pos, end - pos);
        }
        auto end = json.find_first_of(",}\r\n", pos);
        if (end == std::string::npos) end = json.length();
        std::string v = json.substr(pos, end - pos);
        while (!v.empty() && (v.back() == ' ' || v.back() == '\t' || v.back() == '\r' || v.back() == '\n')) v.pop_back();
        return v;
    }

    static MethodBase find_objs_method() {
        static MethodBase m;
        if (m.IsValid()) return m;
        Class oc("UnityEngine", "Object");
        for (auto& mt : oc.GetMethods(false)) {
            auto* mi = mt.GetInfo();
            if (mi && is_valid_pointer(mi) && mi->name && is_valid_pointer(mi->name) && strcmp(mi->name, "FindObjectsOfType") == 0 && mi->parameters_count == 1) {
                std::string pn = get_param_type_name(mi, 0);
                if (pn.find("Type") != std::string::npos) {
                    m = mt; break;
                }
            }
        }
        return m;
    }

    static bool method_has_pointer(const MethodBase& method) {
        auto* info = method.GetInfo();
        return info && is_valid_pointer(info) && info->methodPointer &&
               is_valid_pointer((const void*)info->methodPointer);
    }

    static MethodBase find_resources_objects_all_method() {
        static MethodBase method;
        if (method.IsValid() && method_has_pointer(method)) return method;
        Class resources_cls("UnityEngine", "Resources");
        if (!resources_cls.IsValid()) return {};
        for (auto& candidate : resources_cls.GetMethods(false)) {
            auto* info = candidate.GetInfo();
            if (!info || !is_valid_pointer(info) || !info->name || !is_valid_pointer(info->name) ||
                strcmp(info->name, "FindObjectsOfTypeAll") != 0 || info->parameters_count != 1 ||
                info->is_generic || !method_has_pointer(candidate)) continue;
            std::string parameter = get_param_type_name(info, 0);
            if (parameter == "System.Type" || parameter == "Type" ||
                parameter.find("System.Type") != std::string::npos) {
                method = candidate;
                break;
            }
        }
        return method;
    }

    static MethodBase find_destroy_object_method() {
        static MethodBase method;
        if (method.IsValid() && method_has_pointer(method)) return method;
        Class object_cls("UnityEngine", "Object");
        if (!object_cls.IsValid()) return {};
        for (auto& candidate : object_cls.GetMethods(false)) {
            auto* info = candidate.GetInfo();
            if (!info || !is_valid_pointer(info) || !info->name || !is_valid_pointer(info->name) ||
                strcmp(info->name, "Destroy") != 0 || info->parameters_count != 1 ||
                info->is_generic || !method_has_pointer(candidate)) continue;
            std::string parameter = get_param_type_name(info, 0);
            if (parameter == "UnityEngine.Object" || parameter == "Object" ||
                parameter.find("UnityEngine.Object") != std::string::npos) {
                method = candidate;
                break;
            }
        }
        return method;
    }

    static MethodBase find_objs_method_inactive() {
        static MethodBase m;
        if (m.IsValid()) return m;
        Class oc("UnityEngine", "Object");
        for (auto& mt : oc.GetMethods(false)) {
            auto* mi = mt.GetInfo();
            if (mi && is_valid_pointer(mi) && mi->name && is_valid_pointer(mi->name) && strcmp(mi->name, "FindObjectsOfType") == 0 && mi->parameters_count == 2 && !mi->is_generic) {
                std::string pn = get_param_type_name(mi, 0);
                std::string pb = get_param_type_name(mi, 1);
                if (pn.find("Type") != std::string::npos && pb.find("Boolean") != std::string::npos) {
                    m = mt; break;
                }
            }
        }
        return m;
    }

    static Il2CppObject* sys_type_of(const std::string& aqn) {
        Class tc("System", "Type", Image("mscorlib.dll"));
        if (!tc.IsValid()) return nullptr;
        Method<Il2CppObject*> gt(tc.GetMethod("GetType", 1));
        return gt.IsValid() ? gt(CreateMonoString(aqn.c_str())) : nullptr;
    }

    static bool is_unity_object_instance(Il2CppObject* obj);

    static bool is_object_alive(Il2CppObject* obj) {
        if (!obj || !is_valid_pointer(obj) || !is_valid_pointer(obj->klass)) return false;
        if (is_unity_object_instance(obj)) {
            return reinterpret_cast<BNM::UnityEngine::Object*>(obj)->IsValid();
        }
        return obj->klass->name && is_valid_pointer(obj->klass->name);
    }

    static bool is_unity_object_instance(Il2CppObject* obj) {
        if (!obj || !is_valid_pointer(obj) || !is_valid_pointer(obj->klass)) return false;
        try {
            for (auto* k = obj->klass; k && is_valid_pointer(k); k = k->parent) {
                if (k->name && is_valid_pointer(k->name) && strcmp(k->name, "Object") == 0 &&
                    k->namespaze && is_valid_pointer(k->namespaze) && strcmp(k->namespaze, "UnityEngine") == 0) {
                    return true;
                }
            }
        } catch(...) {}
        return false;
    }

    static bool il2cpp_class_has_base(Il2CppClass* klass, const char* namespaze, const char* name) {
        if (!klass || !is_valid_pointer(klass) || !namespaze || !name) return false;
        try {
            for (auto* cur = klass; cur && is_valid_pointer(cur); cur = cur->parent) {
                if (!cur->name || !is_valid_pointer(cur->name) || !cur->namespaze || !is_valid_pointer(cur->namespaze)) continue;
                if (strcmp(cur->name, name) == 0 && strcmp(cur->namespaze, namespaze) == 0) return true;
            }
        } catch(...) {}
        return false;
    }

    static Il2CppObject* resolve_component_by_addr(uintptr_t addr) {
        if (!addr || !is_valid_pointer((void*)addr)) return nullptr;
        Il2CppObject* obj = (Il2CppObject*)addr;
        if (!is_object_alive(obj)) return nullptr;
        return obj;
    }

    static std::string assemblies_json() {
        attach_thread();
        std::ostringstream j;
        j << "[";
        bool first = true;
        for (auto& img : Image::GetImages()) {
            auto* d = img.GetInfo();
            if (!img.IsValid() || !d || !is_valid_pointer(d) || !d->name || !is_valid_pointer(d->name)) continue;
            if (!first) j << ",";
            first = false;
            j << "\"" << js_esc(d->name) << "\"";
        }
        j << "]";
        return j.str();
    }

    static std::string classes_json(const std::string& asm_) {
        attach_thread();
        Image img(asm_);
        if (!img.IsValid()) return "[]";
        std::ostringstream j;
        j << "[";
        bool first = true;
        for (auto& cls : img.GetClasses(false)) {
            try {
                auto* r = cls.GetClass();
                if (!cls.IsValid() || !r || !is_valid_pointer(r) || !r->name || !is_valid_pointer(r->name)) continue;
                if (!first) j << ",";
                first = false;
                std::string t = "class";
                if (r->enumtype) t = "enum";
                else if (bnm_class_is_value_type(r)) t = "struct";
                else if (r->flags & 0x20) t = "interface";
                std::string ns = (r->namespaze && is_valid_pointer(r->namespaze)) ? r->namespaze : "";
                j << "{\"name\":\"" << js_esc(r->name) << "\",\"ns\":\"" << js_esc(ns) << "\",\"t\":\"" << t << "\"}";
            } catch(...) {}
        }
        j << "]";
        return j.str();
    }

    static std::string all_classes_json() {
        attach_thread();
        std::ostringstream j;
        j << "[";
        bool first = true;
        for (auto& img : Image::GetImages()) {
            auto* d = img.GetInfo();
            if (!img.IsValid() || !d || !is_valid_pointer(d) || !d->name || !is_valid_pointer(d->name)) continue;
            std::string a = d->name;
            for (auto& cls : img.GetClasses(false)) {
                try {
                    auto* r = cls.GetClass();
                    if (!cls.IsValid() || !r || !is_valid_pointer(r) || !r->name || !is_valid_pointer(r->name)) continue;
                    if (!first) j << ",";
                    first = false;
                    std::string t = "class";
                    if (r->enumtype) t = "enum";
                    else if (bnm_class_is_value_type(r)) t = "struct";
                    else if (r->flags & 0x20) t = "interface";
                    std::string ns = (r->namespaze && is_valid_pointer(r->namespaze)) ? r->namespaze : "";
                    j << "{\"name\":\"" << js_esc(r->name) << "\",\"ns\":\"" << js_esc(ns) << "\",\"t\":\"" << t << "\",\"a\":\"" << js_esc(a) << "\"}";
                } catch(...) {}
            }
        }
        j << "]";
        return j.str();
    }

    static std::string class_detail_json(const std::string& a, const std::string& ns, const std::string& cn) {
        attach_thread();
        if (cn.empty()) return "{}";
        try {
            Class cls = a.empty() ? Class(ns.c_str(), cn.c_str()) : Class(ns.c_str(), cn.c_str(), Image(a.c_str()));
            if (!cls.IsValid()) {
                for (auto& img : Image::GetImages()) {
                    if (!img.IsValid()) continue;
                    Class try_c(ns.c_str(), cn.c_str(), img);
                    if (try_c.IsValid()) { cls = try_c; break; }
                }
            }
            if (!cls.IsValid()) return "{}";
            auto* r = cls.GetClass();
            if (!r || !is_valid_pointer(r)) return "{}";

            std::ostringstream j;
            j << "{\"name\":\"" << js_esc(cn) << "\",\"ns\":\"" << js_esc(ns) << "\",\"asm\":\"" << js_esc(a) << "\",";
            j << (r->parent && is_valid_pointer(r->parent) && is_valid_pointer(r->parent->name) ? "\"parent\":\"" + js_esc(r->parent->name) + "\"," : "\"parent\":null,");

            j << "\"fields\":[";
            bool first = true;
            for (auto& f : cls.GetFields(false)) {
                try {
                    auto* fi = f.GetInfo();
                    if (!f.IsValid() || !fi || !is_valid_pointer(fi) || !fi->name || !is_valid_pointer(fi->name)) continue;
                    if (!first) j << ",";
                    first = false;
                    std::string fn = fi->name;
                    std::string ft = (fi->type && is_valid_pointer(fi->type)) ? type_name(fi->type) : "?";
                    bool is_s = fi->type && is_valid_pointer(fi->type) && (fi->type->attrs & 0x10);
                    j << "{\"name\":\"" << js_esc(fn) << "\",\"type\":\"" << js_esc(ft) << "\",\"s\":" << (is_s ? "true" : "false") << ",\"off\":" << fi->offset << "}";
                } catch(...) {}
            }

            j << "],\"methods\":[";
            first = true;
            for (auto& m : cls.GetMethods(false)) {
                try {
                    auto* mi = m.GetInfo();
                    if (!m.IsValid() || !mi || !is_valid_pointer(mi) || !mi->name || !is_valid_pointer(mi->name)) continue;
                    if (!first) j << ",";
                    first = false;
                    char ab[32] = {};
                    if (mi->methodPointer) snprintf(ab, sizeof(ab), "%llX", (unsigned long long)(uintptr_t)mi->methodPointer);
                    std::string ret_t = (mi->return_type && is_valid_pointer(mi->return_type)) ? type_name(mi->return_type) : "?";
                    bool is_s = (mi->flags & 0x10) != 0;
                    j << "{\"name\":\"" << js_esc(mi->name) << "\",\"ret\":\"" << js_esc(ret_t) << "\",\"s\":" << (is_s ? "true" : "false") << ",\"addr\":\"" << ab << "\",\"params\":[";
                    int pcount = (int)mi->parameters_count;
                    for (int i = 0; i < pcount; i++) {
                        if (i > 0) j << ",";
                        std::string pt = get_param_type_name(mi, i);
                        std::string pn = get_param_name(mi, i);
                        j << "{\"n\":\"" << js_esc(pn) << "\",\"t\":\"" << js_esc(pt) << "\"}";
                    }
                    j << "]}";
                } catch(...) {}
            }

            j << "],\"props\":[";
            first = true;
            for (auto& p : cls.GetProperties(false)) {
                try {
                    auto* pi = p._data;
                    if (!p.IsValid() || !pi || !is_valid_pointer(pi) || !pi->name || !is_valid_pointer(pi->name)) continue;
                    if (!first) j << ",";
                    first = false;
                    std::string pn = pi->name;
                    std::string pt = (pi->get && is_valid_pointer(pi->get) && pi->get->return_type && is_valid_pointer(pi->get->return_type)) ? type_name(pi->get->return_type) : "?";
                    j << "{\"name\":\"" << js_esc(pn) << "\",\"type\":\"" << js_esc(pt) << "\",\"g\":" << (pi->get ? "true" : "false") << ",\"s\":" << (pi->set ? "true" : "false") << "}";
                } catch(...) {}
            }
            j << "]}";
            return j.str();
        } catch(...) {
            return "{}";
        }
    }


    static bool is_valid_object(Il2CppObject* o) {
        if (!o || !is_readable(o, sizeof(Il2CppObject))) return false;
        Il2CppClass* k = o->klass;
        if (!k || !is_readable(k, 128)) return false;
        if (!is_valid_pointer(k->name) || !is_readable(k->name, 1)) return false;
        return true;
    }

    static std::string class_short_name(Il2CppClass* k) {
        if (!k || !is_valid_pointer(k) || !is_valid_pointer(k->name)) return "?";
        std::string n = k->name;
        auto bt = n.find('`');
        if (bt != std::string::npos) n = n.substr(0, bt);
        return n;
    }

    static std::string class_full_name(Il2CppClass* k) {
        if (!k || !is_valid_pointer(k) || !is_valid_pointer(k->name)) return "?";
        std::string n = k->name;
        if (k->namespaze && is_valid_pointer(k->namespaze) && k->namespaze[0] != '\0')
            n = std::string(k->namespaze) + "." + n;
        return n;
    }

    static bool class_inherits(Il2CppClass* k, const char* ns, const char* name) {
        int guard = 0;
        for (; k && is_valid_pointer(k) && guard < 32; k = k->parent, guard++) {
            if (!is_valid_pointer(k->name) || !is_valid_pointer(k->namespaze)) continue;
            if (strcmp(k->name, name) == 0 && strcmp(k->namespaze, ns) == 0) return true;
        }
        return false;
    }

    static bool is_string_object(Il2CppObject* o) {
        if (!o || !is_valid_pointer(o->klass) || !is_valid_pointer(o->klass->name)) return false;
        return strcmp(o->klass->name, "String") == 0 && is_valid_pointer(o->klass->namespaze) && strcmp(o->klass->namespaze, "System") == 0;
    }

    static std::string unity_object_name(Il2CppObject* o) {
        try {
            static Method<BNM::Structures::Mono::String*> gn(Class("UnityEngine", "Object").GetMethod("get_name"));
            if (gn.IsValid() && is_object_alive(o)) {
                auto* s = gn[o]();
                if (s) return s->str();
            }
        } catch(...) {}
        return "";
    }

    static uintptr_t object_game_object(Il2CppObject* o) {
        if (!is_valid_object(o)) return 0;
        try {
            if (class_inherits(o->klass, "UnityEngine", "GameObject")) return (uintptr_t)o;
            if (class_inherits(o->klass, "UnityEngine", "Component")) {
                static Method<Il2CppObject*> gg(Class("UnityEngine", "Component").GetMethod("get_gameObject"));
                if (gg.IsValid() && is_object_alive(o)) {
                    Il2CppObject* go = gg[o]();
                    if (go && is_valid_object(go)) return (uintptr_t)go;
                }
            }
        } catch(...) {}
        return 0;
    }

    struct collection_info {
        std::string kind;
        int count = 0;
        Il2CppObject* items = nullptr;
        Il2CppClass* elem_class = nullptr;
        bool listable = false;
    };

    static bool read_int_field(Il2CppObject* o, std::initializer_list<const char*> names, int& out) {
        Class c(o->klass);
        for (const char* n : names) {
            for (Class cur = c; cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
                Field<int> f = cur.GetField(n);
                if (f.IsValid()) {
                    f.SetInstance(o);
                    out = f();
                    return true;
                }
            }
        }
        return false;
    }

    static bool read_ref_field(Il2CppObject* o, std::initializer_list<const char*> names, Il2CppObject*& out) {
        Class c(o->klass);
        for (const char* n : names) {
            for (Class cur = c; cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
                Field<Il2CppObject*> f = cur.GetField(n);
                if (f.IsValid()) {
                    f.SetInstance(o);
                    out = f();
                    return true;
                }
            }
        }
        return false;
    }

    static int array_length(Il2CppObject* arr) {
        if (!arr || !is_readable(arr, 32)) return 0;
        auto* a = (Array<uint8_t>*)arr;
        int n = (int)a->capacity;
        if (n < 0 || n > 10000000) return 0;
        return n;
    }

    static bool get_collection_info(Il2CppObject* o, collection_info& ci) {
        ci = collection_info();
        if (!is_valid_object(o)) return false;
        Il2CppClass* k = o->klass;
        std::string n = k->name;
        try {
            if (n.size() > 2 && n.compare(n.size() - 2, 2, "[]") == 0 && k->element_class) {
                ci.kind = "Array";
                ci.items = o;
                ci.count = array_length(o);
                ci.elem_class = k->element_class;
                ci.listable = true;
                return true;
            }
            if (n.compare(0, 6, "List`1") == 0) {
                ci.kind = "List";
                read_int_field(o, {"_size", "size"}, ci.count);
                Il2CppObject* items = nullptr;
                read_ref_field(o, {"_items", "items"}, items);
                if (items && is_valid_object(items)) {
                    ci.items = items;
                    ci.elem_class = items->klass->element_class;
                }
                ci.listable = true;
                return true;
            }
            if (n.compare(0, 12, "Dictionary`2") == 0) {
                ci.kind = "Dictionary";
                int total = 0, freed = 0;
                if (read_int_field(o, {"_count"}, total)) {
                    read_int_field(o, {"_freeCount"}, freed);
                    ci.count = total - freed;
                } else {
                    read_int_field(o, {"count"}, ci.count);
                }
                ci.listable = true;
                return true;
            }
            static const char* count_only[][2] = {
                    {"HashSet`1", "HashSet"}, {"Queue`1", "Queue"}, {"Stack`1", "Stack"},
                    {"LinkedList`1", "LinkedList"}, {"SortedList`2", "SortedList"}, {"SortedDictionary`2", "SortedDictionary"}
            };
            for (auto& e : count_only) {
                if (n == e[0]) {
                    ci.kind = e[1];
                    read_int_field(o, {"_count", "m_count", "count", "_size", "size"}, ci.count);
                    return true;
                }
            }
        } catch(...) {}
        return false;
    }

    static std::string object_summary(Il2CppObject* o) {
        if (!is_valid_object(o)) return "";
        Il2CppClass* k = o->klass;
        try {
            if (is_string_object(o)) {
                auto* s = (BNM::Structures::Mono::String*)o;
                std::string v = s->str();
                if (v.size() > 80) v = v.substr(0, 80) + "...";
                return "\"" + v + "\"";
            }
            collection_info ci;
            if (get_collection_info(o, ci)) {
                std::string label;
                if (ci.kind == "Array") label = class_full_name(k);
                else {
                    label = ci.kind;
                    if (ci.elem_class) label += "<" + class_full_name(ci.elem_class) + ">";
                }
                return label + " [" + std::to_string(ci.count) + "]";
            }
            if (is_unity_object_instance(o)) {
                std::string nm = unity_object_name(o);
                std::string cn = class_short_name(k);
                return nm.empty() ? cn : nm + " (" + cn + ")";
            }
        } catch(...) {}
        return class_full_name(k);
    }

    static std::string object_ref_string(Il2CppObject* o) {
        if (!o || !is_valid_pointer(o)) return "0x0";
        char buf[32];
        snprintf(buf, sizeof(buf), "0x%llX", (unsigned long long)(uintptr_t)o);
        std::string sm = object_summary(o);
        if (sm.empty()) return buf;
        return std::string(buf) + " (" + sm + ")";
    }

    static std::string object_kind(Il2CppObject* o) {
        if (!is_valid_object(o)) return "invalid";
        if (is_string_object(o)) return "string";
        if (class_inherits(o->klass, "UnityEngine", "GameObject")) return "gameobject";
        if (class_inherits(o->klass, "UnityEngine", "Component")) return "component";
        collection_info ci;
        if (get_collection_info(o, ci)) return ci.listable ? "collection" : "counted";
        if (is_unity_object_instance(o)) return "unity_object";
        return "object";
    }

    static bool read_value_at(Il2CppClass* ec, uint8_t* p, std::string& out, Il2CppObject*& ref) {
        ref = nullptr;
        out = "?";
        if (!ec || !is_valid_pointer(ec) || !is_valid_pointer(ec->name)) return false;
        if (!is_readable(p, 16)) { out = "<unreadable>"; return false; }
        bool is_value = bnm_class_is_value_type(ec) || ec->enumtype;
        std::string nm = ec->name;
        char b[160];
        if (ec->enumtype) { out = std::to_string(*(int32_t*)p); return true; }
        if (is_value) {
            if (nm == "Int32") out = std::to_string(*(int32_t*)p);
            else if (nm == "UInt32") out = std::to_string(*(uint32_t*)p);
            else if (nm == "Int64") out = std::to_string(*(int64_t*)p);
            else if (nm == "UInt64") out = std::to_string(*(uint64_t*)p);
            else if (nm == "Int16") out = std::to_string(*(int16_t*)p);
            else if (nm == "UInt16" || nm == "Char") out = std::to_string(*(uint16_t*)p);
            else if (nm == "Byte") out = std::to_string(*(uint8_t*)p);
            else if (nm == "SByte") out = std::to_string(*(int8_t*)p);
            else if (nm == "Boolean") out = (*(uint8_t*)p) ? "true" : "false";
            else if (nm == "IntPtr" || nm == "UIntPtr") { snprintf(b, sizeof(b), "0x%llX", (unsigned long long)*(uint64_t*)p); out = b; }
            else if (nm == "Single") { snprintf(b, sizeof(b), "%g", *(float*)p); out = b; }
            else if (nm == "Double") { snprintf(b, sizeof(b), "%g", *(double*)p); out = b; }
            else if (nm == "Vector3") { float* f = (float*)p; snprintf(b, sizeof(b), "[%g, %g, %g]", f[0], f[1], f[2]); out = b; }
            else if (nm == "Vector2") { float* f = (float*)p; snprintf(b, sizeof(b), "[%g, %g]", f[0], f[1]); out = b; }
            else if (nm == "Vector4" || nm == "Quaternion" || nm == "Color") { float* f = (float*)p; snprintf(b, sizeof(b), "[%g, %g, %g, %g]", f[0], f[1], f[2], f[3]); out = b; }
            else if (nm == "Vector3Int") { int32_t* f = (int32_t*)p; snprintf(b, sizeof(b), "[%d, %d, %d]", f[0], f[1], f[2]); out = b; }
            else if (nm == "Vector2Int") { int32_t* f = (int32_t*)p; snprintf(b, sizeof(b), "[%d, %d]", f[0], f[1]); out = b; }
            else out = "{" + class_full_name(ec) + "}";
            return true;
        }
        Il2CppObject* r = *(Il2CppObject**)p;
        ref = r;
        if (!r) { out = "null"; return true; }
        if (!is_valid_object(r)) { out = "<invalid>"; ref = nullptr; return false; }
        out = object_summary(r);
        return true;
    }

    static std::string collection_item_json(const std::string& label, Il2CppClass* ec, uint8_t* p) {
        std::string v;
        Il2CppObject* ref = nullptr;
        read_value_at(ec, p, v, ref);
        std::ostringstream j;
        j << "{\"i\":\"" << js_esc(label) << "\",\"t\":\"" << js_esc(class_full_name(ec)) << "\",\"v\":\"" << js_esc(v) << "\"";
        if (ref && is_valid_object(ref)) {
            char ab[32];
            snprintf(ab, sizeof(ab), "%llX", (unsigned long long)(uintptr_t)ref);
            j << ",\"addr\":\"" << ab << "\",\"kind\":\"" << object_kind(ref) << "\"";
        } else {
            j << ",\"addr\":\"\",\"kind\":\"value\"";
        }
        j << "}";
        return j.str();
    }

    static uint32_t array_stride(Il2CppObject* arr, Il2CppClass* ec) {
        uint32_t st = arr && arr->klass ? arr->klass->element_size : 0;
        if (st == 0 && ec) {
            bool is_value = bnm_class_is_value_type(ec) || ec->enumtype;
            st = is_value ? (ec->instance_size > sizeof(Il2CppObject) ? ec->instance_size - (uint32_t)sizeof(Il2CppObject) : 4) : (uint32_t)sizeof(void*);
        }
        return st;
    }

    static uint8_t* array_data(Il2CppObject* arr) {
        return (uint8_t*)((Array<uint8_t>*)arr)->m_Items;
    }

    static std::string dictionary_items_json(Il2CppObject* o, int max_items, bool& truncated) {
        std::ostringstream j;
        bool first = true;
        int emitted = 0;
        Il2CppObject* entries = nullptr;
        if (read_ref_field(o, {"_entries", "entries"}, entries) && entries && is_valid_object(entries)) {
            Il2CppClass* ec = entries->klass->element_class;
            uint32_t stride = array_stride(entries, ec);
            int len = array_length(entries);
            int off_hash = -1, off_key = -1, off_val = -1;
            Il2CppClass* kc = nullptr;
            Il2CppClass* vc = nullptr;
            int min_off = 1 << 30;
            Class ecl(ec);
            for (auto& f : ecl.GetFields(false)) {
                auto* fi = f.GetInfo();
                if (!f.IsValid() || !fi || !is_valid_pointer(fi) || !is_valid_pointer(fi->name) || !fi->type || (fi->type->attrs & 0x10)) continue;
                if ((int)fi->offset < min_off) min_off = (int)fi->offset;
            }
            int adj = min_off >= (int)sizeof(Il2CppObject) && min_off != (1 << 30) ? (int)sizeof(Il2CppObject) : 0;
            for (auto& f : ecl.GetFields(false)) {
                auto* fi = f.GetInfo();
                if (!f.IsValid() || !fi || !is_valid_pointer(fi) || !is_valid_pointer(fi->name) || !fi->type || (fi->type->attrs & 0x10)) continue;
                std::string fname = fi->name;
                Class fc(fi->type);
                if (fname == "hashCode") off_hash = (int)fi->offset - adj;
                else if (fname == "key") { off_key = (int)fi->offset - adj; kc = fc.GetClass(); }
                else if (fname == "value") { off_val = (int)fi->offset - adj; vc = fc.GetClass(); }
            }
            if (off_key >= 0 && off_val >= 0 && kc && vc && stride > 0) {
                uint8_t* base = array_data(entries);
                for (int i = 0; i < len; i++) {
                    if (emitted >= max_items) { truncated = true; break; }
                    uint8_t* e = base + (size_t)i * stride;
                    if (!is_readable(e, std::min<uint32_t>(stride, 64))) break;
                    if (off_hash >= 0 && *(int32_t*)(e + off_hash) < 0) continue;
                    std::string kv;
                    Il2CppObject* kr = nullptr;
                    read_value_at(kc, e + off_key, kv, kr);
                    if (!first) j << ",";
                    first = false;
                    j << collection_item_json(kv, vc, e + off_val);
                    emitted++;
                }
            }
            return j.str();
        }
        Il2CppObject* key_slots = nullptr;
        Il2CppObject* val_slots = nullptr;
        Il2CppObject* link_slots = nullptr;
        int touched = 0;
        if (read_ref_field(o, {"keySlots"}, key_slots) && read_ref_field(o, {"valueSlots"}, val_slots) &&
            key_slots && val_slots && is_valid_object(key_slots) && is_valid_object(val_slots)) {
            read_ref_field(o, {"linkSlots"}, link_slots);
            read_int_field(o, {"touchedSlots"}, touched);
            Il2CppClass* kc = key_slots->klass->element_class;
            Il2CppClass* vc = val_slots->klass->element_class;
            uint32_t ks = array_stride(key_slots, kc), vs = array_stride(val_slots, vc);
            uint32_t ls = link_slots && is_valid_object(link_slots) ? array_stride(link_slots, link_slots->klass->element_class) : 0;
            int len = std::min(touched, std::min(array_length(key_slots), array_length(val_slots)));
            for (int i = 0; i < len; i++) {
                if (emitted >= max_items) { truncated = true; break; }
                if (ls >= 4) {
                    uint8_t* lp = array_data(link_slots) + (size_t)i * ls;
                    if (!is_readable(lp, 4)) break;
                    if ((*(uint32_t*)lp & 0x80000000u) == 0) continue;
                }
                std::string kv;
                Il2CppObject* kr = nullptr;
                read_value_at(kc, array_data(key_slots) + (size_t)i * ks, kv, kr);
                if (!first) j << ",";
                first = false;
                j << collection_item_json(kv, vc, array_data(val_slots) + (size_t)i * vs);
                emitted++;
            }
        }
        return j.str();
    }

    static std::string collection_json(uintptr_t addr) {
        attach_thread();
        Il2CppObject* o = (Il2CppObject*)addr;
        collection_info ci;
        if (!addr || !get_collection_info(o, ci)) return "{\"ok\":false,\"error\":\"Not a supported collection\"}";
        std::ostringstream j;
        const int max_items = 300;
        bool truncated = false;
        j << "{\"ok\":true,\"kind\":\"" << ci.kind << "\",\"type\":\"" << js_esc(class_full_name(o->klass)) << "\",\"count\":" << ci.count
          << ",\"elem\":\"" << js_esc(ci.elem_class ? class_full_name(ci.elem_class) : "") << "\",\"items\":[";
        try {
            if (ci.kind == "Dictionary") {
                j << dictionary_items_json(o, max_items, truncated);
            } else if (ci.items && ci.elem_class) {
                int len = std::min(ci.count, array_length(ci.items));
                uint32_t stride = array_stride(ci.items, ci.elem_class);
                uint8_t* base = array_data(ci.items);
                bool first = true;
                for (int i = 0; i < len; i++) {
                    if (i >= max_items) { truncated = true; break; }
                    if (!first) j << ",";
                    first = false;
                    j << collection_item_json(std::to_string(i), ci.elem_class, base + (size_t)i * stride);
                }
            }
        } catch(...) {}
        j << "],\"truncated\":" << (truncated ? "true" : "false") << "}";
        return j.str();
    }

    static std::string object_info_json(uintptr_t addr) {
        attach_thread();
        Il2CppObject* o = (Il2CppObject*)addr;
        if (!addr || !is_valid_object(o)) return "{\"ok\":false}";
        Il2CppClass* k = o->klass;
        std::string asm_name = (k->image && is_valid_pointer(k->image) && k->image->name && is_valid_pointer(k->image->name)) ? k->image->name : "";
        std::string ns = (k->namespaze && is_valid_pointer(k->namespaze)) ? k->namespaze : "";
        std::string nm = k->name;
        char ab[32];
        snprintf(ab, sizeof(ab), "%llX", (unsigned long long)addr);
        char gb[32] = {};
        uintptr_t go = object_game_object(o);
        if (go) snprintf(gb, sizeof(gb), "%llX", (unsigned long long)go);
        std::ostringstream j;
        j << "{\"ok\":true,\"addr\":\"" << ab << "\",\"kind\":\"" << object_kind(o) << "\",\"go\":\"" << gb << "\",\"asm\":\"" << js_esc(asm_name)
          << "\",\"ns\":\"" << js_esc(ns) << "\",\"cls\":\"" << js_esc(nm) << "\",\"summary\":\"" << js_esc(object_summary(o)) << "\"}";
        return j.str();
    }

    struct invoke_result { bool ok; std::string val, err; };

    static invoke_result invoke_method(const std::string& body) {
        attach_thread();
        invoke_result res = {false, "", ""};
        std::string a = json_val(body, "asm"), ns = json_val(body, "ns"), cn = json_val(body, "cls"), mn = json_val(body, "method");
        bool is_static = json_val(body, "static") == "true" || json_val(body, "static") == "1";
        uintptr_t inst_addr = (uintptr_t)strtoull(json_val(body, "instance").c_str(), nullptr, 16);

        if (!is_static && (!inst_addr || !is_valid_pointer((void*)inst_addr))) {
            res.err = "Instance address is invalid or null for non-static method";
            return res;
        }

        std::vector<std::string> arg_t, arg_v;
        auto ap = body.find("\"args\":[");
        if (ap != std::string::npos) {
            ap += 8;
            auto ae = body.find("]", ap);
            if (ae != std::string::npos) {
                std::string ab = body.substr(ap, ae - ap);
                size_t p = 0;
                while (p < ab.size()) {
                    auto ob = ab.find("{", p), cb = ab.find("}", ob);
                    if (ob == std::string::npos || cb == std::string::npos) break;
                    std::string e = "{" + ab.substr(ob + 1, cb - ob - 1) + "}";
                    arg_t.push_back(json_val(e, "t"));
                    arg_v.push_back(json_val(e, "v"));
                    p = cb + 1;
                }
            }
        }

        if (cn.empty() || mn.empty()) { res.err = "Missing class or method name"; return res; }

        Class cls = a.empty() ? Class(ns.c_str(), cn.c_str()) : Class(ns.c_str(), cn.c_str(), Image(a.c_str()));
        if (!cls.IsValid()) {
            for (auto& img : Image::GetImages()) {
                if (!img.IsValid()) continue;
                Class try_c(ns.c_str(), cn.c_str(), img);
                if (try_c.IsValid()) { cls = try_c; break; }
            }
        }
        if (!cls.IsValid()) { res.err = "Target class not found"; return res; }

        MethodBase mb = cls.GetMethod(mn.c_str(), (int)arg_t.size());
        if (!mb.IsValid()) mb = cls.GetMethod(mn.c_str());
        if (!mb.IsValid()) {
            for (Class cur = cls.GetParent(); cur.IsValid() && cur.GetClass() && !mb.IsValid(); cur = cur.GetParent()) {
                mb = cur.GetMethod(mn.c_str(), (int)arg_t.size());
                if (!mb.IsValid()) mb = cur.GetMethod(mn.c_str());
            }
        }
        if (!mb.IsValid()) { res.err = "Method not found in class hierarchy"; return res; }
        auto* mi = mb.GetInfo();
        if (!mi || !mi->methodPointer) { res.err = "No compiled pointer for method"; return res; }

        std::vector<void*> runtime_args;
        std::vector<int32_t> vi;
        std::vector<uint32_t> vui;
        std::vector<int64_t> vi64;
        std::vector<uint64_t> vui64;
        std::vector<float> vf;
        std::vector<double> vd;
        std::vector<uint8_t> vb;
        std::vector<int8_t> vs8;
        std::vector<int16_t> vs16;
        std::vector<uint16_t> vus16;
        std::vector<Vector3> vv3;
        std::vector<Vector2> vv2;
        std::vector<Color> vc;
        std::vector<Vector4> vv4;
        std::vector<vector3_int> vv3i;
        std::vector<vector2_int> vv2i;
        std::vector<void*> vptrs;

        for (size_t i = 0; i < arg_t.size(); i++) {
            auto& t = arg_t[i]; auto& v = arg_v[i];
            if (t=="System.Int32"||t=="Int32"||t=="int") {
                vi.push_back(std::stoi(v.empty()?"0":v)); runtime_args.push_back(&vi.back());
            } else if (t=="System.UInt32"||t=="UInt32"||t=="uint") {
                vui.push_back((uint32_t)std::stoul(v.empty()?"0":v)); runtime_args.push_back(&vui.back());
            } else if (t=="System.Int64"||t=="Int64"||t=="long") {
                vi64.push_back(std::stoll(v.empty()?"0":v)); runtime_args.push_back(&vi64.back());
            } else if (t=="System.UInt64"||t=="UInt64"||t=="ulong") {
                vui64.push_back(std::stoull(v.empty()?"0":v)); runtime_args.push_back(&vui64.back());
            } else if (t=="System.Single"||t=="Single"||t=="float") {
                vf.push_back(std::stof(v.empty()?"0":v)); runtime_args.push_back(&vf.back());
            } else if (t=="System.Double"||t=="Double"||t=="double") {
                vd.push_back(std::stod(v.empty()?"0":v)); runtime_args.push_back(&vd.back());
            } else if (t=="System.Boolean"||t=="Boolean"||t=="bool") {
                vb.push_back((uint8_t)(v=="true"||v=="1"?1:0)); runtime_args.push_back(&vb.back());
            } else if (t=="System.Byte"||t=="Byte"||t=="byte") {
                vb.push_back((uint8_t)std::stoul(v.empty()?"0":v)); runtime_args.push_back(&vb.back());
            } else if (t=="System.SByte"||t=="SByte"||t=="sbyte") {
                vs8.push_back((int8_t)std::stoi(v.empty()?"0":v)); runtime_args.push_back(&vs8.back());
            } else if (t=="System.Int16"||t=="Int16"||t=="short") {
                vs16.push_back((int16_t)std::stoi(v.empty()?"0":v)); runtime_args.push_back(&vs16.back());
            } else if (t=="System.UInt16"||t=="UInt16"||t=="ushort") {
                vus16.push_back((uint16_t)std::stoul(v.empty()?"0":v)); runtime_args.push_back(&vus16.back());
            } else if (t=="UnityEngine.Vector3"||t=="Vector3") {
                float x=0,y=0,z=0; sscanf(v.c_str(),"[%f,%f,%f]",&x,&y,&z); vv3.push_back({x,y,z}); runtime_args.push_back(&vv3.back());
            } else if (t=="UnityEngine.Vector2"||t=="Vector2") {
                float x=0,y=0; sscanf(v.c_str(),"[%f,%f]",&x,&y); vv2.push_back({x,y}); runtime_args.push_back(&vv2.back());
            } else if (t=="UnityEngine.Vector3Int"||t=="Vector3Int") {
                int x=0,y=0,z=0; sscanf(v.c_str(),"[%d,%d,%d]",&x,&y,&z); vv3i.push_back({x,y,z}); runtime_args.push_back(&vv3i.back());
            } else if (t=="UnityEngine.Vector2Int"||t=="Vector2Int") {
                int x=0,y=0; sscanf(v.c_str(),"[%d,%d]",&x,&y); vv2i.push_back({x,y}); runtime_args.push_back(&vv2i.back());
            } else if (t=="UnityEngine.Color"||t=="Color") {
                float r=1,g=1,b=1,aa=1; sscanf(v.c_str(),"[%f,%f,%f,%f]",&r,&g,&b,&aa); vc.push_back({r,g,b,aa}); runtime_args.push_back(&vc.back());
            } else if (t=="UnityEngine.Vector4"||t=="Vector4"||t=="UnityEngine.Quaternion"||t=="Quaternion") {
                float x=0,y=0,z=0,w=0; sscanf(v.c_str(),"[%f,%f,%f,%f]",&x,&y,&z,&w); vv4.push_back({x,y,z,w}); runtime_args.push_back(&vv4.back());
            } else if (t=="System.String"||t=="String"||t=="string") {
                runtime_args.push_back(CreateMonoString(v));
            } else {
                uintptr_t ptr = (uintptr_t)strtoull(v.empty()?"0":v.c_str(), nullptr, 16);
                vptrs.push_back((void*)ptr);
                runtime_args.push_back(vptrs.back());
            }
        }

        void* inst = is_static ? nullptr : (void*)inst_addr;
        std::string rtn = (mi->return_type && is_valid_pointer(mi->return_type)) ? type_name(mi->return_type) : "?";
        std::ostringstream out;

        Il2CppException* exc = nullptr;
        void* ret = BNM::Internal::il2cppMethods.il2cpp_runtime_invoke(mi, inst, runtime_args.empty() ? nullptr : runtime_args.data(), &exc);

        if (exc) {
            BNM::Structures::Mono::String* exc_msg = nullptr;
            try {
                auto exc_cls = Class(((Il2CppObject*)exc)->klass);
                if (exc_cls.IsValid()) {
                    auto msg_method = exc_cls.GetMethod("get_Message", 0);
                    if (msg_method.IsValid()) {
                        auto* msg_mi = msg_method.GetInfo();
                        if (msg_mi && msg_mi->methodPointer) {
                            Il2CppException* inner = nullptr;
                            auto* msg_ret = (Il2CppObject*)BNM::Internal::il2cppMethods.il2cpp_runtime_invoke(msg_mi, exc, nullptr, &inner);
                            if (msg_ret && !inner) exc_msg = (BNM::Structures::Mono::String*)msg_ret;
                        }
                    }
                }
            } catch(...) {}
            res.err = exc_msg ? exc_msg->str() : "IL2CPP execution exception occurred";
            return res;
        }

        if (rtn=="System.Void"||rtn=="Void"||rtn=="void"||rtn=="?") {
            out << "void";
        } else if (rtn=="System.Single"||rtn=="Single"||rtn=="float") {
            float v = ret ? *(float*)((uint8_t*)ret + sizeof(Il2CppObject)) : 0;
            out << v;
        } else if (rtn=="System.Double"||rtn=="Double"||rtn=="double") {
            double v = ret ? *(double*)((uint8_t*)ret + sizeof(Il2CppObject)) : 0;
            out << v;
        } else if (rtn=="System.Int32"||rtn=="Int32"||rtn=="int") {
            int v = ret ? *(int*)((uint8_t*)ret + sizeof(Il2CppObject)) : 0;
            out << v;
        } else if (rtn=="System.UInt32"||rtn=="UInt32"||rtn=="uint") {
            uint32_t v = ret ? *(uint32_t*)((uint8_t*)ret + sizeof(Il2CppObject)) : 0;
            out << v;
        } else if (rtn=="System.Int64"||rtn=="Int64"||rtn=="long") {
            int64_t v = ret ? *(int64_t*)((uint8_t*)ret + sizeof(Il2CppObject)) : 0;
            out << v;
        } else if (rtn=="System.UInt64"||rtn=="UInt64"||rtn=="ulong") {
            uint64_t v = ret ? *(uint64_t*)((uint8_t*)ret + sizeof(Il2CppObject)) : 0;
            out << v;
        } else if (rtn=="System.Boolean"||rtn=="Boolean"||rtn=="bool") {
            bool v = ret ? *(bool*)((uint8_t*)ret + sizeof(Il2CppObject)) : false;
            out << (v?"true":"false");
        } else if (rtn=="System.Byte"||rtn=="Byte"||rtn=="byte") {
            uint8_t v = ret ? *(uint8_t*)((uint8_t*)ret + sizeof(Il2CppObject)) : 0;
            out << (int)v;
        } else if (rtn=="System.SByte"||rtn=="SByte"||rtn=="sbyte") {
            int8_t v = ret ? *(int8_t*)((uint8_t*)ret + sizeof(Il2CppObject)) : 0;
            out << (int)v;
        } else if (rtn=="UnityEngine.Vector3"||rtn=="Vector3") {
            Vector3 v = ret ? *(Vector3*)((uint8_t*)ret + sizeof(Il2CppObject)) : Vector3{0,0,0};
            char buf[128]; snprintf(buf,sizeof(buf),"[%.4f, %.4f, %.4f]",v.x,v.y,v.z); out<<buf;
        } else if (rtn=="UnityEngine.Vector2"||rtn=="Vector2") {
            Vector2 v = ret ? *(Vector2*)((uint8_t*)ret + sizeof(Il2CppObject)) : Vector2{0,0};
            char buf[128]; snprintf(buf,sizeof(buf),"[%.4f, %.4f]",v.x,v.y); out<<buf;
        } else if (rtn=="UnityEngine.Color"||rtn=="Color") {
            Color v = ret ? *(Color*)((uint8_t*)ret + sizeof(Il2CppObject)) : Color{0,0,0,0};
            char buf[128]; snprintf(buf,sizeof(buf),"[%.4f, %.4f, %.4f, %.4f]",v.r,v.g,v.b,v.a); out<<buf;
        } else if (rtn=="System.String"||rtn=="String"||rtn=="string") {
            auto* s = (BNM::Structures::Mono::String*)ret;
            out << (s ? s->str() : "null");
        } else {
            if (!ret) out << "null";
            else out << rtn << " @ " << object_ref_string((Il2CppObject*)ret);
        }
        res.ok = true; res.val = out.str();
        return res;
    }

    static std::string instances_json(const std::string& a, const std::string& ns, const std::string& cn) {
        attach_thread();
        if (cn.empty()) return "{\"instances\":[]}";
        std::string full = ns.empty() ? cn : ns + "." + cn;
        std::string a_no_ext = a;
        auto dp = a_no_ext.find(".dll");
        if (dp != std::string::npos) a_no_ext = a_no_ext.substr(0, dp);

        Il2CppObject* st = sys_type_of(full + ", " + a_no_ext);
        if (!st) st = sys_type_of(full);
        if (!st) {
            Class c = a.empty() ? Class(ns.c_str(), cn.c_str()) : Class(ns.c_str(), cn.c_str(), Image(a.c_str()));
            if (c.IsValid()) st = (Il2CppObject*)c.GetMonoType();
        }
        if (!st) return "{\"instances\":[],\"error\":\"Class not found / System.Type missing\"}";

        auto fm = find_objs_method();
        if (!fm.IsValid()) return "{\"instances\":[],\"error\":\"FindObjectsOfType missing\"}";

        std::ostringstream j;
        j << "{\"instances\":[";
        try {
            Array<Il2CppObject*>* arr = nullptr;
            auto fm_inactive = find_objs_method_inactive();
            if (fm_inactive.IsValid()) {
                try { arr = Method<Array<Il2CppObject*>*>(fm_inactive)(st, true); } catch(...) { arr = nullptr; }
            }
            if (!arr) arr = Method<Array<Il2CppObject*>*>(fm)(st);
            if (arr && arr->capacity > 0) {
                bool first = true;
                Method<BNM::Structures::Mono::String*> gn(Class("UnityEngine","Object").GetMethod("get_name"));
                for (int i = 0; i < arr->capacity; i++) {
                    Il2CppObject* obj = arr->m_Items[i];
                    if (!obj || !is_valid_pointer(obj)) continue;
                    if (!first) j << ",";
                    first = false;
                    std::string name = cn;
                    if (is_unity_object_instance(obj) && gn.IsValid()) {
                        auto* s = gn[obj]();
                        if (s) name = s->str();
                    }
                    char addr[32]; snprintf(addr, sizeof(addr), "%llX", (unsigned long long)(uintptr_t)obj);
                    j << "{\"addr\":\"" << addr << "\",\"name\":\"" << js_esc(name) << "\"}";
                }
            }
        } catch(...) {}
        j << "]}";
        return j.str();
    }

    static std::string find_instances_json(const std::string& type_name) {
        attach_thread();
        if (type_name.empty()) return "{\"instances\":[]}";

        std::string clean_type = type_name;
        if (clean_type.back() == '*') clean_type.pop_back();
        while (!clean_type.empty() && (clean_type.back() == ' ' || clean_type.back() == '&')) clean_type.pop_back();

        if (clean_type == "UnityEngine.GameObject" || clean_type == "GameObject") {
            auto fm = find_objs_method();
            if (!fm.IsValid()) return "{\"instances\":[]}";
            Il2CppObject* st = sys_type_of("UnityEngine.GameObject, UnityEngine.CoreModule");
            if (!st) st = sys_type_of("UnityEngine.GameObject, UnityEngine");
            if (!st) st = sys_type_of("UnityEngine.GameObject");
            if (!st) return "{\"instances\":[]}";
            Method<BNM::Structures::Mono::String*> gn(Class("UnityEngine","Object").GetMethod("get_name"));
            Method<bool> ga(Class("UnityEngine","GameObject").GetMethod("get_activeSelf"));
            std::ostringstream j;
            j << "{\"instances\":[";
            bool first = true;
            try {
                auto* arr = Method<Array<Il2CppObject*>*>(fm)(st);
                if (arr && arr->capacity > 0) {
                    for (int i = 0; i < arr->capacity; i++) {
                        Il2CppObject* go = arr->m_Items[i];
                        if (!go || !is_valid_pointer(go)) continue;
                        std::string name = "GameObject";
                        bool active = true;
                        if (gn.IsValid()) { auto* s = gn[go](); if (s) name = s->str(); }
                        if (ga.IsValid()) active = ga[go]();
                        if (!first) j << ","; first = false;
                        char addr[32]; snprintf(addr, sizeof(addr), "%llX", (unsigned long long)(uintptr_t)go);
                        j << "{\"addr\":\"" << addr << "\",\"name\":\"" << js_esc(name) << "\",\"active\":" << (active ? "true" : "false") << "}";
                    }
                }
            } catch(...) {}
            j << "]}";
            return j.str();
        }

        if (clean_type == "UnityEngine.Transform" || clean_type == "Transform" || clean_type == "UnityEngine.RectTransform" || clean_type == "RectTransform") {
            auto fm = find_objs_method();
            if (!fm.IsValid()) return "{\"instances\":[]}";
            Il2CppObject* st = sys_type_of("UnityEngine.Transform, UnityEngine.CoreModule");
            if (!st) st = sys_type_of("UnityEngine.Transform, UnityEngine");
            if (!st) st = sys_type_of("UnityEngine.Transform");
            if (!st) return "{\"instances\":[]}";
            Method<Il2CppObject*> cg(Class("UnityEngine","Component").GetMethod("get_gameObject"));
            Method<BNM::Structures::Mono::String*> gn(Class("UnityEngine","Object").GetMethod("get_name"));
            std::ostringstream j;
            j << "{\"instances\":[";
            bool first = true;
            try {
                auto* arr = Method<Array<Il2CppObject*>*>(fm)(st);
                if (arr && arr->capacity > 0) {
                    for (int i = 0; i < arr->capacity; i++) {
                        Il2CppObject* tr = arr->m_Items[i];
                        if (!tr || !is_valid_pointer(tr)) continue;
                        std::string name = "Transform";
                        if (cg.IsValid()) {
                            Il2CppObject* go = cg[tr]();
                            if (go && is_valid_pointer(go) && gn.IsValid()) { auto* s = gn[go](); if (s) name = s->str() + " (Transform)"; }
                        }
                        if (!first) j << ","; first = false;
                        char addr[32]; snprintf(addr, sizeof(addr), "%llX", (unsigned long long)(uintptr_t)tr);
                        j << "{\"addr\":\"" << addr << "\",\"name\":\"" << js_esc(name) << "\",\"active\":true}";
                    }
                }
            } catch(...) {}
            j << "]}";
            return j.str();
        }

        Il2CppObject* st = sys_type_of(clean_type + ", UnityEngine.CoreModule");
        if (!st) st = sys_type_of(clean_type + ", UnityEngine.PhysicsModule");
        if (!st) st = sys_type_of(clean_type + ", UnityEngine.UI");
        if (!st) st = sys_type_of(clean_type + ", UnityEngine");
        if (!st) st = sys_type_of(clean_type + ", Assembly-CSharp");
        if (!st) st = sys_type_of(clean_type);
        if (!st) {
            for (auto& img : Image::GetImages()) {
                if (!img.IsValid()) continue;
                for (auto& cls : img.GetClasses(false)) {
                    auto* r = cls.GetClass();
                    if (!r || !is_valid_pointer(r) || !r->name || !is_valid_pointer(r->name)) continue;
                    std::string full = (r->namespaze && is_valid_pointer(r->namespaze) && r->namespaze[0]) ? std::string(r->namespaze) + "." + r->name : r->name;
                    if (full == clean_type || std::string(r->name) == clean_type) {
                        st = (Il2CppObject*)cls.GetMonoType();
                        break;
                    }
                }
                if (st) break;
            }
        }

        if (!st) return "{\"instances\":[]}";

        auto fm = find_objs_method();
        if (!fm.IsValid()) return "{\"instances\":[]}";

        std::ostringstream j;
        j << "{\"instances\":[";
        bool first = true;
        try {
            auto* arr = Method<Array<Il2CppObject*>*>(fm)(st);
            if (arr && arr->capacity > 0) {
                Method<BNM::Structures::Mono::String*> gn(Class("UnityEngine","Object").GetMethod("get_name"));
                Method<Il2CppObject*> cg(Class("UnityEngine","Component").GetMethod("get_gameObject"));
                for (int i = 0; i < arr->capacity; i++) {
                    Il2CppObject* obj = arr->m_Items[i];
                    if (!obj || !is_valid_pointer(obj)) continue;
                    std::string name = clean_type;
                    if (is_unity_object_instance(obj) && gn.IsValid()) {
                        auto* s = gn[obj]();
                        if (s) name = s->str();
                    }
                    if (cg.IsValid()) {
                        try {
                            Il2CppObject* go = cg[obj]();
                            if (go && is_valid_pointer(go) && gn.IsValid()) {
                                auto* s = gn[go]();
                                if (s) name = s->str() + " (" + clean_type + ")";
                            }
                        } catch(...) {}
                    }
                    if (!first) j << ","; first = false;
                    char addr[32]; snprintf(addr, sizeof(addr), "%llX", (unsigned long long)(uintptr_t)obj);
                    j << "{\"addr\":\"" << addr << "\",\"name\":\"" << js_esc(name) << "\",\"active\":true}";
                }
            }
        } catch(...) {}
        j << "]}";
        return j.str();
    }

    static std::mutex g_scene_cache_mutex;
    static std::string g_scene_cache;
    static double g_scene_cache_updated = 0.0;

    static void invalidate_scene_cache() {
        std::lock_guard<std::mutex> lock(g_scene_cache_mutex);
        g_scene_cache.clear();
        g_scene_cache_updated = 0.0;
    }

    static std::mutex g_scene_registry_mutex;
    static std::unordered_set<uintptr_t> g_scene_registry;
    static bool g_scene_registry_ready = true;

    static void scene_registry_invalidate() {
        std::lock_guard<std::mutex> lock(g_scene_registry_mutex);
        g_scene_registry.clear();
        g_scene_registry_ready = false;
    }

    static void scene_registry_publish(std::unordered_set<uintptr_t>&& fresh) {
        std::lock_guard<std::mutex> lock(g_scene_registry_mutex);
        g_scene_registry.swap(fresh);
        g_scene_registry_ready = true;
    }

    static bool scene_registry_ready() {
        std::lock_guard<std::mutex> lock(g_scene_registry_mutex);
        return g_scene_registry_ready;
    }

    static bool is_protected_unity_object(int scene_handle, int hide_flags) {
        return scene_handle == 0 || (hide_flags & (4 | 16 | 32)) != 0;
    }

    static std::string scene_json() {
        attach_thread();
        {
            std::lock_guard<std::mutex> lock(g_scene_cache_mutex);
            if (!g_scene_cache.empty() && now_seconds() - g_scene_cache_updated < 0.75) {
                LOGD("[Scene] Cache hit bytes=%zu age=%.3f", g_scene_cache.size(), now_seconds() - g_scene_cache_updated);
                return g_scene_cache;
            }
        }

        Il2CppObject* game_object_type = sys_type_of("UnityEngine.GameObject, UnityEngine.CoreModule");
        if (!game_object_type) game_object_type = sys_type_of("UnityEngine.GameObject, UnityEngine");
        if (!game_object_type) game_object_type = sys_type_of("UnityEngine.GameObject");
        if (!game_object_type) {
            LOGE("[Scene] Could not resolve System.Type for UnityEngine.GameObject");
            return "[]";
        }

        MethodBase resources_find_all = find_resources_objects_all_method();
        Array<Il2CppObject*>* all_objects = nullptr;
        if (resources_find_all.IsValid() && method_has_pointer(resources_find_all)) {
            try {
                all_objects = Method<Array<Il2CppObject*>*>(resources_find_all)(game_object_type);
                LOGD("[Scene] Resources.FindObjectsOfTypeAll array=%p count=%zu",
                     all_objects, all_objects && is_valid_pointer(all_objects) ? (size_t)all_objects->capacity : 0);
            } catch(...) {
                LOGE("[Scene] Resources.FindObjectsOfTypeAll invocation failed");
                all_objects = nullptr;
            }
        } else {
            LOGW("[Scene] Resources.FindObjectsOfTypeAll(Type) not resolved");
        }

        bool using_resources = all_objects && is_valid_pointer(all_objects) &&
                               all_objects->capacity > 0 && all_objects->capacity < 250000;
        if (!using_resources) {
            LOGW("[Scene] Resources.FindObjectsOfTypeAll unavailable or empty; falling back to Object.FindObjectsOfType");
        }
        if (!using_resources) {
            all_objects = nullptr;
            MethodBase find_inactive = find_objs_method_inactive();
            if (!find_inactive.IsValid() || !method_has_pointer(find_inactive)) find_inactive = find_objs_method();
            if (find_inactive.IsValid() && method_has_pointer(find_inactive)) {
                try {
                    auto* info = find_inactive.GetInfo();
                    if (info && info->parameters_count == 2)
                        all_objects = Method<Array<Il2CppObject*>*>(find_inactive)(game_object_type, true);
                    else
                        all_objects = Method<Array<Il2CppObject*>*>(find_inactive)(game_object_type);
                } catch(...) {
                    LOGE("[Scene] FindObjectsOfType fallback invocation failed");
                    all_objects = nullptr;
                }
            }
        }

        Method<BNM::Structures::Mono::String*> get_name(Class("UnityEngine", "Object").GetMethod("get_name", 0));
        Method<bool> get_active(Class("UnityEngine", "GameObject").GetMethod("get_activeSelf", 0));
        Method<Il2CppObject*> get_transform(Class("UnityEngine", "GameObject").GetMethod("get_transform", 0));
        Method<Il2CppObject*> get_parent(Class("UnityEngine", "Transform").GetMethod("get_parent", 0));
        Method<Il2CppObject*> get_game_object(Class("UnityEngine", "Component").GetMethod("get_gameObject", 0));
        Method<BNM::UnityEngine::SceneManagement::Scene> get_scene(Class("UnityEngine", "GameObject").GetMethod("get_scene", 0));
        Method<int> get_hide_flags(Class("UnityEngine", "Object").GetMethod("get_hideFlags", 0));

        std::ostringstream json;
        json << "[";
        size_t emitted = 0;
        size_t excluded = 0;
        size_t hidden_without_scene = 0;
        std::unordered_set<uintptr_t> fresh_registry;
        try {
            if (all_objects && is_valid_pointer(all_objects) && all_objects->capacity > 0 && all_objects->capacity < 250000) {
                bool first = true;
                for (size_t i = 0; i < (size_t)all_objects->capacity; ++i) {
                    Il2CppObject* go = all_objects->m_Items[i];
                    if (!go || !is_valid_pointer(go) || !go->klass || !is_valid_pointer(go->klass)) continue;
                    if (!il2cpp_class_has_base(go->klass, "UnityEngine", "GameObject")) continue;
                    if (!reinterpret_cast<BNM::UnityEngine::Object*>(go)->IsValid()) continue;

                    bool scene_valid = false;
                    int hide_flags = 0;
                    if (method_has_pointer(get_scene)) {
                        try {
                            auto scene = get_scene[go]();
                            scene_valid = scene.m_Handle != 0;
                        } catch(...) {}
                    }
                    if (method_has_pointer(get_hide_flags)) {
                        try { hide_flags = get_hide_flags[go](); } catch(...) {}
                    }
                    bool hidden_runtime_object = hide_flags != 0;
                    std::string name = "Unknown";
                    bool active = false;
                    if (method_has_pointer(get_name)) {
                        auto* value = get_name[go]();
                        if (value && is_valid_pointer(value)) name = value->str();
                    }
                    if (method_has_pointer(get_active)) active = get_active[go]();

                    bool diagnostic_name = name.find("BloodMoon") != std::string::npos ||
                                           name.find("Bloodmoon") != std::string::npos ||
                                           name.find("Beam") != std::string::npos ||
                                           name.find("OfflineVRRig") != std::string::npos;
                    if (diagnostic_name) {
                        LOGD("[Scene] Target name=%s addr=%p sceneValid=%d hideFlags=0x%X activeSelf=%d",
                             name.c_str(), go, scene_valid ? 1 : 0, hide_flags, active ? 1 : 0);
                    }
                    if (!scene_valid && !hidden_runtime_object) {
                        if (diagnostic_name)
                            LOGW("[Scene] Target excluded from tree name=%s addr=%p sceneValid=0 hideFlags=0x%X",
                                 name.c_str(), go, hide_flags);
                        ++excluded;
                        continue;
                    }
                    if (!scene_valid) ++hidden_without_scene;

                    uintptr_t parent_addr = 0;
                    if (method_has_pointer(get_transform) && method_has_pointer(get_parent) && method_has_pointer(get_game_object)) {
                        Il2CppObject* transform = get_transform[go]();
                        if (transform && is_valid_pointer(transform)) {
                            Il2CppObject* parent_transform = get_parent[transform]();
                            if (parent_transform && is_valid_pointer(parent_transform)) {
                                Il2CppObject* parent_go = get_game_object[parent_transform]();
                                if (parent_go && is_valid_pointer(parent_go)) parent_addr = (uintptr_t)parent_go;
                            }
                        }
                    }

                    fresh_registry.insert((uintptr_t)go);
                    if (!first) json << ",";
                    first = false;
                    json << "{\"addr\":\"" << std::hex << (uintptr_t)go
                         << "\",\"name\":\"" << js_esc(name)
                         << "\",\"active\":" << (active ? "true" : "false")
                         << ",\"parent\":\"" << std::hex << parent_addr
                         << "\",\"hidden\":" << (!scene_valid ? "true" : "false") << "}";
                    ++emitted;
                }
            }
        } catch(...) {
            LOGE("[Scene] Failed while serializing GameObjects");
        }
        json << "]";
        std::string result = json.str();
        scene_registry_publish(std::move(fresh_registry));
        {
            std::lock_guard<std::mutex> lock(g_scene_cache_mutex);
            g_scene_cache = result;
            g_scene_cache_updated = now_seconds();
        }
        LOGD("[Scene] Enumeration source=%s total=%zu emitted=%zu hiddenNoScene=%zu excluded=%zu bytes=%zu",
             using_resources ? "Resources.FindObjectsOfTypeAll" : "FindObjectsOfType",
             all_objects ? (size_t)all_objects->capacity : 0, emitted, hidden_without_scene, excluded, result.size());
        return result;
    }

    static std::string read_field_raw(const std::string& ft, Class& cls, Il2CppObject* inst, const std::string& name, MethodBase* getter, bool is_static, bool& ok, const Il2CppType* tp = nullptr) {
        ok = false;
        if (!is_static && (!inst || !is_valid_pointer(inst))) return "";
        if (getter) {
            auto* info = getter->GetInfo();
            if (!info || !is_valid_pointer(info) || !info->methodPointer ||
                !is_valid_pointer((const void*)info->methodPointer)) return "";
        }
        try {
            if (ft=="System.Single"||ft=="Single"||ft=="float") {
                float v = 0;
                if (getter) { Method<float> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<float> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="System.Double"||ft=="Double"||ft=="double") {
                double v = 0;
                if (getter) { Method<double> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<double> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="System.Int32"||ft=="Int32"||ft=="int") {
                int v = 0;
                if (getter) { Method<int> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<int> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="System.UInt32"||ft=="UInt32"||ft=="uint") {
                uint32_t v = 0;
                if (getter) { Method<uint32_t> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<uint32_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="System.Int64"||ft=="Int64"||ft=="long") {
                int64_t v = 0;
                if (getter) { Method<int64_t> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<int64_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="System.UInt64"||ft=="UInt64"||ft=="ulong") {
                uint64_t v = 0;
                if (getter) { Method<uint64_t> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<uint64_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="System.Int16"||ft=="Int16"||ft=="short") {
                int16_t v = 0;
                if (getter) { Method<int16_t> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<int16_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="System.UInt16"||ft=="UInt16"||ft=="ushort") {
                uint16_t v = 0;
                if (getter) { Method<uint16_t> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<uint16_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="System.Byte"||ft=="Byte"||ft=="byte") {
                uint8_t v = 0;
                if (getter) { Method<uint8_t> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<uint8_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="System.SByte"||ft=="SByte"||ft=="sbyte") {
                int8_t v = 0;
                if (getter) { Method<int8_t> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<int8_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="System.Boolean"||ft=="Boolean"||ft=="bool") {
                bool v = false;
                if (getter) { Method<bool> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<bool> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return v ? "true" : "false";
            } else if (ft=="System.String"||ft=="String"||ft=="string") {
                BNM::Structures::Mono::String* s = nullptr;
                if (getter) { Method<BNM::Structures::Mono::String*> m(*getter); if(!is_static) m.SetInstance(inst); s = m(); }
                else { Field<BNM::Structures::Mono::String*> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);s=f();} }
                ok = true; return s ? "\"" + js_esc(s->str()) + "\"" : "\"\"";
            } else if (ft=="UnityEngine.Vector3"||ft=="Vector3") {
                Vector3 v = {0,0,0};
                if (getter) { Method<Vector3> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<Vector3> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return "[" + std::to_string(v.x) + "," + std::to_string(v.y) + "," + std::to_string(v.z) + "]";
            } else if (ft=="UnityEngine.Color"||ft=="Color") {
                Color c = {0,0,0,0};
                if (getter) { Method<Color> m(*getter); if(!is_static) m.SetInstance(inst); c = m(); }
                else { Field<Color> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);c=f();} }
                ok = true; return "[" + std::to_string(c.r) + "," + std::to_string(c.g) + "," + std::to_string(c.b) + "," + std::to_string(c.a) + "]";
            } else if (ft=="UnityEngine.Vector2"||ft=="Vector2") {
                Vector2 v = {0,0};
                if (getter) { Method<Vector2> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<Vector2> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return "[" + std::to_string(v.x) + "," + std::to_string(v.y) + "]";
            } else if (ft=="UnityEngine.Vector4"||ft=="Vector4"||ft=="UnityEngine.Quaternion"||ft=="Quaternion") {
                Vector4 v = {0,0,0,0};
                if (getter) { Method<Vector4> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<Vector4> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::string("{\"x\":") + std::to_string(v.x) + ",\"y\":" + std::to_string(v.y) + ",\"z\":" + std::to_string(v.z) + ",\"w\":" + std::to_string(v.w) + "}";
            } else if (ft=="UnityEngine.LayerMask"||ft=="LayerMask") {
                int v = 0;
                if (getter) { Method<int> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<int> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return std::to_string(v);
            } else if (ft=="UnityEngine.Rect"||ft=="Rect") {
                Rect v = {0,0,0,0};
                if (getter) { Method<Rect> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<Rect> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true;
                return std::string("{\"x\":") + std::to_string(v.x) + ",\"y\":" + std::to_string(v.y) + ",\"w\":" + std::to_string(v.w) + ",\"h\":" + std::to_string(v.h) + "}";
            } else if (ft=="UnityEngine.Vector3Int"||ft=="Vector3Int") {
                vector3_int v = {0,0,0};
                if (getter) { Method<vector3_int> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<vector3_int> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return "[" + std::to_string(v.x) + "," + std::to_string(v.y) + "," + std::to_string(v.z) + "]";
            } else if (ft=="UnityEngine.Vector2Int"||ft=="Vector2Int") {
                vector2_int v = {0,0};
                if (getter) { Method<vector2_int> m(*getter); if(!is_static) m.SetInstance(inst); v = m(); }
                else { Field<vector2_int> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);v=f();} }
                ok = true; return "[" + std::to_string(v.x) + "," + std::to_string(v.y) + "]";
            } else {
                if (tp && is_valid_pointer(tp) && bnm_type_is_value_type(tp)) {
                    Class tc(tp);
                    auto* tk = tc.IsValid() ? tc.GetClass() : nullptr;
                    if (tk && tk->enumtype) {
                        int ev = 0;
                        if (getter) { Method<int> m(*getter); if(!is_static) m.SetInstance(inst); ev = m(); }
                        else { Field<int> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);ev=f();} }
                        ok = true;
                        return std::to_string(ev);
                    }
                    return "";
                }
                Il2CppObject* obj_val = nullptr;
                if (getter) {
                    Method<Il2CppObject*> m(*getter);
                    if (!is_static) m.SetInstance(inst);
                    obj_val = m();
                } else {
                    Field<Il2CppObject*> f = cls.GetField(name.c_str());
                    if (f.IsValid()) {
                        if (!is_static) f.SetInstance(inst);
                        obj_val = f();
                    }
                }
                ok = true;
                if (!obj_val || !is_valid_pointer(obj_val)) return "\"0x0\"";
                return "\"" + js_esc(object_ref_string(obj_val)) + "\"";
            }
        } catch(...) {}
        return "";
    }

    static std::string sanitize_json_numbers(const std::string& in) {
        std::string out;
        out.reserve(in.size());
        bool in_str = false;
        for (size_t i = 0; i < in.size(); i++) {
            char c = in[i];
            if (in_str) {
                out += c;
                if (c == '\\' && i + 1 < in.size()) out += in[++i];
                else if (c == '"') in_str = false;
                continue;
            }
            if (c == '"') {
                in_str = true;
                out += c;
                continue;
            }
            if (in.compare(i, 4, "-nan") == 0) { out += '0'; i += 3; continue; }
            if (in.compare(i, 4, "-inf") == 0) { out += '0'; i += 3; continue; }
            if (in.compare(i, 3, "nan") == 0) { out += '0'; i += 2; continue; }
            if (in.compare(i, 3, "inf") == 0) { out += '0'; i += 2; continue; }
            if (c == ',' && i + 1 < in.size() && in[i + 1] >= '0' && in[i + 1] <= '9' && i > 0 && in[i - 1] >= '0' && in[i - 1] <= '9' && !out.empty() && out.find('.') == std::string::npos && in.find('[') == std::string::npos && in.find('{') == std::string::npos) {
                out += '.';
                continue;
            }
            out += c;
        }
        return out;
    }

    static std::string read_field(const std::string& ft, Class& cls, Il2CppObject* inst, const std::string& name, MethodBase* getter, bool is_static, bool& ok, const Il2CppType* tp = nullptr) {
        return sanitize_json_numbers(read_field_raw(ft, cls, inst, name, getter, is_static, ok, tp));
    }

    static void write_field(const std::string& ft, Class& cls, Il2CppObject* inst, const std::string& name, MethodBase* setter, const std::string& v, bool is_static = false) {
        if (!is_static && (!inst || !is_valid_pointer(inst))) return;
        if (setter) {
            auto* info = setter->GetInfo();
            if (!info || !is_valid_pointer(info) || !info->methodPointer ||
                !is_valid_pointer((const void*)info->methodPointer)) return;
        }
        try {
            if (ft=="System.Single"||ft=="Single"||ft=="float") {
                float val = std::stof(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<float> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.Double"||ft=="Double"||ft=="double") {
                double val = std::stod(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<double> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.Int32"||ft=="Int32"||ft=="int") {
                int val = std::stoi(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<int> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.UInt32"||ft=="UInt32"||ft=="uint") {
                uint32_t val = (uint32_t)std::stoul(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<uint32_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.Int64"||ft=="Int64"||ft=="long") {
                int64_t val = std::stoll(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<int64_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.UInt64"||ft=="UInt64"||ft=="ulong") {
                uint64_t val = std::stoull(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<uint64_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.Int16"||ft=="Int16"||ft=="short") {
                int16_t val = (int16_t)std::stoi(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<int16_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.UInt16"||ft=="UInt16"||ft=="ushort") {
                uint16_t val = (uint16_t)std::stoul(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<uint16_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.Byte"||ft=="Byte"||ft=="byte") {
                uint8_t val = (uint8_t)std::stoul(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<uint8_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.SByte"||ft=="SByte"||ft=="sbyte") {
                int8_t val = (int8_t)std::stoi(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<int8_t> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.Boolean"||ft=="Boolean"||ft=="bool") {
                bool val = v=="true"||v=="1";
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<bool> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="System.String"||ft=="String"||ft=="string") {
                auto* s = CreateMonoString(v.c_str());
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(s); }
                else { Field<BNM::Structures::Mono::String*> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=s;} }
            } else if (ft=="UnityEngine.Vector3"||ft=="Vector3") {
                float x=0,y=0,z=0; sscanf(v.c_str(),"[%f,%f,%f]",&x,&y,&z); Vector3 val={x,y,z};
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<Vector3> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="UnityEngine.Color"||ft=="Color") {
                float r=1,g=1,b=1,a=1; sscanf(v.c_str(),"[%f,%f,%f,%f]",&r,&g,&b,&a); Color val={r,g,b,a};
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<Color> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="UnityEngine.Vector2"||ft=="Vector2") {
                float x=0,y=0; sscanf(v.c_str(),"[%f,%f]",&x,&y); Vector2 val={x,y};
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<Vector2> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="UnityEngine.Vector4"||ft=="Vector4"||ft=="UnityEngine.Quaternion"||ft=="Quaternion") {
                float x=0,y=0,z=0,w=0;
                sscanf(v.c_str(),"{\"x\":%f,\"y\":%f,\"z\":%f,\"w\":%f}",&x,&y,&z,&w);
                Vector4 val={x,y,z,w};
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<Vector4> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="UnityEngine.LayerMask"||ft=="LayerMask") {
                int val = std::stoi(v);
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<int> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="UnityEngine.Rect"||ft=="Rect") {
                float x=0,y=0,w=0,h=0;
                sscanf(v.c_str(),"{\"x\":%f,\"y\":%f,\"w\":%f,\"h\":%f}",&x,&y,&w,&h);
                Rect val={x,y,w,h};
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<Rect> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="UnityEngine.Vector3Int"||ft=="Vector3Int") {
                int x=0,y=0,z=0; sscanf(v.c_str(),"[%d,%d,%d]",&x,&y,&z); vector3_int val={x,y,z};
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<vector3_int> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else if (ft=="UnityEngine.Vector2Int"||ft=="Vector2Int") {
                int x=0,y=0; sscanf(v.c_str(),"[%d,%d]",&x,&y); vector2_int val={x,y};
                if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                else { Field<vector2_int> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
            } else {
                const Il2CppType* ptp = nullptr;
                if (setter) {
                    auto* smi = setter->GetInfo();
                    if (smi) ptp = get_param_type(smi, 0);
                } else {
                    Field<Il2CppObject*> tf = cls.GetField(name.c_str());
                    if (tf.IsValid()) { auto* tfi = tf.GetInfo(); if (tfi) ptp = tfi->type; }
                }
                if (ptp && is_valid_pointer(ptp) && bnm_type_is_value_type(ptp)) {
                    Class tc(ptp);
                    auto* tk = tc.IsValid() ? tc.GetClass() : nullptr;
                    if (!tk || !tk->enumtype) return;
                    int val = std::stoi(v.empty() ? "0" : v);
                    if (setter) { Method<void> m(*setter); if(!is_static) m.SetInstance(inst); m(val); }
                    else { Field<int> f=cls.GetField(name.c_str()); if(f.IsValid()){if(!is_static)f.SetInstance(inst);f=val;} }
                    return;
                }
                uintptr_t ptr = (v == "null" || v.empty()) ? 0 : (uintptr_t)strtoull(v.c_str(), nullptr, 16);
                if (ptr && !is_valid_object((Il2CppObject*)ptr)) return;
                Il2CppObject* obj_val = (Il2CppObject*)ptr;
                if (setter) {
                    Method<void> m(*setter);
                    if (!is_static) m.SetInstance(inst);
                    m(obj_val);
                } else {
                    Field<Il2CppObject*> f = cls.GetField(name.c_str());
                    if (f.IsValid()) {
                        if (!is_static) f.SetInstance(inst);
                        f = obj_val;
                    }
                }
            }
        } catch(...) {}
    }

    static bool prop_read_allowed(const MethodInfo* getter, const std::string& type) {
        if (!getter || getter->parameters_count != 0 || !getter->methodPointer ||
            !is_valid_pointer((const void*)getter->methodPointer)) return false;
        static const char* allowed[] = {
                "System.Boolean", "System.Int32", "System.Single", "System.Double", "System.Int64", "System.UInt32",
                "System.UInt64", "System.Int16", "System.UInt16", "System.Byte", "System.SByte", "System.String",
                "UnityEngine.Vector2", "UnityEngine.Vector3", "UnityEngine.Vector4", "UnityEngine.Quaternion",
                "UnityEngine.Color", "UnityEngine.Rect", "UnityEngine.LayerMask", nullptr
        };
        for (auto** a = allowed; *a; a++)
            if (type == *a) return true;
        return false;
    }

    static std::mutex g_component_index_mutex;
    static std::unordered_map<uintptr_t, std::vector<Il2CppObject*>> g_component_index;
    static double g_component_index_updated = 0.0;

    static void invalidate_component_index() {
        std::lock_guard<std::mutex> lock(g_component_index_mutex);
        g_component_index.clear();
        g_component_index_updated = 0.0;
    }

    static bool get_cached_components_for_gameobject(Il2CppObject* go, Il2CppObject* component_type,
                                                     Class& component_cls, std::vector<Il2CppObject*>& out) {
        if (!go || !is_valid_pointer(go) || !component_type || !is_valid_pointer(component_type)) return false;
        double now = now_seconds();
        {
            std::lock_guard<std::mutex> lock(g_component_index_mutex);
            if (g_component_index_updated > 0.0 && now - g_component_index_updated < 8.0) {
                auto it = g_component_index.find((uintptr_t)go);
                if (it != g_component_index.end()) {
                    for (auto* component : it->second) {
                        if (component && is_valid_pointer(component) && is_object_alive(component))
                            out.push_back(component);
                    }
                }
                LOGD("[Inspector] Component index cache hit owner=%p count=%zu age=%.2f",
                     go, out.size(), now - g_component_index_updated);
                return true;
            }
        }

        MethodBase find_all_method = find_objs_method_inactive();
        bool includes_inactive = find_all_method.IsValid();
        if (!includes_inactive) find_all_method = find_objs_method();
        MethodBase get_game_object_base = component_cls.GetMethod("get_gameObject", 0);
        auto* get_game_object_info = get_game_object_base.GetInfo();
        if (!find_all_method.IsValid() || !get_game_object_info || !get_game_object_info->methodPointer ||
            !is_valid_pointer((const void*)get_game_object_info->methodPointer)) return false;
        Method<Il2CppObject*> get_component_game_object(get_game_object_base);

        Array<Il2CppObject*>* all_components = nullptr;
        try {
            if (includes_inactive)
                all_components = Method<Array<Il2CppObject*>*>(find_all_method)(component_type, true);
            else
                all_components = Method<Array<Il2CppObject*>*>(find_all_method)(component_type);
        } catch(...) {
            LOGE("[Inspector] Component index enumeration failed");
            return false;
        }
        if (!all_components || !is_valid_pointer(all_components) || all_components->capacity >= 250000) {
            LOGE("[Inspector] Component index returned no usable array");
            return false;
        }

        std::unordered_map<uintptr_t, std::vector<Il2CppObject*>> fresh;
        fresh.reserve((size_t)all_components->capacity / 2 + 1);
        size_t mapped = 0;
        for (size_t i = 0; i < (size_t)all_components->capacity; ++i) {
            Il2CppObject* component = all_components->m_Items[i];
            if (!component || !is_valid_pointer(component)) continue;
            try {
                Il2CppObject* owner = get_component_game_object[component]();
                if (!owner || !is_valid_pointer(owner)) continue;
                fresh[(uintptr_t)owner].push_back(component);
                mapped++;
            } catch(...) {}
        }

        {
            std::lock_guard<std::mutex> lock(g_component_index_mutex);
            g_component_index.swap(fresh);
            g_component_index_updated = now_seconds();
            auto it = g_component_index.find((uintptr_t)go);
            if (it != g_component_index.end()) {
                for (auto* component : it->second) {
                    if (component && is_valid_pointer(component) && is_object_alive(component))
                        out.push_back(component);
                }
            }
            LOGD("[Inspector] Component index refreshed array=%p count=%zu owners=%zu mapped=%zu inactive=%d selected=%zu",
                 all_components, (size_t)all_components->capacity, g_component_index.size(), mapped,
                 includes_inactive ? 1 : 0, out.size());
        }
        return true;
    }

    static std::string go_info_json(uintptr_t addr) {
        attach_thread();
        if (!addr || !is_valid_pointer((void*)addr)) return "{\"stale\":true}";
        Il2CppObject* go = (Il2CppObject*)addr;
        if (!scene_registry_ready()) return "{\"stale\":true}";
        if (!is_object_alive(go) || !il2cpp_class_has_base(go->klass, "UnityEngine", "GameObject"))
            return "{\"stale\":true}";
        std::string name = "Unknown";
        bool active = false;

        Class go_cls("UnityEngine","GameObject");
        Method<BNM::Structures::Mono::String*> gn(Class("UnityEngine","Object").GetMethod("get_name"));
        Method<bool> ga(go_cls.GetMethod("get_activeSelf"));
        Method<Il2CppObject*> gt(go_cls.GetMethod("get_transform"));

        try {
            if (method_has_pointer(gn)) { auto* s=gn[go](); if(s) name=s->str(); }
            if (method_has_pointer(ga)) active = ga[go]();
        } catch(...) {}
        LOGD("[Inspector] Open GameObject name=%s addr=%p activeSelf=%d", name.c_str(), (void*)go, active ? 1 : 0);

        std::ostringstream j;
        j << "{\"addr\":\"" << std::hex << addr << "\",\"name\":\"" << js_esc(name) << "\",\"active\":" << (active?"true":"false") << ",\"transform\":{";
        try {
            if (method_has_pointer(gt)) {
                Il2CppObject* tr = gt[go]();
                if (tr && is_valid_pointer(tr) && is_object_alive(tr)) {
                    Method<Vector3> gp(Class("UnityEngine","Transform").GetMethod("get_localPosition"));
                    Method<Vector3> gr(Class("UnityEngine","Transform").GetMethod("get_localEulerAngles"));
                    Method<Vector3> gs(Class("UnityEngine","Transform").GetMethod("get_localScale"));
                    Vector3 p=method_has_pointer(gp)?gp[tr]():Vector3{0,0,0};
                    Vector3 r=method_has_pointer(gr)?gr[tr]():Vector3{0,0,0};
                    Vector3 s=method_has_pointer(gs)?gs[tr]():Vector3{0,0,0};
                    j << "\"addr\":\"" << std::hex << (uintptr_t)tr << "\",\"p\":[" << p.x << "," << p.y << "," << p.z << "],\"r\":[" << r.x << "," << r.y << "," << r.z << "],\"s\":[" << s.x << "," << s.y << "," << s.z << "]";
                }
            }
        } catch(...) {}
        j << "},\"scripts\":[";

        try {
            Class component_cls("UnityEngine", "Component");
            auto* component_mono_type = component_cls.IsValid() ? component_cls.GetMonoType() : nullptr;
            Il2CppObject* component_type = reinterpret_cast<Il2CppObject*>(component_mono_type);
            if (!component_type) component_type = sys_type_of("UnityEngine.Component, UnityEngine.CoreModule");
            if (!component_type) component_type = sys_type_of("UnityEngine.Component");

            std::vector<Il2CppObject*> components;
            Array<Il2CppObject*>* component_array = nullptr;
            MethodBase public_get_components;
            for (auto& candidate : go_cls.GetMethods(true)) {
                auto* info = candidate.GetInfo();
                if (!info || !is_valid_pointer(info) || !info->name || !is_valid_pointer(info->name) ||
                    strcmp(info->name, "GetComponents") != 0 || info->parameters_count != 1 ||
                    info->is_generic || !method_has_pointer(candidate)) continue;
                std::string parameter_type = get_param_type_name(info, 0);
                if (parameter_type == "System.Type" || parameter_type == "Type" ||
                    parameter_type.find("System.Type") != std::string::npos) {
                    public_get_components = candidate;
                    LOGD("[Inspector] Exact GetComponents(Type) found metadata=%p pointer=%p parameter=%s",
                         info, (void*)info->methodPointer, parameter_type.c_str());
                    break;
                }
            }

            if (public_get_components.IsValid() && component_type) {
                try {
                    component_array = Method<Array<Il2CppObject*>*>(public_get_components)[go](component_type);
                } catch(...) {
                    LOGE("[Inspector] GetComponents(Type) invocation threw for GameObject=%p", go);
                }
            } else {
                LOGD("[Inspector] Exact GetComponents(Type) unavailable; trying verified internal overload");
            }

            if ((!component_array || !is_valid_pointer(component_array) || component_array->capacity == 0) && component_type) {
                MethodBase internal_get_components;
                for (auto& mt : go_cls.GetMethods(true)) {
                    auto* mi = mt.GetInfo();
                    if (!mi || !is_valid_pointer(mi) || !mi->name || !is_valid_pointer(mi->name)) continue;
                    if (strcmp(mi->name, "GetComponentsInternal") != 0 || mi->parameters_count != 6) continue;
                    if (!method_has_pointer(mt)) {
                        LOGD("[Inspector] GetComponentsInternal candidate has no method pointer");
                        continue;
                    }
                    std::string p0 = get_param_type_name(mi, 0);
                    std::string p1 = get_param_type_name(mi, 1);
                    std::string p2 = get_param_type_name(mi, 2);
                    std::string p3 = get_param_type_name(mi, 3);
                    std::string p4 = get_param_type_name(mi, 4);
                    std::string p5 = get_param_type_name(mi, 5);
                    auto is_bool_type = [](const std::string& value) {
                        return value == "System.Boolean" || value == "Boolean";
                    };
                    bool signature_ok =
                            (p0 == "System.Type" || p0 == "Type" || p0.find("System.Type") != std::string::npos) &&
                            is_bool_type(p1) && is_bool_type(p2) && is_bool_type(p3) && is_bool_type(p4) &&
                            (p5 == "System.Object" || p5 == "Object" || p5.find("System.Object") != std::string::npos);
                    LOGD("[Inspector] GetComponentsInternal candidate signature=(%s,%s,%s,%s,%s,%s) valid=%d",
                         p0.c_str(), p1.c_str(), p2.c_str(), p3.c_str(), p4.c_str(), p5.c_str(), signature_ok ? 1 : 0);
                    if (signature_ok) {
                        internal_get_components = mt;
                        break;
                    }
                }

                if (internal_get_components.IsValid()) {
                    try {
                        component_array = Method<Array<Il2CppObject*>*>(internal_get_components)[go](
                                component_type, false, false, true, false, nullptr);
                    } catch(...) {
                        LOGE("[Inspector] GetComponentsInternal invocation failed for GameObject=%p", go);
                    }
                }
            }

            if (component_array && is_valid_pointer(component_array) && component_array->capacity < 65536) {
                LOGD("[Inspector] Local component array=%p count=%zu", component_array, (size_t)component_array->capacity);
                for (size_t i = 0; i < (size_t)component_array->capacity; ++i) {
                    Il2CppObject* component = component_array->m_Items[i];
                    if (!component || !is_valid_pointer(component) || !is_object_alive(component)) continue;
                    if (std::find(components.begin(), components.end(), component) == components.end())
                        components.push_back(component);
                }
            } else {
                LOGD("[Inspector] Local component retrieval returned %s",
                     component_array ? "invalid or oversized array" : "null");
            }

            if (components.empty() && component_type) {
                std::vector<Il2CppObject*> indexed_components;
                if (get_cached_components_for_gameobject(go, component_type, component_cls, indexed_components)) {
                    for (auto* component : indexed_components) {
                        if (std::find(components.begin(), components.end(), component) == components.end())
                            components.push_back(component);
                    }
                }
            }

            LOGD("[Inspector] GameObject=%p final component count=%zu", go, components.size());

            static const char* k_stop_at_component[] = {
                    "Component", "Object", nullptr
            };
            static const char* k_skip_props[] = {
                    "isActiveAndEnabled", "transform", "gameObject", "tag", "name",
                    "hideFlags", "rigidbody", "rigidbody2D", "camera", "light",
                    "animation", "constantForce", "renderer", "audio", "networkView",
                    "collider", "collider2D", "hingeJoint", "particleSystem", nullptr
            };

            auto is_stop_class = [](const char* n) -> bool {
                for (auto** s = k_stop_at_component; *s; s++)
                    if (strcmp(n, *s) == 0) return true;
                return false;
            };
            auto is_skip_prop = [](const char* n) -> bool {
                for (auto** s = k_skip_props; *s; s++)
                    if (strcmp(n, *s) == 0) return true;
                return false;
            };

            size_t serialized_component_count = 0;
            size_t skipped_transform_count = 0;
            size_t invalid_component_count = 0;
            if (!components.empty()) {
                bool first_script = true;

                for (Il2CppObject* comp : components) {
                    if (!comp || !is_valid_pointer(comp) || !comp->klass || !is_valid_pointer(comp->klass)) {
                        ++invalid_component_count;
                        continue;
                    }

                    auto* klass = comp->klass;
                    LOGD("[Inspector] component candidate=%p class=%s namespace=%s", comp,
                         klass->name && is_valid_pointer(klass->name) ? klass->name : "<invalid>",
                         klass->namespaze && is_valid_pointer(klass->namespaze) ? klass->namespaze : "");
                    bool is_component = il2cpp_class_has_base(klass, "UnityEngine", "Component");
                    bool is_mono_behaviour = il2cpp_class_has_base(klass, "UnityEngine", "MonoBehaviour");
                    bool is_transform = klass->name && is_valid_pointer(klass->name) && strcmp(klass->name, "Transform") == 0 &&
                                        klass->namespaze && is_valid_pointer(klass->namespaze) &&
                                        strcmp(klass->namespaze, "UnityEngine") == 0;

                    if (!is_component) {
                        LOGW("[Inspector] Component[] item does not expose Component base; retaining item=%p class=%s namespace=%s",
                             comp,
                             klass->name && is_valid_pointer(klass->name) ? klass->name : "<invalid>",
                             klass->namespaze && is_valid_pointer(klass->namespaze) ? klass->namespaze : "");
                    }
                    if (is_transform) {
                        ++skipped_transform_count;
                        continue;
                    }

                    std::string comp_category = is_mono_behaviour ? "script" : "builtin";
                    std::string comp_name = (klass->name && is_valid_pointer(klass->name)) ? klass->name : "Unknown";
                    std::string comp_ns = (klass->namespaze && is_valid_pointer(klass->namespaze)) ? klass->namespaze : "";
                    char comp_addr[32]; snprintf(comp_addr, sizeof(comp_addr), "%llX", (unsigned long long)(uintptr_t)comp);
                    std::string comp_asm = (klass->image && is_valid_pointer(klass->image) && klass->image->name && is_valid_pointer(klass->image->name)) ? klass->image->name : "";

                    bool has_enabled_prop = false;
                    bool enabled = true;
                    MethodBase enabled_getter;
                    for (auto* cur = klass; cur && is_valid_pointer(cur) && !enabled_getter.IsValid(); cur = cur->parent) {
                        try {
                            Class current_class(cur);
                            for (auto& property : current_class.GetProperties(false)) {
                                auto* property_info = property._data;
                                if (!property_info || !is_valid_pointer(property_info) || !property_info->name ||
                                    !is_valid_pointer(property_info->name) || strcmp(property_info->name, "enabled") != 0 ||
                                    !property_info->get || !is_valid_pointer(property_info->get) ||
                                    property_info->get->parameters_count != 0)
                                    continue;
                                std::string return_type = type_name(property_info->get->return_type);
                                if (return_type != "System.Boolean" && return_type != "Boolean") continue;
                                enabled_getter = MethodBase(property_info->get);
                                break;
                            }
                        } catch(...) {}
                    }

                    if (enabled_getter.IsValid() && method_has_pointer(enabled_getter) && !enabled_getter._isStatic) {
                        has_enabled_prop = true;
                        try {
                            Method<bool> get_enabled(enabled_getter);
                            get_enabled.SetInstance(comp);
                            enabled = get_enabled();
                        } catch(...) {
                            LOGD("[Inspector] Could not read enabled property for component=%p", comp);
                        }
                    }

                    if (!first_script) j << ",";
                    first_script = false;
                    j << "{\"addr\":\"" << comp_addr << "\",\"name\":\"" << js_esc(comp_name) << "\",\"ns\":\"" << js_esc(comp_ns) << "\",\"asm\":\"" << js_esc(comp_asm) << "\",\"category\":\"" << comp_category << "\",\"enabled\":" << (enabled?"true":"false") << ",\"hasEnabled\":" << (has_enabled_prop?"true":"false") << ",\"fields\":[";

                    try {
                        Class comp_cls(klass);
                        bool first_f = true;
                        std::vector<std::string> seen;

                        for (Class cur = comp_cls; cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
                            auto* cn_ = cur.GetClass()->name;
                            if (!cn_ || !is_valid_pointer(cn_)) break;
                            if (is_stop_class(cn_)) break;

                            for (auto& f : cur.GetFields(false)) {
                                try {
                                    auto* fi = f.GetInfo();
                                    if (!fi || !is_valid_pointer(fi) || !fi->type || !is_valid_pointer(fi->type) || (fi->type->attrs & 0x10)) continue;
                                    std::string fn = fi->name;
                                    bool dup = false; for (auto& seen_name : seen) if (seen_name==fn) dup=true;
                                    if (dup) continue; seen.push_back(fn);
                                    std::string ft = type_name(fi->type);
                                    bool ok = false;
                                    std::string vs = read_field(ft, cur, comp, fn, nullptr, false, ok, fi->type);
                                    if (ok) {
                                        if (!first_f) j << ","; first_f = false;
                                        j << "{\"name\":\"" << js_esc(fn) << "\",\"type\":\"" << js_esc(ft) << "\",\"val\":" << vs << ",\"isProp\":false,\"canWrite\":true}";
                                    }
                                } catch(...) {}
                            }

                            for (auto& property : cur.GetProperties(false)) {
                                try {
                                    auto* pi = property._data;
                                    if (!pi || !is_valid_pointer(pi) || !pi->get) continue;
                                    std::string pn = pi->name;
                                    if (is_skip_prop(pn.c_str())) continue;
                                    if (pn == "Item" || pn == "Chars") continue;
                                    bool dup = false; for (auto& seen_name : seen) if (seen_name==pn) dup=true;
                                    if (dup) continue; seen.push_back(pn);
                                    std::string pt = (pi->get->return_type && is_valid_pointer(pi->get->return_type)) ? type_name(pi->get->return_type) : "?";
                                    if (!prop_read_allowed(pi->get, pt)) continue;
                                    bool ok = false;
                                    MethodBase getter(pi->get);
                                    bool getter_static = getter._isStatic;
                                    std::string vs = read_field(pt, cur, getter_static ? nullptr : comp, pn, &getter, getter_static, ok, pi->get->return_type);
                                    if (ok) {
                                        if (!first_f) j << ","; first_f = false;
                                        j << "{\"name\":\"" << js_esc(pn) << "\",\"type\":\"" << js_esc(pt) << "\",\"val\":" << vs << ",\"isProp\":true,\"canWrite\":" << (pi->set?"true":"false") << "}";
                                    }
                                } catch(...) {}
                            }
                        }
                    } catch(...) {}
                    j << "]}";
                    ++serialized_component_count;
                }
            }
            LOGD("[Inspector] Serialization summary GameObject=%p candidates=%zu serialized=%zu transforms=%zu invalid=%zu",
                 go, components.size(), serialized_component_count, skipped_transform_count, invalid_component_count);
        } catch(...) {
            LOGE("[Inspector] Unexpected failure while building component list for GameObject=%p", go);
        }
        j << "]}";
        std::string result = j.str();
        result = sanitize_json_numbers(result);
        LOGD("[Inspector] JSON ready GameObject=%p bytes=%zu", go, result.size());
        return result;
    }

    static void apply_loop_entry(const std::shared_ptr<loop_entry>& e) {
        try {
            uintptr_t addr = (uintptr_t)strtoull(e->addr.c_str(), nullptr, 16);
            if (!addr || !is_valid_pointer((void*)addr)) return;
            Il2CppObject* obj = resolve_component_by_addr(addr);
            if (!obj) { e->active = false; return; }
            for (Class cur(e->ns.c_str(), e->cls.c_str(), Image(e->asm_.c_str())); cur.IsValid(); cur = cur.GetParent()) {
                if (e->is_prop) {
                    auto p = cur.GetProperty(e->name.c_str());
                    if (p.IsValid() && p._data && p._data->set) {
                        MethodBase s(p._data->set);
                        write_field(e->ftype, cur, obj, e->name, &s, e->val);
                        return;
                    }
                } else {
                    auto f = cur.GetField(e->name.c_str());
                    if (f.IsValid()) { write_field(e->ftype, cur, obj, e->name, nullptr, e->val); return; }
                }
            }
        } catch(...) {}
    }

    static std::string loop_list_json() {
        std::lock_guard<std::mutex> lk(g_loop_mtx);
        std::ostringstream j;
        j << "[";
        bool first = true;
        for (auto& kv : g_loops) {
            if (!first) j << ","; first = false;
            auto& e = kv.second;
            j << "{\"id\":\"" << js_esc(kv.first) << "\",\"cls\":\"" << js_esc(e->cls) << "\",\"name\":\"" << js_esc(e->name) << "\",\"val\":\"" << js_esc(e->val) << "\",\"interval\":" << e->interval_ms << ",\"active\":" << (e->active?"true":"false") << "}";
        }
        j << "]";
        return j.str();
    }

#define MAX_CATCHERS 64
    static std::shared_ptr<catcher_entry>* g_catcher_slots[MAX_CATCHERS] = {};
    static int g_catcher_slot_count = 0;
    static std::mutex g_catcher_slot_mtx;
    static void (*g_catcher_orig_ptrs[MAX_CATCHERS])(Il2CppObject*) = {};

    template<int N>
    static void catcher_hook(Il2CppObject* self) {
        std::shared_ptr<catcher_entry>* slot = g_catcher_slots[N];
        if (slot && *slot && (*slot)->active) {
            catcher_call call;
            call.ts = now_seconds();
            char buf[32]; snprintf(buf, sizeof(buf), "0x%llX", (unsigned long long)(uintptr_t)self);
            call.instance = buf;
            if (self && is_valid_pointer(self) && self->klass && is_valid_pointer(self->klass)) {
                if (is_unity_object_instance(self)) {
                    try {
                        Method<BNM::Structures::Mono::String*> gn(Class("UnityEngine","Object").GetMethod("get_name"));
                        if (gn.IsValid()) { auto* s = gn[self](); if (s) call.instance += " (" + s->str() + ")"; }
                    } catch(...) {}
                }
            }
            std::lock_guard<std::mutex> lk((*slot)->calls_mtx);
            if ((*slot)->calls.size() >= catcher_entry::max_calls) (*slot)->calls.erase((*slot)->calls.begin());
            (*slot)->calls.push_back(std::move(call));
        }
        if (g_catcher_orig_ptrs[N]) g_catcher_orig_ptrs[N](self);
    }

    typedef void(*CatcherHookFn)(Il2CppObject*);
    static CatcherHookFn g_catcher_hook_fns[MAX_CATCHERS] = {
            catcher_hook<0>,catcher_hook<1>,catcher_hook<2>,catcher_hook<3>,catcher_hook<4>,
            catcher_hook<5>,catcher_hook<6>,catcher_hook<7>,catcher_hook<8>,catcher_hook<9>,
            catcher_hook<10>,catcher_hook<11>,catcher_hook<12>,catcher_hook<13>,catcher_hook<14>,
            catcher_hook<15>,catcher_hook<16>,catcher_hook<17>,catcher_hook<18>,catcher_hook<19>,
            catcher_hook<20>,catcher_hook<21>,catcher_hook<22>,catcher_hook<23>,catcher_hook<24>,
            catcher_hook<25>,catcher_hook<26>,catcher_hook<27>,catcher_hook<28>,catcher_hook<29>,
            catcher_hook<30>,catcher_hook<31>,catcher_hook<32>,catcher_hook<33>,catcher_hook<34>,
            catcher_hook<35>,catcher_hook<36>,catcher_hook<37>,catcher_hook<38>,catcher_hook<39>,
            catcher_hook<40>,catcher_hook<41>,catcher_hook<42>,catcher_hook<43>,catcher_hook<44>,
            catcher_hook<45>,catcher_hook<46>,catcher_hook<47>,catcher_hook<48>,catcher_hook<49>,
            catcher_hook<50>,catcher_hook<51>,catcher_hook<52>,catcher_hook<53>,catcher_hook<54>,
            catcher_hook<55>,catcher_hook<56>,catcher_hook<57>,catcher_hook<58>,catcher_hook<59>,
            catcher_hook<60>,catcher_hook<61>,catcher_hook<62>,catcher_hook<63>
    };


#define BNM_APP_NAMESPACE_OPEN
#include "explorer/main_thread.hpp"
#include "explorer/scene_dump.hpp"
#include "explorer/calls.hpp"
#include "explorer/freeze.hpp"
#undef BNM_APP_NAMESPACE_OPEN

    static void start_server() {
        httplib::Server svr;

        svr.Get("/",[](const httplib::Request&, httplib::Response& res) {
            attach_thread();
            int n = 0;
            for (auto& img : Image::GetImages()) if (img.IsValid()) n++;
            res.set_content(get_explorer_html(n), "text/html");
        });

        svr.Get("/api/assemblies",[](const httplib::Request&, httplib::Response& res) {
            res.set_content(assemblies_json(), "application/json");
        });

        svr.Get("/api/classes",[](const httplib::Request& req, httplib::Response& res) {
            res.set_content(classes_json(req.get_param_value("a")), "application/json");
        });

        svr.Get("/api/allclasses",[](const httplib::Request&, httplib::Response& res) {
            res.set_content(all_classes_json(), "application/json");
        });

        svr.Get("/api/class",[](const httplib::Request& req, httplib::Response& res) {
            res.set_content(class_detail_json(req.get_param_value("a"), req.get_param_value("ns"), req.get_param_value("n")), "application/json");
        });

        svr.Get("/api/instances",[](const httplib::Request& req, httplib::Response& res) {
            res.set_content(instances_json(req.get_param_value("a"), req.get_param_value("ns"), req.get_param_value("n")), "application/json");
        });

        svr.Get("/api/find_instances",[](const httplib::Request& req, httplib::Response& res) {
            std::string t = req.has_param("type") ? req.get_param_value("type") : "";
            res.set_content(find_instances_json(t), "application/json");
        });

        svr.Get("/api/collection",[](const httplib::Request& req, httplib::Response& res) {
            std::string s = req.get_param_value("addr");
            res.set_content(s.empty() ? "{\"ok\":false}" : collection_json((uintptr_t)strtoull(s.c_str(), nullptr, 16)), "application/json");
        });

        svr.Get("/api/object/info",[](const httplib::Request& req, httplib::Response& res) {
            std::string s = req.get_param_value("addr");
            res.set_content(s.empty() ? "{\"ok\":false}" : object_info_json((uintptr_t)strtoull(s.c_str(), nullptr, 16)), "application/json");
        });

        svr.Get("/api/scene",[](const httplib::Request&, httplib::Response& res) {
            if (!mt_on_main_thread() && !mt_install()) {
                res.status = 503;
                res.set_content("{\"error\":\"main thread dispatcher unavailable\"}", "application/json");
                return;
            }
            std::string out;
            if (!mt_call_string([]() { return scene_json(); }, out, 10000)) out = "{\"error\":\"timeout\"}";
            res.set_content(out, "application/json");
        });

        svr.Get("/api/scene/inspect",[](const httplib::Request& req, httplib::Response& res) {
            auto s = req.get_param_value("addr");
            if (s.empty()) { res.set_content("{}", "application/json"); return; }
            if (!mt_on_main_thread() && !mt_install()) {
                res.status = 503;
                res.set_content("{\"error\":\"main thread dispatcher unavailable\"}", "application/json");
                return;
            }
            uintptr_t ga = (uintptr_t)strtoull(s.c_str(), nullptr, 16);
            std::string out;
            if (!mt_call_string([ga]() { return go_info_json(ga); }, out, 10000)) out = "{\"error\":\"timeout\"}";
            res.set_content(out, "application/json");
        });

        svr.Get("/api/controller/inspect",[](const httplib::Request& req, httplib::Response& res) {
            attach_thread();
            uintptr_t addr = (uintptr_t)strtoull(req.get_param_value("addr").c_str(), nullptr, 16);
            std::string a = req.get_param_value("asm"), ns = req.get_param_value("ns"), cn = req.get_param_value("cls");

            if (cn.empty()) { res.set_content("{}", "application/json"); return; }
            Class start_cls = a.empty() ? Class(ns.c_str(), cn.c_str()) : Class(ns.c_str(), cn.c_str(), Image(a.c_str()));
            if (!start_cls.IsValid()) {
                for (auto& img : Image::GetImages()) {
                    if (!img.IsValid()) continue;
                    Class try_c(ns.c_str(), cn.c_str(), img);
                    if (try_c.IsValid()) { start_cls = try_c; break; }
                }
            }
            if (!start_cls.IsValid()) { res.set_content("{}", "application/json"); return; }

            std::ostringstream j;
            j << "{\"addr\":\"" << std::hex << addr << "\",\"name\":\"" << js_esc(cn) << "\",\"fields\":[";

            bool first_f = true;
            std::vector<std::string> seen;

            for (Class cur = start_cls; cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
                auto* cn_ = cur.GetClass()->name;
                if (!cn_ || !is_valid_pointer(cn_)) break;
                if (strcmp(cn_, "Object") == 0 && cur.GetClass()->namespaze && strcmp(cur.GetClass()->namespaze, "System") == 0) break;

                for (auto& f : cur.GetFields(false)) {
                    try {
                        auto* fi = f.GetInfo();
                        if (!fi || !is_valid_pointer(fi) || !fi->type || !is_valid_pointer(fi->type)) continue;
                        bool f_static = (fi->type->attrs & 0x10) != 0;
                        if (!f_static && (!addr || !is_valid_pointer((void*)addr))) continue;
                        std::string fn = fi->name;
                        bool dup = false; for (auto& s : seen) if (s==fn) dup=true;
                        if (dup) continue; seen.push_back(fn);
                        std::string ft = type_name(fi->type);
                        bool ok = false;
                        std::string vs = read_field(ft, cur, f_static ? nullptr : (Il2CppObject*)addr, fn, nullptr, f_static, ok, fi->type);
                        if (ok) { if (!first_f) j << ","; first_f = false; j << "{\"name\":\"" << js_esc(fn) << "\",\"type\":\"" << ft << "\",\"val\":" << vs << ",\"isProp\":false,\"canWrite\":true,\"static\":" << (f_static?"true":"false") << "}"; }
                    } catch(...) {}
                }

                for (auto& p : cur.GetProperties(false)) {
                    try {
                        auto* pi = p._data;
                        if (!pi || !is_valid_pointer(pi) || !pi->get) continue;
                        bool p_static = (pi->get->flags & 0x10) != 0;
                        if (!p_static && (!addr || !is_valid_pointer((void*)addr))) continue;
                        std::string pn = pi->name;
                        bool dup = false; for (auto& s : seen) if (s==pn) dup=true;
                        if (dup) continue; seen.push_back(pn);
                        std::string pt = (pi->get->return_type && is_valid_pointer(pi->get->return_type)) ? type_name(pi->get->return_type) : "?";
                        bool ok = false;
                        MethodBase getter(pi->get);
                        std::string vs = read_field(pt, cur, p_static ? nullptr : (Il2CppObject*)addr, pn, &getter, p_static, ok, pi->get->return_type);
                        if (ok) { if (!first_f) j << ","; first_f = false; j << "{\"name\":\"" << js_esc(pn) << "\",\"type\":\"" << pt << "\",\"val\":" << vs << ",\"isProp\":true,\"canWrite\":" << (pi->set?"true":"false") << ",\"static\":" << (p_static?"true":"false") << "}"; }
                    } catch(...) {}
                }
            }

            j << "],\"methods\":[";
            bool first_m = true;
            for (Class cur = start_cls; cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
                auto* cn_ = cur.GetClass()->name;
                if (!cn_ || !is_valid_pointer(cn_)) break;
                if (strcmp(cn_, "Object") == 0 && cur.GetClass()->namespaze && strcmp(cur.GetClass()->namespaze, "System") == 0) break;
                for (auto& m : cur.GetMethods(false)) {
                    auto* mi = m.GetInfo();
                    if (!m.IsValid() || !mi || !is_valid_pointer(mi) || !mi->name || !is_valid_pointer(mi->name)) continue;
                    if (!first_m) j << ","; first_m = false;
                    std::string ret_t = (mi->return_type && is_valid_pointer(mi->return_type)) ? type_name(mi->return_type) : "?";
                    j << "{\"name\":\"" << js_esc(mi->name) << "\",\"ret\":\"" << js_esc(ret_t) << "\",\"s\":" << ((mi->flags&0x10)?"true":"false") << ",\"params\":[";
                    int pcount = (int)mi->parameters_count;
                    for (int i = 0; i < pcount; i++) {
                        if (i>0) j<<",";
                        std::string pt = get_param_type_name(mi, i);
                        std::string pn = get_param_name(mi, i);
                        j << "{\"n\":\"" << js_esc(pn) << "\",\"t\":\"" << js_esc(pt) << "\"}";
                    }
                    j << "]}";
                }
            }
            j << "]}";
            res.set_content(j.str(), "application/json");
        });

        svr.Post("/api/scene/update",[](const httplib::Request& req, httplib::Response& res) {
            attach_thread();
            uintptr_t addr = (uintptr_t)strtoull(json_val(req.body, "addr").c_str(), nullptr, 16);
            if (!addr || !is_valid_pointer((void*)addr)) { res.set_content("{\"ok\":false}", "application/json"); return; }
            std::string type = json_val(req.body, "type"), prop = json_val(req.body, "prop"), val = json_val(req.body, "val");
            try {
                if (type == "gameobject") {
                    if (prop == "active") {
                        bool new_active = val == "true";
                        Il2CppObject* go = resolve_component_by_addr(addr);
                        if (go) {
                            Method<void> set_active(Class("UnityEngine","GameObject").GetMethod("SetActive", 1));
                            if (set_active.IsValid()) set_active[go](new_active);
                        }
                    } else if (prop == "name") {
                        Il2CppObject* go = resolve_component_by_addr(addr);
                        if (go) {
                            Method<void> m(Class("UnityEngine","Object").GetMethod("set_name",1));
                            if(m.IsValid()) m[go](CreateMonoString(val));
                        }
                    }
                } else if (type == "transform") {
                    float x=0,y=0,z=0; sscanf(val.c_str(),"[%f,%f,%f]",&x,&y,&z); Vector3 v={x,y,z};
                    Il2CppObject* tr = resolve_component_by_addr(addr);
                    if (tr) {
                        if (prop=="p") { Method<void> m(Class("UnityEngine","Transform").GetMethod("set_localPosition",1)); if(m.IsValid()) m[tr](v); }
                        else if (prop=="r") { Method<void> m(Class("UnityEngine","Transform").GetMethod("set_localEulerAngles",1)); if(m.IsValid()) m[tr](v); }
                        else if (prop=="s") { Method<void> m(Class("UnityEngine","Transform").GetMethod("set_localScale",1)); if(m.IsValid()) m[tr](v); }
                    }
                } else if (type == "script") {
                    std::string name = json_val(req.body, "prop2");
                    std::string ft   = json_val(req.body, "ftype");
                    bool is_prop     = json_val(req.body, "isProp") == "true";
                    if (prop == "enabled") {
                        try {
                            bool new_enabled = val == "true";
                            Il2CppObject* comp = resolve_component_by_addr(addr);
                            if (comp && comp->klass) {
                                auto set_enabled_on_comp = [&](Il2CppObject* c, bool v) {
                                    Class comp_klass(c->klass);
                                    for (Class cur = comp_klass; cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
                                        auto* cn_ = cur.GetClass()->name;
                                        if (!cn_ || strcmp(cn_, "Component") == 0 || strcmp(cn_, "Object") == 0) break;
                                        auto ep = cur.GetProperty("enabled");
                                        if (ep.IsValid() && ep._data && ep._data->set) {
                                            Method<void> m(ep._data->set); m.SetInstance(c); m(v); return true;
                                        }
                                    }
                                    return false;
                                };
                                auto get_enabled_from_comp = [&](Il2CppObject* c) -> bool {
                                    Class comp_klass(c->klass);
                                    for (Class cur = comp_klass; cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
                                        auto* cn_ = cur.GetClass()->name;
                                        if (!cn_ || strcmp(cn_, "Component") == 0 || strcmp(cn_, "Object") == 0) break;
                                        auto ep = cur.GetProperty("enabled");
                                        if (ep.IsValid() && ep._data && ep._data->get) {
                                            Method<bool> m(ep._data->get); m.SetInstance(c);
                                            try { return m(); } catch(...) {}
                                        }
                                    }
                                    return false;
                                };
                                if (new_enabled) {
                                    bool already = get_enabled_from_comp(comp);
                                    if (!already) {
                                        set_enabled_on_comp(comp, true);
                                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                        comp = resolve_component_by_addr(addr);
                                        if (!comp) { res.set_content("{\"ok\":true,\"warn\":\"enabled but object moved\"}", "application/json"); return; }
                                    }
                                } else {
                                    set_enabled_on_comp(comp, false);
                                }
                            }
                        } catch(...) {}
                    } else {
                        std::string comp_ns  = json_val(req.body, "compNs");
                        std::string comp_cls = json_val(req.body, "compCls");
                        std::string comp_asm = json_val(req.body, "compAsm");
                        Il2CppObject* obj = resolve_component_by_addr(addr);
                        if (!obj) { res.set_content("{\"ok\":false,\"error\":\"stale address\"}", "application/json"); return; }
                        Class start_cls = (obj->klass) ? Class(obj->klass) :
                                          Class(comp_ns.c_str(), comp_cls.c_str(), comp_asm.empty() ? Image() : Image(comp_asm.c_str()));
                        for (Class cur = start_cls; cur.IsValid(); cur = cur.GetParent()) {
                            if (is_prop) {
                                auto p = cur.GetProperty(name.c_str());
                                if (p.IsValid() && p._data && p._data->set) { MethodBase s(p._data->set); write_field(ft, cur, obj, name, &s, val); break; }
                            } else {
                                auto f = cur.GetField(name.c_str());
                                if (f.IsValid()) { write_field(ft, cur, obj, name, nullptr, val); break; }
                            }
                        }
                    }
                }
            } catch(...) {}
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Post("/api/instance/update",[](const httplib::Request& req, httplib::Response& res) {
            attach_thread();
            uintptr_t addr = (uintptr_t)strtoull(json_val(req.body, "addr").c_str(), nullptr, 16);
            std::string a=json_val(req.body,"asm"), ns=json_val(req.body,"ns"), cn=json_val(req.body,"cls");
            std::string name=json_val(req.body,"name"), ft=json_val(req.body,"ftype"), val=json_val(req.body,"val");
            bool is_prop = json_val(req.body,"isProp") == "true";
            bool is_static_field = json_val(req.body,"isStatic") == "true";
            Il2CppObject* obj = is_static_field ? nullptr : resolve_component_by_addr(addr);
            if (obj || is_static_field) {
                try {
                    for (Class cur(ns.c_str(),cn.c_str(),Image(a.c_str())); cur.IsValid(); cur = cur.GetParent()) {
                        if (is_prop) {
                            auto p = cur.GetProperty(name.c_str());
                            if (p.IsValid() && p._data && p._data->set) { MethodBase s(p._data->set); write_field(ft,cur,obj,name,&s,val,is_static_field); break; }
                        } else {
                            auto f = cur.GetField(name.c_str());
                            if (f.IsValid()) { write_field(ft,cur,obj,name,nullptr,val,is_static_field); break; }
                        }
                    }
                } catch(...) {}
            }
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Post("/api/scene/delete",[](const httplib::Request& req, httplib::Response& res) {
            uintptr_t addr = (uintptr_t)strtoull(json_val(req.body, "addr").c_str(), nullptr, 16);
            LOGD("[Scene] Delete request addr=%p", (void*)addr);
            if (!addr) { res.status = 400; res.set_content("{\"ok\":false,\"error\":\"missing address\"}", "application/json"); return; }
            if (!mt_on_main_thread() && !mt_install()) {
                res.status = 503;
                res.set_content("{\"ok\":false,\"error\":\"main thread dispatcher unavailable\"}", "application/json");
                return;
            }
            std::string out;
            bool completed = mt_call_string([addr]() {
                attach_thread();
                Il2CppObject* obj = resolve_component_by_addr(addr);
                if (!obj || !il2cpp_class_has_base(obj->klass, "UnityEngine", "GameObject")) {
                    LOGE("[Scene] Delete rejected addr=%p: invalid GameObject", (void*)addr);
                    return std::string("{\"ok\":false,\"error\":\"invalid GameObject\"}");
                }
                auto* unity_object = reinterpret_cast<BNM::UnityEngine::Object*>(obj);
                if (!unity_object->IsValid()) {
                    LOGE("[Scene] Delete rejected addr=%p: Unity object is not alive", (void*)addr);
                    return std::string("{\"ok\":false,\"error\":\"GameObject is no longer valid\"}");
                }
                if (!scene_registry_ready()) {
                    return std::string("{\"ok\":false,\"error\":\"scene is refreshing, try again\"}");
                }
                std::string target_name = "Unknown";
                std::string target_path;
                int target_instance_id = 0;
                bool target_active_self = false;
                bool target_active_hierarchy = false;
                int target_scene_handle = 0;
                int target_hide_flags = 0;
                try {
                    Class object_cls("UnityEngine", "Object");
                    Class game_object_cls("UnityEngine", "GameObject");
                    Class transform_cls("UnityEngine", "Transform");
                    Method<BNM::Structures::Mono::String*> get_name(object_cls.GetMethod("get_name", 0));
                    Method<int> get_instance_id(object_cls.GetMethod("GetInstanceID", 0));
                    Method<bool> get_active_self(game_object_cls.GetMethod("get_activeSelf", 0));
                    Method<bool> get_active_hierarchy(game_object_cls.GetMethod("get_activeInHierarchy", 0));
                    Method<int> get_flags(object_cls.GetMethod("get_hideFlags", 0));
                    Method<BNM::UnityEngine::SceneManagement::Scene> get_scene(game_object_cls.GetMethod("get_scene", 0));
                    Method<Il2CppObject*> get_transform(game_object_cls.GetMethod("get_transform", 0));
                    Method<Il2CppObject*> get_parent(transform_cls.GetMethod("get_parent", 0));
                    if (method_has_pointer(get_name)) {
                        auto* value = get_name[obj]();
                        if (value && is_valid_pointer(value)) target_name = value->str();
                    }
                    if (method_has_pointer(get_instance_id)) target_instance_id = get_instance_id[obj]();
                    if (method_has_pointer(get_active_self)) target_active_self = get_active_self[obj]();
                    if (method_has_pointer(get_active_hierarchy)) target_active_hierarchy = get_active_hierarchy[obj]();
                    if (method_has_pointer(get_flags)) target_hide_flags = get_flags[obj]();
                    if (method_has_pointer(get_scene)) target_scene_handle = get_scene[obj]().m_Handle;
                    if (method_has_pointer(get_transform) && method_has_pointer(get_parent)) {
                        Il2CppObject* tr = get_transform[obj]();
                        std::vector<std::string> path_parts;
                        for (int depth = 0; tr && is_valid_pointer(tr) && depth < 32; ++depth) {
                            std::string part = "?";
                            if (method_has_pointer(get_name)) {
                                auto* value = get_name[tr]();
                                if (value && is_valid_pointer(value)) part = value->str();
                            }
                            path_parts.push_back(part);
                            tr = get_parent[tr]();
                        }
                        for (auto it = path_parts.rbegin(); it != path_parts.rend(); ++it) {
                            if (!target_path.empty()) target_path += "/";
                            target_path += *it;
                        }
                    }
                } catch(...) {
                    LOGW("[Scene] Delete target metadata collection failed addr=%p", obj);
                }
                LOGD("[Scene] Delete target name=%s path=%s addr=%p instanceId=%d activeSelf=%d activeHierarchy=%d sceneHandle=%d hideFlags=0x%X",
                     target_name.c_str(), target_path.c_str(), obj, target_instance_id,
                     target_active_self ? 1 : 0, target_active_hierarchy ? 1 : 0,
                     target_scene_handle, target_hide_flags);

                if (is_protected_unity_object(target_scene_handle, target_hide_flags)) {
                    LOGW("[Scene] Delete rejected addr=%p: engine owned object sceneHandle=%d hideFlags=0x%X",
                         (void*)addr, target_scene_handle, target_hide_flags);
                    return std::string("{\"ok\":false,\"error\":\"engine owned object cannot be deleted\"}");
                }
                MethodBase destroy_base = find_destroy_object_method();
                if (!destroy_base.IsValid() || !method_has_pointer(destroy_base)) {
                    LOGE("[Scene] Delete rejected addr=%p: exact Object.Destroy(Object) method unavailable", (void*)addr);
                    return std::string("{\"ok\":false,\"error\":\"Object.Destroy(Object) unavailable\"}");
                }
                auto* destroy_info = destroy_base.GetInfo();
                LOGD("[Scene] Destroy call target=%p methodInfo=%p methodPointer=%p parameter=%s",
                     obj, destroy_info, destroy_info ? (void*)destroy_info->methodPointer : nullptr,
                     destroy_info ? get_param_type_name(destroy_info, 0).c_str() : "?");
                try {
                    freeze_bypass_guard bypass;
                    Method<void> destroy(destroy_base);
                    destroy(obj);
                    invalidate_component_index();
                    invalidate_scene_cache();
                    scene_registry_invalidate();
                    LOGD("[Scene] Destroy scheduled GameObject=%p", obj);
                    return std::string("{\"ok\":true,\"scheduled\":true}");
                } catch(...) {
                    LOGE("[Scene] Destroy failed for GameObject=%p", obj);
                    return std::string("{\"ok\":false,\"error\":\"Object.Destroy invocation failed\"}");
                }
            }, out, 5000);
            if (!completed) { res.status = 504; out = "{\"ok\":false,\"error\":\"main thread operation timed out\"}"; }
            res.set_content(out, "application/json");
        });

        svr.Post("/api/invoke",[](const httplib::Request& req, httplib::Response& res) {
            auto r = invoke_method(req.body);
            std::ostringstream j;
            j << "{\"ok\":" << (r.ok?"true":"false") << ",\"value\":\"" << js_esc(r.val) << "\",\"error\":\"" << js_esc(r.err) << "\"}";
            res.set_content(j.str(), "application/json");
        });

        svr.Post("/api/loop/add",[](const httplib::Request& req, httplib::Response& res) {
            auto e = std::make_shared<loop_entry>();
            e->addr         = json_val(req.body, "addr");
            e->asm_         = json_val(req.body, "asm");
            e->ns           = json_val(req.body, "ns");
            e->cls          = json_val(req.body, "cls");
            e->name         = json_val(req.body, "name");
            e->ftype        = json_val(req.body, "ftype");
            e->is_prop      = json_val(req.body, "isProp") == "true";
            e->val          = json_val(req.body, "val");
            int ms          = 0;
            try { ms = std::stoi(json_val(req.body, "interval")); } catch(...) {}
            if (ms < 50) ms = 100;
            e->interval_ms = ms;

            std::string id = e->addr + "_" + e->cls + "_" + e->name;
            {
                std::lock_guard<std::mutex> lk(g_loop_mtx);
                auto it = g_loops.find(id);
                if (it != g_loops.end()) it->second->active = false;
                g_loops[id] = e;
            }

            std::thread([e, id]() {
                attach_thread();
                while (e->active) {
                    apply_loop_entry(e);
                    std::this_thread::sleep_for(std::chrono::milliseconds(e->interval_ms));
                }
                std::lock_guard<std::mutex> lk(g_loop_mtx);
                auto it = g_loops.find(id);
                if (it != g_loops.end() && it->second == e) g_loops.erase(it);
            }).detach();

            std::ostringstream j;
            j << "{\"ok\":true,\"id\":\"" << js_esc(id) << "\"}";
            res.set_content(j.str(), "application/json");
        });

        svr.Post("/api/loop/remove",[](const httplib::Request& req, httplib::Response& res) {
            std::string id = json_val(req.body, "id");
            std::lock_guard<std::mutex> lk(g_loop_mtx);
            auto it = g_loops.find(id);
            if (it != g_loops.end()) { it->second->active = false; g_loops.erase(it); }
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Post("/api/loop/removeall",[](const httplib::Request&, httplib::Response& res) {
            std::lock_guard<std::mutex> lk(g_loop_mtx);
            for (auto& kv : g_loops) kv.second->active = false;
            g_loops.clear();
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Get("/api/loop/list",[](const httplib::Request&, httplib::Response& res) {
            res.set_content(loop_list_json(), "application/json");
        });

        svr.Get("/api/logs",[](const httplib::Request&, httplib::Response& res) {
            res.set_content(logs_json(), "application/json");
        });

        svr.Post("/api/logs/clear",[](const httplib::Request&, httplib::Response& res) {
            std::lock_guard<std::mutex> lk(g_log_mtx);
            g_log_buffer.clear();
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Post("/api/logs/add",[](const httplib::Request& req, httplib::Response& res) {
            std::string msg = json_val(req.body, "msg");
            int level = 0;
            try { level = std::stoi(json_val(req.body, "level")); } catch(...) {}
            add_log_entry(msg, level);
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Get("/api/timescale",[](const httplib::Request&, httplib::Response& res) {
            attach_thread();
            float ts = 1.0f;
            try {
                Class time_cls("UnityEngine", "Time");
                if (time_cls.IsValid()) {
                    Method<float> get(time_cls.GetMethod("get_timeScale"));
                    if (get.IsValid()) ts = get();
                }
            } catch(...) {}
            res.set_content("{\"value\":" + std::to_string(ts) + "}", "application/json");
        });

        svr.Post("/api/timescale",[](const httplib::Request& req, httplib::Response& res) {
            attach_thread();
            float val = 1.0f;
            try { val = std::stof(json_val(req.body, "value")); } catch(...) {}
            try {
                Class time_cls("UnityEngine", "Time");
                if (time_cls.IsValid()) {
                    Method<void> set(time_cls.GetMethod("set_timeScale", 1));
                    if (set.IsValid()) set(val);
                }
            } catch(...) {}
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Post("/api/scene/destroy",[](const httplib::Request& req, httplib::Response& res) {
            uintptr_t addr = (uintptr_t)strtoull(json_val(req.body, "addr").c_str(), nullptr, 16);
            if (!addr) { res.status = 400; res.set_content("{\"ok\":false,\"error\":\"missing address\"}", "application/json"); return; }
            if (!mt_on_main_thread() && !mt_install()) {
                res.status = 503;
                res.set_content("{\"ok\":false,\"error\":\"main thread dispatcher unavailable\"}", "application/json");
                return;
            }
            std::string out;
            bool completed = mt_call_string([addr]() {
                attach_thread();
                Il2CppObject* obj = resolve_component_by_addr(addr);
                if (!obj || !is_unity_object_instance(obj))
                    return std::string("{\"ok\":false,\"error\":\"invalid Unity object\"}");
                auto* unity_object = reinterpret_cast<BNM::UnityEngine::Object*>(obj);
                if (!unity_object->IsValid()) return std::string("{\"ok\":false,\"error\":\"Unity object is no longer valid\"}");
                MethodBase destroy_base = Class("UnityEngine", "Object").GetMethod("Destroy", 1);
                auto* destroy_info = destroy_base.GetInfo();
                if (!destroy_info || !destroy_info->methodPointer || !is_valid_pointer((const void*)destroy_info->methodPointer))
                    return std::string("{\"ok\":false,\"error\":\"Object.Destroy method unavailable\"}");
                try {
                    freeze_bypass_guard bypass;
                    Method<void> destroy(destroy_base);
                    destroy(obj);
                    invalidate_component_index();
                    invalidate_scene_cache();
                    LOGD("[Scene] Destroy scheduled UnityObject=%p", obj);
                    return std::string("{\"ok\":true,\"scheduled\":true}");
                } catch(...) {
                    LOGE("[Scene] Destroy failed for UnityObject=%p", obj);
                    return std::string("{\"ok\":false,\"error\":\"Object.Destroy invocation failed\"}");
                }
            }, out, 5000);
            if (!completed) { res.status = 504; out = "{\"ok\":false,\"error\":\"main thread operation timed out\"}"; }
            res.set_content(out, "application/json");
        });

        svr.Post("/api/scene/create",[](const httplib::Request& req, httplib::Response& res) {
            attach_thread();
            std::string name = json_val(req.body, "name");
            if (name.empty()) name = "New GameObject";
            std::ostringstream j;
            try {
                Class go_cls("UnityEngine", "GameObject");
                Method<Il2CppObject*> ctor;
                for (auto& m : go_cls.GetMethods(false)) {
                    auto* mi = m.GetInfo();
                    if (mi && is_valid_pointer(mi) && mi->name && is_valid_pointer(mi->name) && strcmp(mi->name, ".ctor") == 0 && mi->parameters_count == 1) {
                        ctor = Method<Il2CppObject*>(m);
                        break;
                    }
                }
                if (ctor.IsValid()) {
                    Il2CppObject* new_go = go_cls.CreateNewObjectParameters(CreateMonoString(name.c_str()));
                    if (new_go) {
                        j << "{\"ok\":true,\"addr\":\"" << std::hex << (uintptr_t)new_go << "\"}";
                        res.set_content(j.str(), "application/json");
                        return;
                    }
                }
            } catch(...) {}
            res.set_content("{\"ok\":false,\"error\":\"Failed to create GameObject\"}", "application/json");
        });

        svr.Post("/api/scene/addcomponent",[](const httplib::Request& req, httplib::Response& res) {
            attach_thread();
            uintptr_t addr = (uintptr_t)strtoull(json_val(req.body, "addr").c_str(), nullptr, 16);
            std::string type_name_ = json_val(req.body, "type");
            if (!addr || !is_valid_pointer((void*)addr) || type_name_.empty()) { res.set_content("{\"ok\":false}", "application/json"); return; }
            Il2CppObject* go = resolve_component_by_addr(addr);
            if (!go) { res.set_content("{\"ok\":false,\"error\":\"stale address\"}", "application/json"); return; }
            try {
                Il2CppObject* type_obj = sys_type_of(type_name_);
                if (!type_obj) type_obj = sys_type_of(type_name_ + ", UnityEngine.CoreModule");
                if (!type_obj) type_obj = sys_type_of(type_name_ + ", UnityEngine");
                if (!type_obj) type_obj = sys_type_of(type_name_ + ", Assembly-CSharp");
                if (type_obj) {
                    Class go_cls("UnityEngine", "GameObject");
                    Method<Il2CppObject*> add_comp(go_cls.GetMethod("AddComponent", 1));
                    if (add_comp.IsValid()) {
                        Il2CppObject* comp = add_comp[go](type_obj);
                        if (comp) {
                            std::ostringstream j;
                            j << "{\"ok\":true,\"addr\":\"" << std::hex << (uintptr_t)comp << "\"}";
                            res.set_content(j.str(), "application/json");
                            return;
                        }
                    }
                }
            } catch(...) {}
            res.set_content("{\"ok\":false,\"error\":\"Failed to add component\"}", "application/json");
        });

        svr.Post("/api/scene/removecomponent",[](const httplib::Request& req, httplib::Response& res) {
            uintptr_t addr = (uintptr_t)strtoull(json_val(req.body, "addr").c_str(), nullptr, 16);
            LOGD("[Scene] Remove component request addr=%p", (void*)addr);
            if (!addr) { res.status = 400; res.set_content("{\"ok\":false,\"error\":\"missing address\"}", "application/json"); return; }
            if (!mt_on_main_thread() && !mt_install()) {
                res.status = 503;
                res.set_content("{\"ok\":false,\"error\":\"main thread dispatcher unavailable\"}", "application/json");
                return;
            }
            std::string out;
            bool completed = mt_call_string([addr]() {
                attach_thread();
                Il2CppObject* obj = resolve_component_by_addr(addr);
                if (!obj || !il2cpp_class_has_base(obj->klass, "UnityEngine", "Component")) {
                    LOGE("[Scene] Remove component rejected addr=%p: invalid Component", (void*)addr);
                    return std::string("{\"ok\":false,\"error\":\"invalid Component\"}");
                }
                if (il2cpp_class_has_base(obj->klass, "UnityEngine", "Transform")) {
                    LOGE("[Scene] Remove component rejected addr=%p: Transform is mandatory", (void*)addr);
                    return std::string("{\"ok\":false,\"error\":\"Transform cannot be removed separately\"}");
                }
                auto* unity_object = reinterpret_cast<BNM::UnityEngine::Object*>(obj);
                if (!unity_object->IsValid()) {
                    LOGE("[Scene] Remove component rejected addr=%p: Unity object is not alive", (void*)addr);
                    return std::string("{\"ok\":false,\"error\":\"Component is no longer valid\"}");
                }
                MethodBase destroy_base = find_destroy_object_method();
                if (!destroy_base.IsValid() || !method_has_pointer(destroy_base)) {
                    LOGE("[Scene] Remove component rejected addr=%p: exact Object.Destroy(Object) unavailable", (void*)addr);
                    return std::string("{\"ok\":false,\"error\":\"Object.Destroy(Object) unavailable\"}");
                }
                try {
                    freeze_bypass_guard bypass;
                    Method<void> destroy(destroy_base);
                    destroy(obj);
                    invalidate_component_index();
                    LOGD("[Scene] Destroy scheduled Component=%p class=%s", obj,
                         obj->klass->name && is_valid_pointer(obj->klass->name) ? obj->klass->name : "<invalid>");
                    return std::string("{\"ok\":true,\"scheduled\":true}");
                } catch(...) {
                    LOGE("[Scene] Destroy failed for Component=%p", obj);
                    return std::string("{\"ok\":false,\"error\":\"Object.Destroy invocation failed\"}");
                }
            }, out, 5000);
            if (!completed) { res.status = 504; out = "{\"ok\":false,\"error\":\"main thread operation timed out\"}"; }
            res.set_content(out, "application/json");
        });

        svr.Get("/api/dump",[](const httplib::Request&, httplib::Response& res) {
            attach_thread();
            std::ostringstream out;
            for (auto& img : Image::GetImages()) {
                auto* d = img.GetInfo();
                if (!img.IsValid() || !d || !is_valid_pointer(d) || !d->name || !is_valid_pointer(d->name)) continue;
                out << "// Assembly: " << d->name << "\n";
                for (auto& cls : img.GetClasses(false)) {
                    auto* r = cls.GetClass();
                    if (!cls.IsValid() || !r || !is_valid_pointer(r) || !r->name || !is_valid_pointer(r->name)) continue;
                    std::string cls_kw = "class";
                    if (r->enumtype) cls_kw = "enum";
                    else if (bnm_class_is_value_type(r)) cls_kw = "struct";
                    else if (r->flags & 0x20) cls_kw = "interface";
                    if (r->namespaze && is_valid_pointer(r->namespaze) && strlen(r->namespaze) > 0) {
                        out << "namespace " << r->namespaze << " {\n";
                    }
                    std::string indent = (r->namespaze && is_valid_pointer(r->namespaze) && strlen(r->namespaze) > 0) ? "    " : "";
                    std::string parent = "";
                    if (r->parent && is_valid_pointer(r->parent) && r->parent->name && is_valid_pointer(r->parent->name) && strcmp(r->parent->name, "Object") != 0 && strcmp(r->parent->name, "ValueType") != 0)
                        parent = " : " + std::string(r->parent->namespaze && is_valid_pointer(r->parent->namespaze) && strlen(r->parent->namespaze) > 0 ? std::string(r->parent->namespaze) + "." : "") + r->parent->name;
                    out << indent << "public " << cls_kw << " " << r->name << parent << " {\n";
                    for (auto& f : cls.GetFields(false)) {
                        auto* fi = f.GetInfo();
                        if (!fi || !is_valid_pointer(fi) || !fi->name || !is_valid_pointer(fi->name)) continue;
                        std::string tn = (fi->type && is_valid_pointer(fi->type)) ? type_name(fi->type) : "?";
                        bool is_static = fi->type && is_valid_pointer(fi->type) && (fi->type->attrs & 0x10);
                        out << indent << "    public " << (is_static ? "static " : "") << tn << " " << fi->name << "; // 0x" << std::hex << fi->offset << std::dec << "\n";
                    }
                    for (auto& p : cls.GetProperties(false)) {
                        auto* pi = p._data;
                        if (!pi || !is_valid_pointer(pi) || !pi->name || !is_valid_pointer(pi->name)) continue;
                        std::string tn = (pi->get && is_valid_pointer(pi->get) && pi->get->return_type && is_valid_pointer(pi->get->return_type)) ? type_name(pi->get->return_type) : "?";
                        out << indent << "    public " << tn << " " << pi->name << " {";
                        if (pi->get) out << " get;";
                        if (pi->set) out << " set;";
                        out << " }\n";
                    }
                    for (auto& m : cls.GetMethods(false)) {
                        auto* mi = m.GetInfo();
                        if (!mi || !is_valid_pointer(mi) || !mi->name || !is_valid_pointer(mi->name)) continue;
                        std::string ret = (mi->return_type && is_valid_pointer(mi->return_type)) ? type_name(mi->return_type) : "?";
                        bool is_static = mi->flags & 0x10;
                        char ab[32] = {};
                        if (mi->methodPointer) snprintf(ab, sizeof(ab), " // %llX", (unsigned long long)(uintptr_t)mi->methodPointer);
                        out << indent << "    public " << (is_static ? "static " : "") << ret << " " << mi->name << "(";
                        int pcount = (int)mi->parameters_count;
                        for (int i = 0; i < pcount; i++) {
                            if (i > 0) out << ", ";
                            out << get_param_type_name(mi, i) << " " << get_param_name(mi, i);
                        }
                        out << ") {}" << ab << "\n";
                    }
                    out << indent << "}\n";
                    if (r->namespaze && is_valid_pointer(r->namespaze) && strlen(r->namespaze) > 0) out << "}\n";
                }
            }
            res.set_content(out.str(), "text/plain");
        });

        svr.Get("/api/scene/addable",[](const httplib::Request&, httplib::Response& res) {
            attach_thread();
            std::ostringstream j;
            j << "{";
            bool first_cat = true;
            static const struct { const char* cat; const char* ns; const char* name; } known[] = {
                    {"Physics","UnityEngine","Rigidbody"},{"Physics","UnityEngine","BoxCollider"},{"Physics","UnityEngine","SphereCollider"},
                    {"Physics","UnityEngine","CapsuleCollider"},{"Physics","UnityEngine","MeshCollider"},{"Physics","UnityEngine","CharacterController"},
                    {"Physics","UnityEngine","WheelCollider"},{"Physics","UnityEngine","Rigidbody2D"},{"Physics","UnityEngine","BoxCollider2D"},
                    {"Physics","UnityEngine","CircleCollider2D"},{"Physics","UnityEngine","PolygonCollider2D"},{"Physics","UnityEngine","CapsuleCollider2D"},
                    {"Rendering","UnityEngine","MeshRenderer"},{"Rendering","UnityEngine","MeshFilter"},{"Rendering","UnityEngine","SkinnedMeshRenderer"},
                    {"Rendering","UnityEngine","SpriteRenderer"},{"Rendering","UnityEngine","LineRenderer"},{"Rendering","UnityEngine","TrailRenderer"},
                    {"Rendering","UnityEngine","Camera"},{"Rendering","UnityEngine","Light"},
                    {"Audio","UnityEngine","AudioSource"},{"Audio","UnityEngine","AudioListener"},{"Audio","UnityEngine","AudioReverbZone"},
                    {"Animation","UnityEngine","Animator"},{"Animation","UnityEngine","Animation"},{"Animation","UnityEngine","ParticleSystem"},
                    {"Navigation","UnityEngine","NavMeshAgent"},{"Navigation","UnityEngine","NavMeshObstacle"},
                    {"UI","UnityEngine.UI","Text"},{"UI","UnityEngine.UI","Image"},{"UI","UnityEngine.UI","Button"},
                    {"UI","UnityEngine.UI","Toggle"},{"UI","UnityEngine.UI","Slider"},{"UI","UnityEngine.UI","InputField"},
                    {"UI","UnityEngine.UI","Canvas"},{"UI","UnityEngine","CanvasGroup"},
                    {nullptr,nullptr,nullptr}
            };
            std::map<std::string, std::vector<std::pair<std::string,std::string>>> cats;
            for (int i = 0; known[i].cat; i++) {
                Class c(known[i].ns, known[i].name);
                if (c.IsValid()) cats[known[i].cat].push_back({std::string(known[i].ns) + "." + known[i].name, known[i].name});
            }
            for (auto& img : Image::GetImages()) {
                auto* d = img.GetInfo();
                if (!img.IsValid() || !d || !is_valid_pointer(d) || !d->name || !is_valid_pointer(d->name)) continue;
                if (strstr(d->name, "Assembly-CSharp") == nullptr && strstr(d->name, "Assembly-CSharp-firstpass") == nullptr) continue;
                for (auto& cls : img.GetClasses(false)) {
                    auto* r = cls.GetClass();
                    if (!cls.IsValid() || !r || !is_valid_pointer(r) || !r->name || !is_valid_pointer(r->name)) continue;
                    bool is_mono = false;
                    for (auto* cur = r; cur && is_valid_pointer(cur); cur = cur->parent)
                        if (cur->name && is_valid_pointer(cur->name) && strcmp(cur->name, "MonoBehaviour") == 0) { is_mono = true; break; }
                    if (!is_mono) continue;
                    std::string ns = (r->namespaze && is_valid_pointer(r->namespaze)) ? r->namespaze : "";
                    std::string full_name = ns.empty() ? r->name : ns + "." + r->name;
                    cats["Scripts"].push_back({full_name, r->name});
                }
            }
            for (auto& kv : cats) {
                if (!first_cat) j << ","; first_cat = false;
                j << "\"" << js_esc(kv.first) << "\":[";
                bool first = true;
                for (auto& p : kv.second) {
                    if (!first) j << ","; first = false;
                    j << "{\"full\":\"" << js_esc(p.first) << "\",\"name\":\"" << js_esc(p.second) << "\"}";
                }
                j << "]";
            }
            j << "}";
            res.set_content(j.str(), "application/json");
        });

        svr.Post("/api/catcher/hook",[](const httplib::Request& req, httplib::Response& res) {
            attach_thread();
            std::string asm_ = json_val(req.body, "asm");
            std::string ns   = json_val(req.body, "ns");
            std::string cls  = json_val(req.body, "cls");
            std::string meth = json_val(req.body, "method");
            std::string id   = asm_ + "|" + ns + "|" + cls + "|" + meth;
            {
                std::lock_guard<std::mutex> lk(g_catcher_mtx);
                if (g_catchers.count(id)) {
                    res.set_content(std::string("{\"ok\":true,\"exists\":true,\"id\":\"") + js_esc(id) + "\"}", "application/json");
                    return;
                }
            }
            int slot = -1;
            {
                std::lock_guard<std::mutex> lk(g_catcher_slot_mtx);
                for (int i = 0; i < MAX_CATCHERS; i++) {
                    if (!g_catcher_slots[i]) { slot = i; break; }
                }
            }
            if (slot < 0) { res.set_content("{\"ok\":false,\"error\":\"Maximum active catchers reached\"}", "application/json"); return; }
            auto entry = std::make_shared<catcher_entry>();
            entry->id = id; entry->asm_ = asm_; entry->ns = ns; entry->cls = cls; entry->method = meth;
            entry->slot_idx = slot;
            Class c(ns.c_str(), cls.c_str(), Image(asm_.c_str()));
            if (!c.IsValid()) { res.set_content("{\"ok\":false,\"error\":\"Target class not found\"}", "application/json"); return; }
            MethodBase mb = c.GetMethod(meth.c_str());
            if (!mb.IsValid()) { res.set_content("{\"ok\":false,\"error\":\"Target method not found\"}", "application/json"); return; }
            auto* mi = mb.GetInfo();
            if (!mi || !is_valid_pointer(mi) || !mi->methodPointer) { res.set_content("{\"ok\":false,\"error\":\"No compiled pointer for method\"}", "application/json"); return; }
            entry->ret_type = (mi->return_type && is_valid_pointer(mi->return_type)) ? type_name(mi->return_type) : "?";
            int pcount = (int)mi->parameters_count;
            for (int i = 0; i < pcount; i++) {
                entry->param_types.push_back(get_param_type_name(mi, i));
                entry->param_names.push_back(get_param_name(mi, i));
            }
            {
                std::lock_guard<std::mutex> lk(g_catcher_slot_mtx);
                g_catcher_slots[slot] = new std::shared_ptr<catcher_entry>(entry);
            }
            InvokeHook(mb, g_catcher_hook_fns[slot], g_catcher_orig_ptrs[slot]);
            {
                std::lock_guard<std::mutex> lk(g_catcher_mtx);
                g_catchers[id] = entry;
            }
            std::ostringstream j;
            j << "{\"ok\":true,\"id\":\"" << js_esc(id) << "\"}";
            res.set_content(j.str(), "application/json");
        });

        svr.Post("/api/catcher/unhook",[](const httplib::Request& req, httplib::Response& res) {
            std::string id = json_val(req.body, "id");
            std::lock_guard<std::mutex> lk(g_catcher_mtx);
            auto it = g_catchers.find(id);
            if (it != g_catchers.end()) {
                it->second->active = false;
                int sl = it->second->slot_idx;
                if (sl >= 0 && sl < MAX_CATCHERS) {
                    std::lock_guard<std::mutex> lk2(g_catcher_slot_mtx);
                    delete g_catcher_slots[sl];
                    g_catcher_slots[sl] = nullptr;
                }
                g_catchers.erase(it);
            }
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Post("/api/catcher/clear",[](const httplib::Request& req, httplib::Response& res) {
            std::string id = json_val(req.body, "id");
            std::lock_guard<std::mutex> lk(g_catcher_mtx);
            auto it = g_catchers.find(id);
            if (it != g_catchers.end()) {
                std::lock_guard<std::mutex> lk2(it->second->calls_mtx);
                it->second->calls.clear();
            }
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Get("/api/catcher/list",[](const httplib::Request&, httplib::Response& res) {
            std::lock_guard<std::mutex> lk(g_catcher_mtx);
            std::ostringstream j;
            j << "[";
            bool first = true;
            for (auto& kv : g_catchers) {
                auto& e = kv.second;
                if (!first) j << ","; first = false;
                std::lock_guard<std::mutex> lk2(e->calls_mtx);
                j << "{\"id\":\"" << js_esc(e->id) << "\",\"ns\":\"" << js_esc(e->ns) << "\",\"cls\":\"" << js_esc(e->cls)
                  << "\",\"method\":\"" << js_esc(e->method) << "\",\"ret\":\"" << js_esc(e->ret_type)
                  << "\",\"count\":" << e->calls.size() << ",\"params\":[";
                for (size_t i = 0; i < e->param_types.size(); i++) {
                    if (i>0) j<<",";
                    j << "{\"t\":\"" << js_esc(e->param_types[i]) << "\",\"n\":\"" << js_esc(e->param_names[i]) << "\"}";
                }
                j << "]}";
            }
            j << "]";
            res.set_content(j.str(), "application/json");
        });

        svr.Get("/api/catcher/calls",[](const httplib::Request& req, httplib::Response& res) {
            std::string id = req.get_param_value("id");
            std::lock_guard<std::mutex> lk(g_catcher_mtx);
            auto it = g_catchers.find(id);
            if (it == g_catchers.end()) { res.set_content("[]", "application/json"); return; }
            auto& e = it->second;
            std::lock_guard<std::mutex> lk2(e->calls_mtx);
            std::ostringstream j;
            j << "[";
            bool first = true;
            for (auto& c : e->calls) {
                if (!first) j << ","; first = false;
                j << "{\"ts\":" << std::fixed << c.ts << ",\"instance\":\"" << js_esc(c.instance) << "\",\"args\":[";
                for (size_t i = 0; i < c.args.size(); i++) {
                    if (i>0) j<<",";
                    j << "{\"t\":\"" << js_esc(c.args[i].type) << "\",\"v\":" << c.args[i].val << "}";
                }
                j << "],\"ret\":\"" << js_esc(c.ret) << "\"}";
            }
            j << "]";
            res.set_content(j.str(), "application/json");
        });

        svr.Post("/api/lua/exec",[](const httplib::Request& req, httplib::Response& res) {
            try {
                std::string code = json_val(req.body, "code");
                if (code.empty()) { res.set_content("{\"output\":\"Error: empty code\"}", "application/json"); return; }
                std::string out = execute_lua_bnm(code);
                std::ostringstream j;
                j << "{\"output\":\"" << js_esc(out) << "\"}";
                res.set_content(j.str(), "application/json");
            } catch (...) {
                res.set_content("{\"output\":\"Error: native crash in executor\"}", "application/json");
            }
        });

        svr.Post("/api/lua/stop",[](const httplib::Request&, httplib::Response& res) {
            stop_lua_bnm();
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Post("/api/lua/reset",[](const httplib::Request&, httplib::Response& res) {
            reset_lua_bnm();
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Get("/api/lua/poll",[](const httplib::Request&, httplib::Response& res) {
            std::ostringstream j;
            j << "{\"output\":\"" << js_esc(poll_lua_bnm_output()) << "\"}";
            res.set_content(j.str(), "application/json");
        });

        register_dump_routes(svr);
        register_calls_routes(svr);
        register_freeze_routes(svr);
        register_workspace_routes(svr);

        svr.listen("0.0.0.0", g_port);
    }

    static std::mutex method_finder_log_mutex;
    static std::unordered_set<std::string> method_finder_logged_names;
    static std::unordered_set<std::string> method_finder_resolved_names;

    static void log_method_finder_resolved_once(const char* name, const char* runtime_name, void* address) {
        if (!name || !address) return;
        std::lock_guard<std::mutex> lock(method_finder_log_mutex);
        if (!method_finder_resolved_names.emplace(name).second) return;
        Dl_info info{};
        if (dladdr(address, &info) && info.dli_fbase) {
            const char* file_name = info.dli_fname ? info.dli_fname : "?";
            const char* short_name = std::strrchr(file_name, '/');
            LOGI("IL2CPP finder: %s -> %s at %s+0x%zx", name, runtime_name, short_name ? short_name + 1 : file_name,
                 static_cast<size_t>(reinterpret_cast<uintptr_t>(address) - reinterpret_cast<uintptr_t>(info.dli_fbase)));
        } else {
            LOGI("IL2CPP finder: %s -> %s at %p", name, runtime_name, address);
        }
    }

    static void log_method_finder_failure_once(const char* name, const char* runtime_name) {
        if (!name) return;
        std::lock_guard<std::mutex> lock(method_finder_log_mutex);
        if (!method_finder_logged_names.emplace(name).second) return;
        if (runtime_name) LOGW("IL2CPP method lookup failed for %s (mapped runtime name: %s).", name, runtime_name);
        else LOGW("IL2CPP method lookup failed for %s: no runtime-name mapping exists and the canonical name is not used with a renamed export table.", name);
    }

    static void* il2cpp_name_map_finder(const char* name, void*) {
        void* handle = BNM::Loading::GetIl2CppLibraryHandle();
        if (!handle || !name) return nullptr;
        std::string runtime_name;
        if (il2cpp_autodetect::find_runtime_name(name, runtime_name)) {
            void* mapped = BNM_dlsym(handle, runtime_name.c_str());
            if (!mapped) log_method_finder_failure_once(name, runtime_name.c_str());
            else log_method_finder_resolved_once(name, runtime_name.c_str(), mapped);
            return mapped;
        }
        if (il2cpp_autodetect::runtime_name_count() > 0) {
            log_method_finder_failure_once(name, nullptr);
            return nullptr;
        }
        void* canonical = BNM_dlsym(handle, name);
        if (!canonical) log_method_finder_failure_once(name, nullptr);
        return canonical;
    }

    static std::atomic<bool> bnm_loaded_flag{false};

    static void on_loaded() {
        bnm_loaded_flag.store(true);
        LOGI("BNM loaded successfully on the IL2CPP thread; starting BNMExplorer services.");
        hook_debug_log();
        std::thread(start_server).detach();
    }

    static void il2cpp_late_load_watchdog() {
        using domain_get_function = void* (*)();
        using thread_attach_function = void* (*)(void*);
        using thread_detach_function = void (*)(void*);
        std::this_thread::sleep_for(std::chrono::seconds(8));
        bool reported_waiting = false;
        for (int attempt = 0; attempt < 52 && !bnm_loaded_flag.load(); ++attempt) {
            auto domain_get = reinterpret_cast<domain_get_function>(il2cpp_name_map_finder("il2cpp_domain_get", nullptr));
            auto thread_attach = reinterpret_cast<thread_attach_function>(il2cpp_name_map_finder("il2cpp_thread_attach", nullptr));
            auto thread_detach = reinterpret_cast<thread_detach_function>(il2cpp_name_map_finder("il2cpp_thread_detach", nullptr));
            void* domain = domain_get ? domain_get() : nullptr;
            if (domain && thread_attach) {
                if (bnm_loaded_flag.load()) return;
                LOGW("IL2CPP is initialized but the il2cpp_init hook did not load BNM; loading BNM through the users finder.");
                void* attached = thread_attach(domain);
                BNM::Loading::TrySetupByUsersFinder();
                if (attached && thread_detach) thread_detach(attached);
                if (!bnm_loaded_flag.load()) LOGE("BNM manual load finished without the on-loaded callback.");
                return;
            }
            if (!reported_waiting) {
                LOGI("IL2CPP domain is not available yet (domain_get=%p); waiting for the il2cpp_init hook.", reinterpret_cast<void*>(domain_get));
                reported_waiting = true;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (!bnm_loaded_flag.load()) LOGE("BNM has not finished loading 60 s after startup.");
    }

#ifndef BNM_INIT_ENTRY
#define BNM_INIT_ENTRY BNM_Init
#endif

    extern "C" JNIEXPORT jint JNICALL BNM_INIT_ENTRY(JavaVM* vm, [[maybe_unused]] void* reserved) {
        JNIEnv* env = nullptr;
        if (!vm || vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK || !env) return JNI_ERR;
        BNM::Loading::AddOnLoadedEvent(on_loaded);
        BNM::Loading::SetMethodFinder(il2cpp_name_map_finder, nullptr);
        const bool loading_started = BNM::Loading::TryLoadByJNI(env);
        if (loading_started) {
            LOGI("BNM TryLoadByJNI installed its IL2CPP init hook; waiting for the BNM on-loaded callback.");
            std::thread(il2cpp_late_load_watchdog).detach();
            return JNI_VERSION_1_6;
        }
        LOGE("BNM TryLoadByJNI could not resolve or hook il2cpp_init; BNM initialization failed.");
        return JNI_ERR;
    }

}