#pragma once
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <setjmp.h>
#include <signal.h>
#include <cstdio>
#include <ctime>
#include "lua_objects.hpp"
#include "main_thread.hpp"

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

struct lua_task {
    int id;
    int ref;
    double next_time;
    double interval;
    bool repeating;
    bool dead;
    bool game_start = false;
};

struct rt_deferred_cb {
    int cb_ref = 0;
    std::function<void(lua_State*)> build;
};

struct lua_runtime_state {
    lua_State* state = nullptr;
    std::recursive_mutex mtx;
    std::mutex out_mtx;
    std::vector<lua_task> tasks;
    std::vector<int> main_refs;
    std::vector<rt_deferred_cb> deferred;
    double main_pending_since = 0.0;
    std::string exec_output;
    std::string async_output;
    int next_id = 1;
    int exec_depth = 0;
    double deadline = 0.0;
    double last_pump = 0.0;
    bool worker_started = false;
    std::atomic<bool> main_hook_active{false};
    std::atomic<bool> prefer_main{false};
    std::atomic<bool> debug_pcall{false};
    std::atomic<bool> guard_calls{false};
    std::atomic<bool> file_log{true};
};

static lua_runtime_state g_rt;
static const size_t max_async_output = 65536;
static const double exec_timeout_seconds = 5.0;
static const double callback_timeout_seconds = 1.0;
static const char* rt_log_fallbacks[] = {
        "/sdcard/BNMExplorer/log.txt",
        "/storage/emulated/0/BNMExplorer/log.txt",
        "/data/local/tmp/BNMExplorer/log.txt"
};
static std::string g_rt_log_path;
static bool g_rt_log_probed = false;
static bool g_rt_log_failed = false;
static std::mutex g_rt_log_mtx;

static double rt_now() {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

static std::string rt_resolve_log_path() {
    if (!g_rt_log_path.empty()) return g_rt_log_path;
    for (const char* candidate : rt_log_fallbacks) {
        FILE* f = fopen(candidate, "a");
        if (f) {
            fclose(f);
            return candidate;
        }
    }
    return "";
}

static void rt_log_file(const std::string& line) {
    if (!g_rt.file_log.load()) return;
    std::lock_guard<std::mutex> lk(g_rt_log_mtx);
    if (g_rt_log_failed) return;
    std::string path = rt_resolve_log_path();
    if (path.empty()) {
        g_rt_log_failed = true;
        return;
    }
    struct stat st;
    if (stat(path.c_str(), &st) == 0 && st.st_size > 2 * 1024 * 1024) remove(path.c_str());
    FILE* f = fopen(path.c_str(), "a");
    if (!f) {
        g_rt_log_failed = true;
        return;
    }
    time_t t = time(nullptr);
    struct tm tmv;
    localtime_r(&t, &tmv);
    fprintf(f, "[%02d-%02d %02d:%02d:%02d] %s\n", tmv.tm_mon + 1, tmv.tm_mday, tmv.tm_hour, tmv.tm_min, tmv.tm_sec, line.c_str());
    fclose(f);
}

static void rt_append_output(const std::string& text) {
    std::lock_guard<std::mutex> lk(g_rt.out_mtx);
    std::string& target = g_rt.exec_depth > 0 ? g_rt.exec_output : g_rt.async_output;
    if (!target.empty()) target += "\n";
    target += text;
    if (g_rt.exec_depth == 0 && target.size() > max_async_output)
        target.erase(0, target.size() - max_async_output);
    rt_log_file(text);
}

static std::string rt_take_async_output() {
    std::lock_guard<std::mutex> lk(g_rt.out_mtx);
    std::string out;
    out.swap(g_rt.async_output);
    return out;
}

static std::string describe_value(lua_State* L, int idx) {
    int t = lua_type(L, idx);
    char buf[96];
    switch (t) {
        case LUA_TNIL: return "nil";
        case LUA_TBOOLEAN: return lua_toboolean(L, idx) ? "true" : "false";
        case LUA_TNUMBER:
            snprintf(buf, sizeof(buf), "%.14g", lua_tonumber(L, idx));
            return buf;
        case LUA_TSTRING: return lua_tostring(L, idx);
        default:
            if (luaL_callmeta(L, idx, "__tostring")) {
                const char* s = lua_tostring(L, -1);
                std::string out = s ? s : "?";
                lua_pop(L, 1);
                return out;
            }
            snprintf(buf, sizeof(buf), "%s: %p", luaL_typename(L, idx), lua_topointer(L, idx));
            return buf;
    }
}

static int l_print(lua_State* L) {
    int n = lua_gettop(L);
    std::string line;
    for (int i = 1; i <= n; i++) {
        if (i > 1) line += "\t";
        line += describe_value(L, i);
    }
    rt_append_output(line);
    return 0;
}

static int rt_traceback(lua_State* L) {
    if (!lua_isstring(L, 1)) return 1;
    const char* raw = lua_tostring(L, 1);
    std::string msg = raw ? raw : "unknown error";
    std::string tb = "\nstack traceback:";
    int level = 1;
    int shown = 0;
    lua_Debug ar;
    while (shown < 16 && lua_getstack(L, level, &ar)) {
        if (lua_getinfo(L, "Sln", &ar) && ar.short_src[0]) {
            char line[192];
            if (ar.currentline > 0)
                snprintf(line, sizeof(line), "\n    %s:%d: in %s", ar.short_src, ar.currentline,
                         ar.name && ar.namewhat[0] ? ar.name : "function");
            else
                snprintf(line, sizeof(line), "\n    %s: in %s", ar.short_src,
                         ar.what[0] == 'm' ? "main chunk" : (ar.name && ar.namewhat[0] ? ar.name : "C function"));
            tb += line;
            shown++;
        }
        level++;
    }
    if (shown == 0) tb += "    (no lua frames)";
    lua_pushfstring(L, "%s%s", msg.c_str(), tb.c_str());
    return 1;
}

static void rt_report_error(lua_State* L, const std::string& prefix) {
    std::string msg;
    if (lua_isstring(L, -1)) {
        const char* s = lua_tostring(L, -1);
        msg = s ? s : "unknown";
    } else {
        msg = std::string(luaL_typename(L, -1)) + " error object";
    }
    rt_append_output(prefix + msg);
}

static void rt_push_error_handler(lua_State* L) {
    lua_pushcfunction(L, rt_traceback);
}

static void rt_ensure_worker();

static void rt_queue_deferred(int cb_ref, std::function<void(lua_State*)> build) {
    std::lock_guard<std::recursive_mutex> lk(g_rt.mtx);
    rt_deferred_cb entry;
    entry.cb_ref = cb_ref;
    entry.build = std::move(build);
    g_rt.deferred.push_back(std::move(entry));
    rt_ensure_worker();
}

static void rt_defer_callback(lua_State* L, int cb_index, std::function<void(lua_State*)> build) {
    lua_pushvalue(L, cb_index);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    rt_queue_deferred(ref, std::move(build));
}

static void rt_run_deferred(lua_State* L) {
    std::vector<rt_deferred_cb> pending;
    {
        std::unique_lock<std::recursive_mutex> lk(g_rt.mtx, std::try_to_lock);
        if (!lk.owns_lock()) return;
        if (g_rt.deferred.empty()) return;
        pending.swap(g_rt.deferred);
    }
    for (auto& cb : pending) {
        rt_push_error_handler(L);
        lua_rawgeti(L, LUA_REGISTRYINDEX, cb.cb_ref);
        if (cb.build) cb.build(L);
        int top = lua_gettop(L);
        g_rt.deadline = rt_now() + callback_timeout_seconds;
        if (lua_pcall(L, top - 2, 0, 1)) {
            rt_report_error(L, "AsyncCallbackError: ");
            lua_pop(L, 2);
        } else {
            lua_pop(L, 1);
        }
        g_rt.deadline = 0.0;
        luaL_unref(L, LUA_REGISTRYINDEX, cb.cb_ref);
    }
}

static void rt_deadline_hook(lua_State* L, lua_Debug*) {
    if (g_rt.deadline > 0.0 && rt_now() > g_rt.deadline) {
        g_rt.deadline = 0.0;
        luaL_error(L, "script timeout");
    }
}

static lua_task* rt_find_task(int id) {
    for (auto& t : g_rt.tasks)
        if (t.id == id) return &t;
    return nullptr;
}

static bool rt_frame_count_ready() {
    static MethodInfo* mi = nullptr;
    static bool searched = false;
    if (!searched) {
        searched = true;
        Class time_cls("UnityEngine", "Time");
        if (time_cls.IsValid()) {
            const auto& cand = find_candidates(time_cls.GetClass(), "get_frameCount", 0);
            if (!cand.empty() && cand.front()->methodPointer) mi = cand.front();
        }
    }
    if (!mi) return true;
    char err[128];
    void* ret = invoke_raw(mi, nullptr, nullptr, err, sizeof(err));
    if (!ret) return false;
    return *(int*)((uint8_t*)ret + sizeof(Il2CppObject)) > 0;
}

static bool rt_should_wait_main() {
    return g_rt.prefer_main.load() && mt_pumping() && !mt_on_main_thread();
}

static void rt_pump_main_locked(lua_State* L) {
    if (!L || g_rt.main_refs.empty()) return;
    std::vector<int> refs;
    refs.swap(g_rt.main_refs);
    g_rt.main_pending_since = 0.0;
    double previous_deadline = g_rt.deadline;
    static bool fallback_warned = false;
    bool on_main = mt_on_main_thread();
    if (!on_main && !fallback_warned && mt_pumping()) {
        fallback_warned = true;
        rt_append_output("Warning: no main-thread hook point found, main_thread callbacks run on a worker thread");
    }
    for (int ref : refs) {
        rt_push_error_handler(L);
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        g_rt.deadline = rt_now() + callback_timeout_seconds;
        if (lua_pcall(L, 0, 0, -2)) {
            rt_report_error(L, on_main ? "MainThreadError: " : "MainThreadError (ran on worker thread): ");
            lua_pop(L, 2);
        } else {
            lua_pop(L, 1);
        }
        g_rt.deadline = previous_deadline;
    }
}

static void rt_run_task(lua_State* L, int id) {
    lua_task* t = rt_find_task(id);
    if (!t || t->dead) return;
    if (t->game_start && !rt_frame_count_ready()) {
        t->next_time = rt_now() + 0.25;
        return;
    }
    int ref = t->ref;
    rt_push_error_handler(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    g_rt.deadline = rt_now() + callback_timeout_seconds;
    int err = lua_pcall(L, 0, 0, -2);
    g_rt.deadline = 0.0;
    bool failed = false;
    if (err) {
        char label[64];
        snprintf(label, sizeof(label), "TaskError (task %d): ", id);
        rt_report_error(L, label);
        lua_pop(L, 2);
        failed = true;
    } else {
        lua_pop(L, 1);
    }
    t = rt_find_task(id);
    if (!t) return;
    if (failed || !t->repeating) {
        t->dead = true;
        return;
    }
    t->next_time = rt_now() + t->interval;
}

static void rt_pump_locked() {
    lua_State* L = g_rt.state;
    if (!L) return;
    if (rt_should_wait_main()) return;
    double now = rt_now();
    std::vector<int> due;
    for (auto& t : g_rt.tasks)
        if (!t.dead && now >= t.next_time) due.push_back(t.id);
    for (int id : due) rt_run_task(L, id);
    for (size_t i = 0; i < g_rt.tasks.size();) {
        if (g_rt.tasks[i].dead) {
            luaL_unref(L, LUA_REGISTRYINDEX, g_rt.tasks[i].ref);
            g_rt.tasks.erase(g_rt.tasks.begin() + i);
        } else {
            i++;
        }
    }
}

static void rt_pump_from_main();

static void rt_worker_loop() {
    BNM::AttachIl2Cpp();
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        if (g_rt.main_hook_active.load()) continue;
        std::unique_lock<std::recursive_mutex> lk(g_rt.mtx, std::try_to_lock);
        if (!lk.owns_lock()) continue;
        rt_pump_locked();
        rt_run_deferred(g_rt.state);
        if (!g_rt.main_refs.empty() && g_rt.main_pending_since > 0 && rt_now() - g_rt.main_pending_since > 3.0)
            rt_pump_main_locked(g_rt.state);
    }
}

static void rt_ensure_worker() {
    if (g_rt.worker_started) return;
    g_rt.worker_started = true;
    g_mt_slice_extra = rt_pump_from_main;
    std::thread(rt_worker_loop).detach();
}

static void rt_pump_from_main() {
    std::unique_lock<std::recursive_mutex> lk(g_rt.mtx, std::try_to_lock);
    if (!lk.owns_lock()) return;
    double now = rt_now();
    if (now - g_rt.last_pump < 0.004) return;
    g_rt.last_pump = now;
    rt_pump_locked();
    rt_run_deferred(g_rt.state);
    rt_pump_main_locked(g_rt.state);
}

static void (*g_tick_original)(void*) = nullptr;

static void tick_trampoline(void* self) {
    if (g_tick_original) g_tick_original(self);
    rt_pump_from_main();
}

static int add_task(lua_State* L, double delay, double interval, bool repeating, bool game_start = false) {
    luaL_checktype(L, -1, LUA_TFUNCTION);
    lua_task t;
    t.id = g_rt.next_id++;
    t.ref = luaL_ref(L, LUA_REGISTRYINDEX);
    t.next_time = rt_now() + delay;
    t.interval = interval;
    t.repeating = repeating;
    t.dead = false;
    t.game_start = game_start;
    g_rt.tasks.push_back(t);
    rt_ensure_worker();
    lua_pushnumber(L, t.id);
    return 1;
}

static int l_every(lua_State* L) {
    double seconds = luaL_checknumber(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    lua_settop(L, 2);
    lua_remove(L, 1);
    if (seconds < 0) seconds = 0;
    return add_task(L, seconds, seconds, true);
}

static int l_after(lua_State* L) {
    double seconds = luaL_checknumber(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    lua_settop(L, 2);
    lua_remove(L, 1);
    if (seconds < 0) seconds = 0;
    return add_task(L, seconds, 0.0, false);
}

static int l_on_update(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_settop(L, 1);
    return add_task(L, 0.0, 0.0, true);
}

static int l_on_game_start(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_settop(L, 1);
    return add_task(L, 0.1, 0.25, false, true);
}

static int l_main_thread(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_settop(L, 1);
    mt_install();
    lua_pushvalue(L, 1);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    std::lock_guard<std::recursive_mutex> lk(g_rt.mtx);
    g_rt.main_refs.push_back(ref);
    g_rt.main_pending_since = rt_now();
    rt_ensure_worker();
    return 0;
}

static int l_cancel(lua_State* L) {
    int id = (int)luaL_checknumber(L, 1);
    lua_task* t = rt_find_task(id);
    if (t) t->dead = true;
    lua_pushboolean(L, t != nullptr);
    return 1;
}

static int l_cancel_all(lua_State* L) {
    for (auto& t : g_rt.tasks) t.dead = true;
    return 0;
}

static int l_get_time(lua_State* L) {
    lua_pushnumber(L, rt_now());
    return 1;
}

static int l_set_tick_hook(lua_State* L) {
    const char* full_name = luaL_checkstring(L, 1);
    const char* method_name = luaL_checkstring(L, 2);
    const char* asm_name = lua_gettop(L) >= 3 && lua_isstring(L, 3) ? lua_tostring(L, 3) : "";
    if (g_rt.main_hook_active.load()) return luaL_error(L, "tick hook is already installed");
    Il2CppClass* k = find_class_cached(full_name, asm_name);
    if (!k) return luaL_error(L, "class '%s' not found", full_name);
    const auto& cand = find_candidates(k, method_name, 0);
    if (cand.empty()) return luaL_error(L, "method '%s' without arguments not found", method_name);
    MethodInfo* mi = cand.front();
    if (kind_from_type(mi->return_type) != value_kind::nothing) return luaL_error(L, "method '%s' must return void", method_name);
    bool ok = InvokeHook(MethodBase(mi), tick_trampoline, g_tick_original);
    if (!ok) return luaL_error(L, "failed to hook '%s'", method_name);
    g_rt.main_hook_active.store(true);
    lua_pushboolean(L, 1);
    return 1;
}

static int l_has_tick_hook(lua_State* L) {
    lua_pushboolean(L, g_rt.main_hook_active.load());
    return 1;
}

static thread_local sigjmp_buf* tls_guard_jmp = nullptr;
static thread_local volatile sig_atomic_t tls_guard_depth = 0;
static std::once_flag g_guard_once_flag;

static void rt_crash_handler(int sig, siginfo_t*, void*) {
    if (tls_guard_jmp && tls_guard_depth > 0) {
        tls_guard_depth = 0;
        siglongjmp(*tls_guard_jmp, sig == SIGBUS ? 2 : 1);
    }
    signal(sig, SIG_DFL);
    raise(sig);
}

static void rt_install_crash_handlers() {
    std::call_once(g_guard_once_flag, []() {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_sigaction = rt_crash_handler;
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGSEGV, &sa, nullptr);
        sigaction(SIGBUS, &sa, nullptr);
        sigaction(SIGILL, &sa, nullptr);
        sigaction(SIGFPE, &sa, nullptr);
    });
}

static bool rt_guard_begin(sigjmp_buf& jb) {
    rt_install_crash_handlers();
    if (sigsetjmp(jb, 1) != 0) {
        tls_guard_jmp = nullptr;
        return false;
    }
    tls_guard_jmp = &jb;
    tls_guard_depth = 1;
    return true;
}

static void rt_guard_end() {
    tls_guard_jmp = nullptr;
    tls_guard_depth = 0;
}

static void* rt_guarded_invoke(MethodInfo* mi, void* inst, void** args, Il2CppException** exc) {
    if (tls_guard_depth > 0 || !g_rt.guard_calls.load()) {
        return BNM::Internal::il2cppMethods.il2cpp_runtime_invoke(mi, inst, args, exc);
    }
    sigjmp_buf jb;
    if (!rt_guard_begin(jb)) {
        *exc = (Il2CppException*)1;
        return nullptr;
    }
    void* ret = BNM::Internal::il2cppMethods.il2cpp_runtime_invoke(mi, inst, args, exc);
    rt_guard_end();
    return ret;
}

static int l_safe_pcall(lua_State* L) {
    int base = lua_gettop(L);
    luaL_checktype(L, 1, LUA_TFUNCTION);
    int status;
    if (g_rt.debug_pcall.load()) {
        rt_push_error_handler(L);
        lua_insert(L, 1);
        status = lua_pcall(L, base - 1, LUA_MULTRET, 1);
        if (status) {
            lua_pushboolean(L, 0);
            lua_insert(L, 2);
            lua_remove(L, 1);
            rt_report_error(L, "PcallError: ");
            return 2;
        }
        lua_remove(L, 1);
        lua_pushboolean(L, 1);
        lua_insert(L, 1);
        return lua_gettop(L);
    }
    status = lua_pcall(L, base - 1, LUA_MULTRET, 0);
    lua_pushboolean(L, status == 0);
    lua_insert(L, 1);
    return lua_gettop(L);
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
