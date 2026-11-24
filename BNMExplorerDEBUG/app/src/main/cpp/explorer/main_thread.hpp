#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>
#include <sys/syscall.h>
#include <dobby.h>

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

struct mt_task {
    std::function<bool()> fn;
    bool urgent;
};

static std::mutex g_mt_mtx;
static std::deque<mt_task> g_mt_tasks;
static std::atomic<int> g_mt_pending{0};
static std::atomic<int> g_mt_tid{0};
static std::atomic<bool> g_mt_ready{false};
static std::atomic<bool> g_mt_failed{false};
static std::atomic<int64_t> g_mt_last_bg{0};
static std::mutex g_mt_install_mtx;
static std::string g_mt_error;
static std::string g_mt_hooked_name;

static int64_t mt_now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

static void mt_run_slice() {
    static thread_local bool busy = false;
    if (busy) return;
    busy = true;
    g_mt_tid.store((int)syscall(SYS_gettid));
    int64_t now = mt_now_ms();
    bool bg_allowed = now - g_mt_last_bg.load() >= 10;
    auto t0 = std::chrono::steady_clock::now();
    std::vector<mt_task> again;
    size_t guard = 0;
    for (;;) {
        mt_task t;
        {
            std::lock_guard<std::mutex> lk(g_mt_mtx);
            size_t pick = g_mt_tasks.size();
            for (size_t i = 0; i < g_mt_tasks.size(); i++) {
                if (g_mt_tasks[i].urgent || bg_allowed) {
                    pick = i;
                    break;
                }
            }
            if (pick == g_mt_tasks.size()) break;
            t = std::move(g_mt_tasks[pick]);
            g_mt_tasks.erase(g_mt_tasks.begin() + pick);
        }
        bool done = true;
        try {
            done = t.fn();
        } catch (...) {
        }
        if (!t.urgent) g_mt_last_bg.store(mt_now_ms());
        if (done) g_mt_pending.fetch_sub(1);
        else again.push_back(std::move(t));
        if (!t.urgent) bg_allowed = false;
        if (++guard > 256) break;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count() > 8) break;
    }
    if (!again.empty()) {
        std::lock_guard<std::mutex> lk(g_mt_mtx);
        for (auto& a : again) g_mt_tasks.push_back(std::move(a));
    }
    busy = false;
}

using mt_fn = void* (*)(void*, void*);
static const int mt_hook_slots = 3;
static mt_fn g_mt_orig[mt_hook_slots] = {};

template <int N>
static void* mt_hook(void* a0, void* a1) {
    void* r = g_mt_orig[N] ? g_mt_orig[N](a0, a1) : nullptr;
    if (g_mt_pending.load(std::memory_order_relaxed) > 0) mt_run_slice();
    return r;
}

static const mt_fn g_mt_fns[mt_hook_slots] = {mt_hook<0>, mt_hook<1>, mt_hook<2>};

static bool mt_install() {
    if (g_mt_ready.load()) return true;
    if (g_mt_failed.load()) return false;
    std::lock_guard<std::mutex> lk(g_mt_install_mtx);
    if (g_mt_ready.load()) return true;
    if (g_mt_failed.load()) return false;
    BNM::AttachIl2Cpp();
    struct cand {
        const char* ns;
        const char* cls;
        const char* method;
        int params;
    };
    static const cand cands[] = {
        {"UnityEngine", "Camera", "FireOnPreCull", 1},
        {"UnityEngine", "Camera", "FireOnPreRender", 1},
        {"UnityEngine", "Canvas", "SendWillRenderCanvases", 0},
    };
    int slot = 0;
    for (auto& c : cands) {
        if (slot >= mt_hook_slots) break;
        try {
            Class k(c.ns, c.cls);
            if (!k.IsValid()) continue;
            for (auto& m : k.GetMethods(false)) {
                auto* mi = m.GetInfo();
                if (!mi || !mi->name || !mi->methodPointer || mi->is_generic) continue;
                if (strcmp(mi->name, c.method) != 0 || (int)mi->parameters_count != c.params) continue;
                if (DobbyHook((void*)mi->methodPointer, (dobby_dummy_func_t)g_mt_fns[slot], (dobby_dummy_func_t*)&g_mt_orig[slot]) == RT_SUCCESS) {
                    g_mt_hooked_name += std::string(c.cls) + "." + c.method + " ";
                    slot++;
                }
                break;
            }
        } catch (...) {
        }
    }
    if (slot == 0) {
        g_mt_failed.store(true);
        g_mt_error = "no main thread hook point found";
        return false;
    }
    g_mt_ready.store(true);
    return true;
}

static bool mt_on_main_thread() {
    int t = g_mt_tid.load();
    return t != 0 && t == (int)syscall(SYS_gettid);
}

static void mt_post(std::function<bool()> fn, bool urgent) {
    if (!mt_install()) {
        std::thread([fn]() {
            BNM::AttachIl2Cpp();
            for (;;) {
                bool done = true;
                try {
                    done = fn();
                } catch (...) {
                }
                if (done) break;
                usleep(2000);
            }
        }).detach();
        return;
    }
    {
        std::lock_guard<std::mutex> lk(g_mt_mtx);
        g_mt_tasks.push_back({std::move(fn), urgent});
    }
    g_mt_pending.fetch_add(1);
}

struct mt_string_job {
    std::mutex m;
    std::condition_variable cv;
    bool done = false;
    std::string result;
};

static bool mt_call_string(std::function<std::string()> fn, std::string& out, int timeout_ms) {
    if (mt_on_main_thread() || !mt_install()) {
        out = fn();
        return true;
    }
    auto job = std::make_shared<mt_string_job>();
    mt_post([job, fn]() -> bool {
        std::string r;
        try {
            r = fn();
        } catch (...) {
        }
        {
            std::lock_guard<std::mutex> lk(job->m);
            job->result = std::move(r);
            job->done = true;
        }
        job->cv.notify_all();
        return true;
    }, true);
    std::unique_lock<std::mutex> lk(job->m);
    if (!job->cv.wait_for(lk, std::chrono::milliseconds(timeout_ms), [&] { return job->done; })) return false;
    out = job->result;
    return true;
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
