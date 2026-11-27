#pragma once
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <dlfcn.h>
#include <string>
#include <thread>
#include <vector>
#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

struct symbol_entry {
    uintptr_t address;
    MethodInfo* method;
};

struct symbol_index {
    std::vector<symbol_entry> entries;
    std::atomic<int> state{0};
};

static symbol_index g_symbols;
static const uintptr_t symbol_max_span = 0x8000;

static uintptr_t symbol_strip(uintptr_t raw) {
    return raw & 0x00FFFFFFFFFFFFFFULL;
}

static bool symbol_plausible(const void* p) {
    uintptr_t a = symbol_strip((uintptr_t)p);
    return a >= 0x10000 && a < 0x0001000000000000ULL;
}

static std::string symbol_class_name(Il2CppClass* k) {
    if (!k || !symbol_plausible(k) || !symbol_plausible(k->name)) return "?";
    std::string name = k->name;
    if (symbol_plausible(k->namespaze) && k->namespaze[0] != '\0') name = std::string(k->namespaze) + "." + name;
    return name;
}

static void symbols_build() {
    BNM::AttachIl2Cpp();
    std::vector<symbol_entry> out;
    for (auto& img : BNM::Image::GetImages()) {
        if (!img.IsValid()) continue;
        for (auto& cls : img.GetClasses(true)) {
            if (!cls.IsValid()) continue;
            for (auto& m : cls.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (mi && symbol_plausible(mi) && mi->methodPointer && !mi->is_generic)
                    out.push_back({symbol_strip((uintptr_t)mi->methodPointer), mi});
            }
        }
    }
    std::sort(out.begin(), out.end(), [](const symbol_entry& a, const symbol_entry& b) { return a.address < b.address; });
    out.erase(std::unique(out.begin(), out.end(), [](const symbol_entry& a, const symbol_entry& b) { return a.address == b.address; }), out.end());
    g_symbols.entries = std::move(out);
    g_symbols.state.store(2, std::memory_order_release);
}

static void symbols_ensure() {
    int expected = 0;
    if (g_symbols.state.compare_exchange_strong(expected, 1)) std::thread(symbols_build).detach();
}

static const symbol_entry* symbols_find(uintptr_t address) {
    if (g_symbols.state.load(std::memory_order_acquire) != 2) return nullptr;
    const auto& list = g_symbols.entries;
    auto it = std::upper_bound(list.begin(), list.end(), address, [](uintptr_t a, const symbol_entry& e) { return a < e.address; });
    if (it == list.begin()) return nullptr;
    --it;
    return address - it->address <= symbol_max_span ? &*it : nullptr;
}

static std::string symbols_module_offset(uintptr_t address) {
    Dl_info info{};
    char buf[64];
    if (dladdr((void*)address, &info) && info.dli_fname && info.dli_fbase) {
        const char* base = strrchr(info.dli_fname, '/');
        snprintf(buf, sizeof(buf), "+0x%llX", (unsigned long long)(address - (uintptr_t)info.dli_fbase));
        std::string name = info.dli_sname ? info.dli_sname : (base ? base + 1 : info.dli_fname);
        return name + buf;
    }
    snprintf(buf, sizeof(buf), "0x%llX", (unsigned long long)address);
    return buf;
}

static std::string symbols_describe(uintptr_t raw) {
    uintptr_t address = symbol_strip(raw);
    if (!address) return "?";
    if (const symbol_entry* e = symbols_find(address)) {
        char buf[32];
        snprintf(buf, sizeof(buf), "+0x%llX", (unsigned long long)(address - e->address));
        std::string name = symbol_plausible(e->method->name) ? e->method->name : "?";
        return symbol_class_name(bnm_method_class(e->method)) + "::" + name + (address == e->address ? "" : buf);
    }
    return symbols_module_offset(address);
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
