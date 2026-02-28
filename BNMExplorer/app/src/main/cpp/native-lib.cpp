#include <jni.h>
#include <android/log.h>
#include <thread>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include "Include/BNMResolve.hpp"
#include "Include/httplib.h"
#include "explorer/html_page.hpp"

#define TAG "BNMExplorer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

using namespace BNM;
using namespace BNM::IL2CPP;
using namespace BNM::Structures::Unity;

static int g_port = 8080;

static std::string jsEsc(const char* s) {
    if (!s) return "";
    std::string o;
    for (const char* p = s; *p; ++p) {
        if      (*p == '"')  o += "\\\"";
        else if (*p == '\\') o += "\\\\";
        else if (*p == '\n') o += "\\n";
        else if (*p == '\r') o += "\\r";
        else                 o += *p;
    }
    return o;
}

static std::string jsEsc(const std::string& s) { return jsEsc(s.c_str()); }

static std::string jsonVal(const std::string& json, const std::string& key) {
    std::string qk = "\"" + key + "\":";
    auto pos = json.find(qk);
    if (pos == std::string::npos) return "";
    pos += qk.length();
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    if (pos < json.length() && json[pos] == '"') {
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
                char n = raw[i+1];
                if      (n == '"')  { out += '"';  i++; }
                else if (n == '\\') { out += '\\'; i++; }
                else if (n == 'n')  { out += '\n'; i++; }
                else if (n == 'r')  { out += '\r'; i++; }
                else out += raw[i];
            } else out += raw[i];
        }
        return out;
    }
    auto end = json.find_first_of(",}", pos);
    if (end == std::string::npos) return "";
    std::string v = json.substr(pos, end - pos);
    while (!v.empty() && (v.back() == ' ' || v.back() == '\t' || v.back() == '\r' || v.back() == '\n')) v.pop_back();
    return v;
}

static std::string typeName(const Il2CppType* t) {
    if (!t) return "?";
    Class c(t);
    if (!c.IsValid()) return "?";
    auto* r = c.GetClass();
    if (!r || !r->name) return "?";
    std::string n = r->name;
    if (r->namespaze && strlen(r->namespaze) > 0)
        n = std::string(r->namespaze) + "." + n;
    return n;
}

static MethodBase findObjsMethod() {
    static MethodBase m;
    if (m.IsValid()) return m;
    Class oc("UnityEngine", "Object");
    for (auto& mt : oc.GetMethods(false)) {
        auto* mi = mt.GetInfo();
        if (mi && mi->name && strcmp(mi->name, "FindObjectsOfType") == 0 && mi->parameters_count == 1)
            if (mi->parameters[0] && strstr(typeName(mi->parameters[0]).c_str(), "Type"))
            { m = mt; break; }
    }
    return m;
}

static Il2CppObject* sysTypeOf(const std::string& aqn) {
    Class tc("System", "Type", Image("mscorlib.dll"));
    if (!tc.IsValid()) return nullptr;
    Method<Il2CppObject*> gt(tc.GetMethod("GetType", 1));
    return gt.IsValid() ? gt(CreateMonoString(aqn.c_str())) : nullptr;
}

static std::string assembliesJson() {
    std::ostringstream j;
    j << "[";
    bool first = true;
    for (auto& img : Image::GetImages()) {
        auto* d = img.GetInfo();
        if (!img.IsValid() || !d || !d->name) continue;
        if (!first) j << ",";
        first = false;
        j << "\"" << jsEsc(d->name) << "\"";
    }
    j << "]";
    return j.str();
}

static std::string classesJson(const std::string& asm_) {
    Image img(asm_);
    if (!img.IsValid()) return "[]";
    std::ostringstream j;
    j << "[";
    bool first = true;
    for (auto& cls : img.GetClasses(false)) {
        auto* r = cls.GetClass();
        if (!cls.IsValid() || !r || !r->name) continue;
        if (!first) j << ",";
        first = false;
        std::string t = "class";
        if (r->enumtype) t = "enum";
        else if (r->byval_arg.valuetype) t = "struct";
        else if (r->flags & 0x20) t = "interface";
        j << "{\"name\":\"" << jsEsc(r->name) << "\",\"ns\":\"" << jsEsc(r->namespaze ? r->namespaze : "") << "\",\"t\":\"" << t << "\"}";
    }
    j << "]";
    return j.str();
}

static std::string allClassesJson() {
    std::ostringstream j;
    j << "[";
    bool first = true;
    for (auto& img : Image::GetImages()) {
        auto* d = img.GetInfo();
        if (!img.IsValid() || !d || !d->name) continue;
        std::string a = d->name;
        for (auto& cls : img.GetClasses(false)) {
            auto* r = cls.GetClass();
            if (!cls.IsValid() || !r || !r->name) continue;
            if (!first) j << ",";
            first = false;
            std::string t = "class";
            if (r->enumtype) t = "enum";
            else if (r->byval_arg.valuetype) t = "struct";
            else if (r->flags & 0x20) t = "interface";
            j << "{\"name\":\"" << jsEsc(r->name) << "\",\"ns\":\"" << jsEsc(r->namespaze ? r->namespaze : "") << "\",\"t\":\"" << t << "\",\"a\":\"" << jsEsc(a) << "\"}";
        }
    }
    j << "]";
    return j.str();
}

static std::string classDetailJson(const std::string& a, const std::string& ns, const std::string& cn) {
    Class cls(ns.c_str(), cn.c_str(), Image(a.c_str()));
    if (!cls.IsValid()) return "{}";
    auto* r = cls.GetClass();
    if (!r) return "{}";

    std::ostringstream j;
    j << "{\"name\":\"" << jsEsc(cn) << "\",\"ns\":\"" << jsEsc(ns) << "\",\"asm\":\"" << jsEsc(a) << "\",";
    j << (r->parent && r->parent->name ? "\"parent\":\"" + jsEsc(r->parent->name) + "\"," : "\"parent\":null,");

    j << "\"fields\":[";
    bool first = true;
    for (auto& f : cls.GetFields(false)) {
        auto* fi = f.GetInfo();
        if (!f.IsValid() || !fi || !fi->name) continue;
        if (!first) j << ",";
        first = false;
        j << "{\"name\":\"" << jsEsc(fi->name) << "\",\"type\":\"" << jsEsc(typeName(fi->type)) << "\",\"s\":" << (fi->type && (fi->type->attrs & 0x10) ? "true" : "false") << ",\"off\":" << fi->offset << "}";
    }

    j << "],\"methods\":[";
    first = true;
    for (auto& m : cls.GetMethods(false)) {
        auto* mi = m.GetInfo();
        if (!m.IsValid() || !mi || !mi->name) continue;
        if (!first) j << ",";
        first = false;
        char ab[32] = {};
        if (mi->methodPointer) snprintf(ab, sizeof(ab), "%llX", (unsigned long long)(uintptr_t)mi->methodPointer);
        j << "{\"name\":\"" << jsEsc(mi->name) << "\",\"ret\":\"" << jsEsc(typeName(mi->return_type)) << "\",\"s\":" << ((mi->flags & 0x10) ? "true" : "false") << ",\"addr\":\"" << ab << "\",\"params\":[";
        for (int i = 0; i < (int)mi->parameters_count; i++) {
            if (i > 0) j << ",";
            j << "{\"n\":\"arg" << i << "\",\"t\":\"" << jsEsc(mi->parameters && mi->parameters[i] ? typeName(mi->parameters[i]) : "?") << "\"}";
        }
        j << "]}";
    }

    j << "],\"props\":[";
    first = true;
    for (auto& p : cls.GetProperties(false)) {
        auto* pi = p._data;
        if (!p.IsValid() || !pi || !pi->name) continue;
        if (!first) j << ",";
        first = false;
        j << "{\"name\":\"" << jsEsc(pi->name) << "\",\"type\":\"" << jsEsc(pi->get && pi->get->return_type ? typeName(pi->get->return_type) : "?") << "\",\"g\":" << (pi->get ? "true" : "false") << ",\"s\":" << (pi->set ? "true" : "false") << "}";
    }
    j << "]}";
    return j.str();
}

struct InvokeResult { bool ok; std::string val, err; };

static InvokeResult invokeMethod(const std::string& body) {
    InvokeResult res = {false, "", ""};
    std::string a = jsonVal(body, "asm"), ns = jsonVal(body, "ns"), cn = jsonVal(body, "cls"), mn = jsonVal(body, "method");
    bool isStatic = jsonVal(body, "static") == "true" || jsonVal(body, "static") == "1";
    uintptr_t instAddr = (uintptr_t)strtoull(jsonVal(body, "instance").c_str(), nullptr, 16);

    std::vector<std::string> argT, argV;
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
                argT.push_back(jsonVal(e, "t"));
                argV.push_back(jsonVal(e, "v"));
                p = cb + 1;
            }
        }
    }

    if (a.empty() || cn.empty() || mn.empty()) { res.err = "Missing asm/cls/method"; return res; }

    Class cls(ns.c_str(), cn.c_str(), Image(a.c_str()));
    if (!cls.IsValid()) { res.err = "Class not found"; return res; }

    MethodBase mb = cls.GetMethod(mn.c_str(), (int)argT.size());
    if (!mb.IsValid()) mb = cls.GetMethod(mn.c_str());
    if (!mb.IsValid()) {
        for (Class cur = cls.GetParent(); cur.IsValid() && cur.GetClass() && !mb.IsValid(); cur = cur.GetParent()) {
            mb = cur.GetMethod(mn.c_str(), (int)argT.size());
            if (!mb.IsValid()) mb = cur.GetMethod(mn.c_str());
        }
    }
    if (!mb.IsValid()) { res.err = "Method not found"; return res; }
    auto* mi = mb.GetInfo();
    if (!mi || !mi->methodPointer) { res.err = "No pointer"; return res; }

    std::vector<void*> ptrs;
    std::vector<int> vi; std::vector<float> vf; std::vector<uint8_t> vb;
    std::vector<Vector3> vv3; std::vector<Color> vc;
    std::vector<BNM::Structures::Mono::String*> vs;

    for (size_t i = 0; i < argT.size(); i++) {
        auto& t = argT[i]; auto& v = argV[i];
        if (t=="System.Int32"||t=="Int32"||t=="int")                                                        { vi.push_back(std::stoi(v.empty()?"0":v)); ptrs.push_back(&vi.back()); }
        else if (t=="System.Single"||t=="Single"||t=="float"||t=="System.Double"||t=="double")              { vf.push_back(std::stof(v.empty()?"0":v)); ptrs.push_back(&vf.back()); }
        else if (t=="System.Boolean"||t=="Boolean"||t=="bool")                                              { vb.push_back((uint8_t)(v=="true"||v=="1"?1:0)); ptrs.push_back(&vb.back()); }
        else if (t=="UnityEngine.Vector3"||t=="Vector3")                                                    { float x=0,y=0,z=0; sscanf(v.c_str(),"[%f,%f,%f]",&x,&y,&z); vv3.push_back({x,y,z}); ptrs.push_back(&vv3.back()); }
        else if (t=="UnityEngine.Color"||t=="Color")                                                        { float r=1,g=1,b=1,aa=1; sscanf(v.c_str(),"[%f,%f,%f,%f]",&r,&g,&b,&aa); vc.push_back({r,g,b,aa}); ptrs.push_back(&vc.back()); }
        else if (t=="System.String"||t=="String"||t=="string")                                              { vs.push_back(CreateMonoString(v)); ptrs.push_back(vs.back()); }
        else                                                                                                 { vi.push_back(std::stoi(v.empty()?"0":v)); ptrs.push_back(&vi.back()); }
    }

    void* inst = isStatic ? nullptr : (void*)instAddr;
    std::string rtn = typeName(mi->return_type);
    std::ostringstream out;

    try {
        if (rtn=="System.Void"||rtn=="Void"||rtn=="void"||rtn=="?") {
            typedef void(*F)(void*,...);
            auto fn = (F)mi->methodPointer;
            switch(ptrs.size()) { case 0:fn(inst);break; case 1:fn(inst,ptrs[0]);break; case 2:fn(inst,ptrs[0],ptrs[1]);break; case 3:fn(inst,ptrs[0],ptrs[1],ptrs[2]);break; case 4:fn(inst,ptrs[0],ptrs[1],ptrs[2],ptrs[3]);break; default:fn(inst);break; }
            out << "void";
        } else if (rtn=="System.Single"||rtn=="Single"||rtn=="float"||rtn=="double") {
            typedef float(*F)(void*,...); float r=0;
            switch(ptrs.size()){case 0:r=((F)mi->methodPointer)(inst);break;case 1:r=((F)mi->methodPointer)(inst,ptrs[0]);break;default:r=((F)mi->methodPointer)(inst);break;}
            out << r;
        } else if (rtn=="System.Int32"||rtn=="Int32"||rtn=="int") {
            typedef int(*F)(void*,...); int r=0;
            switch(ptrs.size()){case 0:r=((F)mi->methodPointer)(inst);break;case 1:r=((F)mi->methodPointer)(inst,ptrs[0]);break;default:r=((F)mi->methodPointer)(inst);break;}
            out << r;
        } else if (rtn=="System.Boolean"||rtn=="Boolean"||rtn=="bool") {
            typedef bool(*F)(void*,...); bool r=false;
            switch(ptrs.size()){case 0:r=((F)mi->methodPointer)(inst);break;case 1:r=((F)mi->methodPointer)(inst,ptrs[0]);break;default:r=((F)mi->methodPointer)(inst);break;}
            out << (r?"true":"false");
        } else if (rtn=="UnityEngine.Vector3"||rtn=="Vector3") {
            typedef Vector3(*F)(void*,...);
            Vector3 r=((F)mi->methodPointer)(inst);
            char buf[128]; snprintf(buf,sizeof(buf),"[%.4f, %.4f, %.4f]",r.x,r.y,r.z); out<<buf;
        } else if (rtn=="System.String"||rtn=="String"||rtn=="string") {
            typedef BNM::Structures::Mono::String*(*F)(void*,...);
            auto* r=((F)mi->methodPointer)(inst);
            out << (r?r->str():"null");
        } else {
            typedef void*(*F)(void*,...);
            void* r=((F)mi->methodPointer)(inst);
            char buf[64]; snprintf(buf,sizeof(buf),"0x%llX",(unsigned long long)(uintptr_t)r);
            out << rtn << " @ " << buf;
        }
        res.ok = true; res.val = out.str();
    } catch(...) { res.err = "Exception during invocation"; }
    return res;
}

static std::string instancesJson(const std::string& a, const std::string& ns, const std::string& cn) {
    std::string full = ns.empty() ? cn : ns + "." + cn;
    std::string aNoExt = a;
    auto dp = aNoExt.find(".dll");
    if (dp != std::string::npos) aNoExt = aNoExt.substr(0, dp);

    Il2CppObject* st = sysTypeOf(full + ", " + aNoExt);
    if (!st) st = sysTypeOf(full);
    if (!st) return "{\"error\":\"Class not found / System.Type missing\"}";

    auto fm = findObjsMethod();
    if (!fm.IsValid()) return "{\"error\":\"FindObjectsOfType missing\"}";

    std::ostringstream j;
    j << "{\"instances\":[";
    try {
        auto* arr = Method<Array<Il2CppObject*>*>(fm)(st);
        if (arr && arr->capacity > 0) {
            bool first = true;
            Method<BNM::Structures::Mono::String*> gn(Class("UnityEngine","Object").GetMethod("get_name"));
            for (int i = 0; i < arr->capacity; i++) {
                Il2CppObject* obj = arr->m_Items[i];
                if (!obj) continue;
                if (!first) j << ",";
                first = false;
                std::string name = "obj";
                if (gn.IsValid()) { auto* s = gn[obj](); if (s) name = s->str(); }
                char addr[32]; snprintf(addr, sizeof(addr), "%llX", (unsigned long long)(uintptr_t)obj);
                j << "{\"addr\":\"" << addr << "\",\"name\":\"" << jsEsc(name) << "\"}";
            }
        }
    } catch(...) {}
    j << "]}";
    return j.str();
}

static std::string sceneJson() {
    auto fm = findObjsMethod();
    if (!fm.IsValid()) return "[]";

    Il2CppObject* st = sysTypeOf("UnityEngine.GameObject, UnityEngine.CoreModule");
    if (!st) st = sysTypeOf("UnityEngine.GameObject, UnityEngine");
    if (!st) st = sysTypeOf("UnityEngine.GameObject");
    if (!st) return "[]";

    Method<BNM::Structures::Mono::String*> gn(Class("UnityEngine","Object").GetMethod("get_name"));
    Method<bool> ga(Class("UnityEngine","GameObject").GetMethod("get_activeSelf"));
    Method<Il2CppObject*> gt(Class("UnityEngine","GameObject").GetMethod("get_transform"));
    Method<Il2CppObject*> tp(Class("UnityEngine","Transform").GetMethod("get_parent"));
    Method<Il2CppObject*> cg(Class("UnityEngine","Component").GetMethod("get_gameObject"));

    std::ostringstream j;
    j << "[";
    try {
        auto* arr = Method<Array<Il2CppObject*>*>(fm)(st);
        if (arr && arr->capacity > 0) {
            bool first = true;
            for (int i = 0; i < arr->capacity; i++) {
                Il2CppObject* go = arr->m_Items[i];
                if (!go) continue;
                std::string name = "Unknown";
                bool active = false;
                if (gn.IsValid()) { auto* s = gn[go](); if (s) name = s->str(); }
                if (ga.IsValid()) active = ga[go]();
                uintptr_t par = 0;
                if (gt.IsValid() && tp.IsValid() && cg.IsValid()) {
                    Il2CppObject* tr = gt[go]();
                    if (tr) { Il2CppObject* pt = tp[tr](); if (pt) { Il2CppObject* pg = cg[pt](); if (pg) par = (uintptr_t)pg; } }
                }
                if (!first) j << ",";
                first = false;
                j << "{\"addr\":\"" << std::hex << (uintptr_t)go << "\",\"name\":\"" << jsEsc(name) << "\",\"active\":" << (active?"true":"false") << ",\"parent\":\"" << std::hex << par << "\"}";
            }
        }
    } catch(...) {}
    j << "]";
    return j.str();
}

static std::string readField(const std::string& ft, Class& cls, Il2CppObject* inst, const std::string& name, MethodBase* getter, bool& ok) {
    ok = false;
    if (!inst) return "";
    try {
        if (ft=="System.Single"||ft=="Single"||ft=="float"||ft=="System.Double"||ft=="Double"||ft=="double") {
            float v = 0;
            if (getter) { Method<float> m(*getter); m.SetInstance(inst); v = m(); }
            else { Field<float> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);v=f();} }
            ok = true; return std::to_string(v);
        } else if (ft=="System.Int32"||ft=="Int32"||ft=="int") {
            int v = 0;
            if (getter) { Method<int> m(*getter); m.SetInstance(inst); v = m(); }
            else { Field<int> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);v=f();} }
            ok = true; return std::to_string(v);
        } else if (ft=="System.Boolean"||ft=="Boolean"||ft=="bool") {
            bool v = false;
            if (getter) { Method<bool> m(*getter); m.SetInstance(inst); v = m(); }
            else { Field<bool> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);v=f();} }
            ok = true; return v ? "true" : "false";
        } else if (ft=="System.String"||ft=="String"||ft=="string") {
            BNM::Structures::Mono::String* s = nullptr;
            if (getter) { Method<BNM::Structures::Mono::String*> m(*getter); m.SetInstance(inst); s = m(); }
            else { Field<BNM::Structures::Mono::String*> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);s=f();} }
            ok = true; return s ? "\"" + jsEsc(s->str()) + "\"" : "\"\"";
        } else if (ft=="UnityEngine.Vector3"||ft=="Vector3") {
            Vector3 v = {0,0,0};
            if (getter) { Method<Vector3> m(*getter); m.SetInstance(inst); v = m(); }
            else { Field<Vector3> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);v=f();} }
            ok = true; return "[" + std::to_string(v.x) + "," + std::to_string(v.y) + "," + std::to_string(v.z) + "]";
        } else if (ft=="UnityEngine.Color"||ft=="Color") {
            Color c = {0,0,0,0};
            if (getter) { Method<Color> m(*getter); m.SetInstance(inst); c = m(); }
            else { Field<Color> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);c=f();} }
            ok = true; return "[" + std::to_string(c.r) + "," + std::to_string(c.g) + "," + std::to_string(c.b) + "," + std::to_string(c.a) + "]";
        }
    } catch(...) {}
    return "";
}

static void writeField(const std::string& ft, Class& cls, Il2CppObject* inst, const std::string& name, MethodBase* setter, const std::string& v) {
    if (!inst) return;
    try {
        if (ft=="System.Single"||ft=="Single"||ft=="float"||ft=="System.Double"||ft=="Double"||ft=="double") {
            float val = std::stof(v);
            if (setter) { Method<void> m(*setter); m.SetInstance(inst); m(val); }
            else { Field<float> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);f=val;} }
        } else if (ft=="System.Int32"||ft=="Int32"||ft=="int") {
            int val = std::stoi(v);
            if (setter) { Method<void> m(*setter); m.SetInstance(inst); m(val); }
            else { Field<int> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);f=val;} }
        } else if (ft=="System.Boolean"||ft=="Boolean"||ft=="bool") {
            bool val = v=="true"||v=="1";
            if (setter) { Method<void> m(*setter); m.SetInstance(inst); m(val); }
            else { Field<bool> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);f=val;} }
        } else if (ft=="System.String"||ft=="String"||ft=="string") {
            auto* s = CreateMonoString(v.c_str());
            if (setter) { Method<void> m(*setter); m.SetInstance(inst); m(s); }
            else { Field<BNM::Structures::Mono::String*> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);f=s;} }
        } else if (ft=="UnityEngine.Vector3"||ft=="Vector3") {
            float x=0,y=0,z=0; sscanf(v.c_str(),"[%f,%f,%f]",&x,&y,&z); Vector3 val={x,y,z};
            if (setter) { Method<void> m(*setter); m.SetInstance(inst); m(val); }
            else { Field<Vector3> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);f=val;} }
        } else if (ft=="UnityEngine.Color"||ft=="Color") {
            float r=1,g=1,b=1,a=1; sscanf(v.c_str(),"[%f,%f,%f,%f]",&r,&g,&b,&a); Color val={r,g,b,a};
            if (setter) { Method<void> m(*setter); m.SetInstance(inst); m(val); }
            else { Field<Color> f=cls.GetField(name.c_str()); if(f.IsValid()){f.SetInstance(inst);f=val;} }
        }
    } catch(...) {}
}

static std::string goInfoJson(uintptr_t addr) {
    if (!addr) return "{}";
    Il2CppObject* go = (Il2CppObject*)addr;
    std::string name = "Unknown";
    bool active = false;

    Class goCls("UnityEngine","GameObject");
    Method<BNM::Structures::Mono::String*> gn(Class("UnityEngine","Object").GetMethod("get_name"));
    Method<bool> ga(goCls.GetMethod("get_activeSelf"));
    Method<Il2CppObject*> gt(goCls.GetMethod("get_transform"));

    try {
        if (gn.IsValid()) { auto* s=gn[go](); if(s) name=s->str(); }
        if (ga.IsValid()) active = ga[go]();
    } catch(...) {}

    std::ostringstream j;
    j << "{\"addr\":\"" << std::hex << addr << "\",\"name\":\"" << jsEsc(name) << "\",\"active\":" << (active?"true":"false") << ",\"transform\":{";
    try {
        if (gt.IsValid()) {
            Il2CppObject* tr = gt[go]();
            if (tr) {
                Method<Vector3> gp(Class("UnityEngine","Transform").GetMethod("get_localPosition"));
                Method<Vector3> gr(Class("UnityEngine","Transform").GetMethod("get_localEulerAngles"));
                Method<Vector3> gs(Class("UnityEngine","Transform").GetMethod("get_localScale"));
                Vector3 p=gp.IsValid()?gp[tr]():Vector3{0,0,0};
                Vector3 r=gr.IsValid()?gr[tr]():Vector3{0,0,0};
                Vector3 s=gs.IsValid()?gs[tr]():Vector3{0,0,0};
                j << "\"addr\":\"" << std::hex << (uintptr_t)tr << "\",\"p\":[" << p.x << "," << p.y << "," << p.z << "],\"r\":[" << r.x << "," << r.y << "," << r.z << "],\"s\":[" << s.x << "," << s.y << "," << s.z << "]";
            }
        }
    } catch(...) {}
    j << "}}";
    return j.str();
}

static void startServer() {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        int n = 0;
        for (auto& img : Image::GetImages()) if (img.IsValid()) n++;
        res.set_content(GetExplorerHTML(n), "text/html");
    });

    svr.Get("/api/assemblies", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(assembliesJson(), "application/json");
    });

    svr.Get("/api/classes", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content(classesJson(req.get_param_value("a")), "application/json");
    });

    svr.Get("/api/allclasses", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(allClassesJson(), "application/json");
    });

    svr.Get("/api/class", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content(classDetailJson(req.get_param_value("a"), req.get_param_value("ns"), req.get_param_value("n")), "application/json");
    });

    svr.Get("/api/instances", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content(instancesJson(req.get_param_value("a"), req.get_param_value("ns"), req.get_param_value("n")), "application/json");
    });

    svr.Get("/api/scene", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(sceneJson(), "application/json");
    });

    svr.Get("/api/scene/inspect", [](const httplib::Request& req, httplib::Response& res) {
        auto s = req.get_param_value("addr");
        res.set_content(s.empty() ? "{}" : goInfoJson((uintptr_t)strtoull(s.c_str(), nullptr, 16)), "application/json");
    });

    svr.Get("/api/controller/inspect", [](const httplib::Request& req, httplib::Response& res) {
        uintptr_t addr = (uintptr_t)strtoull(req.get_param_value("addr").c_str(), nullptr, 16);
        std::string a = req.get_param_value("asm"), ns = req.get_param_value("ns"), cn = req.get_param_value("cls");

        Class startCls(ns.c_str(), cn.c_str(), Image(a.c_str()));
        if (!startCls.IsValid()) { res.set_content("{}", "application/json"); return; }

        std::ostringstream j;
        j << "{\"addr\":\"" << std::hex << addr << "\",\"name\":\"" << jsEsc(cn) << "\",\"fields\":[";

        bool firstF = true;
        std::vector<std::string> seen;

        for (Class cur = startCls; cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
            auto* cn_ = cur.GetClass()->name;
            if (!cn_) break;
            if (strcmp(cn_, "Object") == 0 && cur.GetClass()->namespaze && strcmp(cur.GetClass()->namespaze, "System") == 0) break;
            if (!addr) continue;

            for (auto& f : cur.GetFields(false)) {
                try {
                    auto* fi = f.GetInfo();
                    if (!fi || !fi->type || (fi->type->attrs & 0x10)) continue;
                    std::string fn = fi->name;
                    bool dup = false; for (auto& s : seen) if (s==fn) dup=true;
                    if (dup) continue; seen.push_back(fn);
                    std::string ft = typeName(fi->type);
                    bool ok = false;
                    std::string vs = readField(ft, cur, (Il2CppObject*)addr, fn, nullptr, ok);
                    if (ok) { if (!firstF) j << ","; firstF = false; j << "{\"name\":\"" << jsEsc(fn) << "\",\"type\":\"" << ft << "\",\"val\":" << vs << ",\"isProp\":false,\"canWrite\":true}"; }
                } catch(...) {}
            }

            for (auto& p : cur.GetProperties(false)) {
                try {
                    auto* pi = p._data;
                    if (!pi || !pi->get) continue;
                    std::string pn = pi->name;
                    bool dup = false; for (auto& s : seen) if (s==pn) dup=true;
                    if (dup) continue; seen.push_back(pn);
                    std::string pt = typeName(pi->get->return_type);
                    bool ok = false;
                    MethodBase getter(pi->get);
                    std::string vs = readField(pt, cur, (Il2CppObject*)addr, pn, &getter, ok);
                    if (ok) { if (!firstF) j << ","; firstF = false; j << "{\"name\":\"" << jsEsc(pn) << "\",\"type\":\"" << pt << "\",\"val\":" << vs << ",\"isProp\":true,\"canWrite\":" << (pi->set?"true":"false") << "}"; }
                } catch(...) {}
            }
        }

        j << "],\"methods\":[";
        bool firstM = true;
        for (Class cur = startCls; cur.IsValid() && cur.GetClass(); cur = cur.GetParent()) {
            auto* cn_ = cur.GetClass()->name;
            if (!cn_) break;
            if (strcmp(cn_, "Object") == 0 && cur.GetClass()->namespaze && strcmp(cur.GetClass()->namespaze, "System") == 0) break;
            for (auto& m : cur.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (!m.IsValid() || !mi || !mi->name) continue;
                if (!firstM) j << ","; firstM = false;
                j << "{\"name\":\"" << jsEsc(mi->name) << "\",\"ret\":\"" << jsEsc(typeName(mi->return_type)) << "\",\"s\":" << ((mi->flags&0x10)?"true":"false") << ",\"params\":[";
                for (int i = 0; i < (int)mi->parameters_count; i++) {
                    if (i>0) j<<",";
                    j << "{\"n\":\"arg" << i << "\",\"t\":\"" << jsEsc(mi->parameters&&mi->parameters[i]?typeName(mi->parameters[i]):"?") << "\"}";
                }
                j << "]}";
            }
        }
        j << "]}";
        res.set_content(j.str(), "application/json");
    });

    svr.Post("/api/scene/update", [](const httplib::Request& req, httplib::Response& res) {
        uintptr_t addr = (uintptr_t)strtoull(jsonVal(req.body, "addr").c_str(), nullptr, 16);
        if (!addr) { res.set_content("{\"ok\":false}", "application/json"); return; }
        std::string type = jsonVal(req.body, "type"), prop = jsonVal(req.body, "prop"), val = jsonVal(req.body, "val");
        try {
            if (type == "gameobject") {
                if (prop == "active") { Method<void> m(Class("UnityEngine","GameObject").GetMethod("SetActive",1)); if(m.IsValid()) m[(Il2CppObject*)addr](val=="true"); }
                else if (prop == "name") { Method<void> m(Class("UnityEngine","Object").GetMethod("set_name",1)); if(m.IsValid()) m[(Il2CppObject*)addr](CreateMonoString(val)); }
            } else if (type == "transform") {
                float x=0,y=0,z=0; sscanf(val.c_str(),"[%f,%f,%f]",&x,&y,&z); Vector3 v={x,y,z};
                if (prop=="p") { Method<void> m(Class("UnityEngine","Transform").GetMethod("set_localPosition",1)); if(m.IsValid()) m[(Il2CppObject*)addr](v); }
                else if (prop=="r") { Method<void> m(Class("UnityEngine","Transform").GetMethod("set_localEulerAngles",1)); if(m.IsValid()) m[(Il2CppObject*)addr](v); }
                else if (prop=="s") { Method<void> m(Class("UnityEngine","Transform").GetMethod("set_localScale",1)); if(m.IsValid()) m[(Il2CppObject*)addr](v); }
            }
        } catch(...) {}
        res.set_content("{\"ok\":true}", "application/json");
    });

    svr.Post("/api/instance/update", [](const httplib::Request& req, httplib::Response& res) {
        uintptr_t addr = (uintptr_t)strtoull(jsonVal(req.body, "addr").c_str(), nullptr, 16);
        std::string a=jsonVal(req.body,"asm"), ns=jsonVal(req.body,"ns"), cn=jsonVal(req.body,"cls");
        std::string name=jsonVal(req.body,"name"), ft=jsonVal(req.body,"ftype"), val=jsonVal(req.body,"val");
        bool isProp = jsonVal(req.body,"isProp") == "true";
        if (addr) {
            try {
                for (Class cur(ns.c_str(),cn.c_str(),Image(a.c_str())); cur.IsValid(); cur = cur.GetParent()) {
                    if (isProp) {
                        auto p = cur.GetProperty(name.c_str());
                        if (p.IsValid() && p._data && p._data->set) { MethodBase s(p._data->set); writeField(ft,cur,(Il2CppObject*)addr,name,&s,val); break; }
                    } else {
                        auto f = cur.GetField(name.c_str());
                        if (f.IsValid()) { writeField(ft,cur,(Il2CppObject*)addr,name,nullptr,val); break; }
                    }
                }
            } catch(...) {}
        }
        res.set_content("{\"ok\":true}", "application/json");
    });

    svr.Post("/api/scene/delete", [](const httplib::Request& req, httplib::Response& res) {
        auto s = jsonVal(req.body, "addr");
        if (!s.empty()) {
            uintptr_t addr = (uintptr_t)strtoull(s.c_str(), nullptr, 16);
            if (addr) {
                try { Method<void> m(Class("UnityEngine","Object").GetMethod("Destroy",1)); if(m.IsValid()) m((Il2CppObject*)addr); } catch(...) {}
            }
        }
        res.set_content("{\"ok\":true}", "application/json");
    });

    svr.Post("/api/invoke", [](const httplib::Request& req, httplib::Response& res) {
        auto r = invokeMethod(req.body);
        std::ostringstream j;
        j << "{\"ok\":" << (r.ok?"true":"false") << ",\"value\":\"" << jsEsc(r.val) << "\",\"error\":\"" << jsEsc(r.err) << "\"}";
        res.set_content(j.str(), "application/json");
    });

    LOGI("BNM Explorer listening on 0.0.0.0:%d", g_port);
    svr.listen("0.0.0.0", g_port);
}

static void OnLoaded() {
    std::thread(startServer).detach();
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, [[maybe_unused]] void* reserved) {
    JNIEnv* env;
    vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    BNM::Loading::AddOnLoadedEvent(OnLoaded);
    BNM::Loading::TryLoadByJNI(env);
    return JNI_VERSION_1_6;
}