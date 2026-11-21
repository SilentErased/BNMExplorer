#pragma once
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <string>
#include "lua_objects.hpp"

struct lua_task {
    int id;
    int ref;
    double next_time;
    double interval;
    bool repeating;
    bool dead;
};

struct lua_runtime_state {
    lua_State* state = nullptr;
    std::recursive_mutex mtx;
    std::mutex out_mtx;
    std::vector<lua_task> tasks;
    std::string exec_output;
    std::string async_output;
    int next_id = 1;
    int exec_depth = 0;
    double deadline = 0.0;
    double last_pump = 0.0;
    bool worker_started = false;
    std::atomic<bool> main_hook_active{false};
};

static lua_runtime_state g_rt;
static const size_t max_async_output = 65536;
static const double exec_timeout_seconds = 5.0;
static const double callback_timeout_seconds = 1.0;

static double rt_now() {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

static void rt_append_output(const std::string& text) {
    std::lock_guard<std::mutex> lk(g_rt.out_mtx);
    std::string& target = g_rt.exec_depth > 0 ? g_rt.exec_output : g_rt.async_output;
    if (!target.empty()) target += "\n";
    target += text;
    if (g_rt.exec_depth == 0 && target.size() > max_async_output)
        target.erase(0, target.size() - max_async_output);
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

static void rt_run_task(lua_State* L, int id) {
    lua_task* t = rt_find_task(id);
    if (!t || t->dead) return;
    int ref = t->ref;
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    g_rt.deadline = rt_now() + callback_timeout_seconds;
    int err = lua_pcall(L, 0, 0, 0);
    g_rt.deadline = 0.0;
    bool failed = false;
    if (err) {
        const char* msg = lua_tostring(L, -1);
        rt_append_output(std::string("RuntimeError: ") + (msg ? msg : "unknown"));
        lua_pop(L, 1);
        failed = true;
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
    if (!L || g_rt.tasks.empty()) return;
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

static void rt_worker_loop() {
    BNM::AttachIl2Cpp();
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        if (g_rt.main_hook_active.load()) continue;
        std::unique_lock<std::recursive_mutex> lk(g_rt.mtx, std::try_to_lock);
        if (!lk.owns_lock()) continue;
        rt_pump_locked();
    }
}

static void rt_ensure_worker() {
    if (g_rt.worker_started) return;
    g_rt.worker_started = true;
    std::thread(rt_worker_loop).detach();
}

static void rt_pump_from_main() {
    std::unique_lock<std::recursive_mutex> lk(g_rt.mtx, std::try_to_lock);
    if (!lk.owns_lock()) return;
    double now = rt_now();
    if (now - g_rt.last_pump < 0.004) return;
    g_rt.last_pump = now;
    rt_pump_locked();
}

static void (*g_tick_original)(void*) = nullptr;

static void tick_trampoline(void* self) {
    if (g_tick_original) g_tick_original(self);
    rt_pump_from_main();
}

static int add_task(lua_State* L, double delay, double interval, bool repeating) {
    luaL_checktype(L, -1, LUA_TFUNCTION);
    lua_task t;
    t.id = g_rt.next_id++;
    t.ref = luaL_ref(L, LUA_REGISTRYINDEX);
    t.next_time = rt_now() + delay;
    t.interval = interval;
    t.repeating = repeating;
    t.dead = false;
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
