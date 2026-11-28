#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <sys/stat.h>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <chrono>
#include <string>
#include <vector>

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

extern "C" int bnm_audio_decode(const uint8_t* data, size_t size, float** out_pcm, long* out_frames, int* out_channels, int* out_rate, char* err, size_t err_size);
extern "C" void bnm_audio_free_pcm(void* ptr);

template <typename Fn>
static bool audio_run_main(Fn&& fn, std::string& error, int timeout_ms = 10000) {
    if (mt_on_main_thread() || !mt_pumping()) {
        try {
            fn();
            return true;
        } catch (const std::exception& e) {
            error = e.what();
            return false;
        } catch (...) {
            error = "native error";
            return false;
        }
    }
    struct audio_job {
        std::mutex m;
        std::condition_variable cv;
        bool done = false;
        std::string error;
    };
    auto job = std::make_shared<audio_job>();
    mt_post([&fn, job]() -> bool {
        std::string err;
        try {
            fn();
        } catch (const std::exception& e) {
            err = e.what();
        } catch (...) {
            err = "native error";
        }
        {
            std::lock_guard<std::mutex> lk(job->m);
            job->error = std::move(err);
            job->done = true;
        }
        job->cv.notify_all();
        return true;
    }, true);
    std::unique_lock<std::mutex> lk(job->m);
    if (!job->cv.wait_for(lk, std::chrono::milliseconds(timeout_ms), [&] { return job->done; })) {
        error = "timed out waiting for the main thread (is the game running?)";
        return false;
    }
    error = job->error;
    return error.empty();
}

static void* audio_call_raw(MethodInfo* mi, void* inst, void** args, char* err, size_t err_size) {
    if (!mi || !mi->methodPointer) {
        snprintf(err, err_size, "method not found");
        return nullptr;
    }
    Il2CppException* exc = nullptr;
    void* ret = BNM::Internal::il2cppMethods.il2cpp_runtime_invoke(mi, inst, args, &exc);
    if (exc) {
        format_exception(exc, err, err_size);
        return nullptr;
    }
    return ret;
}

static Il2CppObject* audio_create_clip(const char* name, long frames, int channels, int rate, char* err, size_t err_size) {
    Il2CppClass* clip_klass = find_class_cached("UnityEngine.AudioClip", "");
    if (!clip_klass) {
        snprintf(err, err_size, "UnityEngine.AudioClip not found in this build");
        return nullptr;
    }
    MethodInfo* mi = nullptr;
    for (auto* m : find_candidates(clip_klass, "Create", 5)) {
        if ((m->flags & 0x10) && m->methodPointer && type_full_name(bnm_method_parameter_type(m, 0)) == "System.String") {
            mi = m;
            break;
        }
    }
    if (!mi) {
        snprintf(err, err_size, "AudioClip.Create(string, int, int, int, bool) not found");
        return nullptr;
    }
    auto* str = CreateMonoString(name);
    int32_t f = (int32_t)frames;
    int32_t c = (int32_t)channels;
    int32_t r = (int32_t)rate;
    uint8_t stream[1] = {0};
    void* args[5] = {&str, &f, &c, &r, stream};
    auto* ret = (Il2CppObject*)audio_call_raw(mi, nullptr, args, err, err_size);
    return ret;
}

static bool audio_fill_clip(Il2CppObject* clip, const float* pcm, long count, long offset, char* err, size_t err_size) {
    auto& m = BNM::Internal::il2cppMethods;
    Il2CppClass* single = find_class_cached("System.Single", "");
    if (!single || !m.il2cpp_array_new) {
        snprintf(err, err_size, "cannot allocate a float array");
        return false;
    }
    auto* arr = m.il2cpp_array_new(single, (il2cpp_array_size_t)count);
    if (!arr) {
        snprintf(err, err_size, "float array allocation failed");
        return false;
    }
    memcpy((uint8_t*)arr + 0x20, pcm, (size_t)count * 4);
    const auto& candidates = find_candidates(clip->klass, "SetData", 2);
    MethodInfo* mi = nullptr;
    for (auto* cand : candidates) {
        if (!cand->methodPointer) continue;
        std::string param = type_full_name(bnm_method_parameter_type(cand, 0));
        if (param.find("Single[]") != std::string::npos) {
            mi = cand;
            break;
        }
    }
    if (!mi && !candidates.empty()) mi = candidates.front();
    if (!mi) {
        snprintf(err, err_size, "AudioClip.SetData not found");
        return false;
    }
    void* args[2] = {arr, &offset};
    void* ret = audio_call_raw(mi, clip, args, err, err_size);
    if (!ret) return false;
    bool ok = *(uint8_t*)((uint8_t*)ret + sizeof(Il2CppObject)) != 0;
    if (!ok) snprintf(err, err_size, "SetData returned false");
    return ok;
}

static bool audio_set_instance_value(Il2CppObject* target, const char* name, const void* value, size_t size, char* err, size_t err_size) {
    member_info mem = find_member(target->klass, name);
    if (mem.setter && mem.setter->methodPointer) {
        void* arg = size == sizeof(void*) ? *(void* const*)value : const_cast<void*>(value);
        void* args[1] = {arg};
        char inner[192];
        audio_call_raw(mem.setter, target, args, inner, sizeof(inner));
        if (inner[0]) {
            snprintf(err, err_size, "%s", inner);
            return false;
        }
        return true;
    }
    if (mem.field) {
        memcpy((uint8_t*)target + mem.field->offset, value, size);
        return true;
    }
    snprintf(err, err_size, "member '%s' not found", name);
    return false;
}

static Il2CppObject* audio_make_source(Il2CppObject* clip, float volume, bool loop, float spatial, Il2CppObject* parent_go, Il2CppObject** out_go, char* err, size_t err_size) {
    *out_go = nullptr;
    Il2CppClass* go_klass = find_class_cached("UnityEngine.GameObject", "");
    Il2CppClass* src_klass = find_class_cached("UnityEngine.AudioSource", "");
    if (!go_klass || !src_klass) {
        snprintf(err, err_size, "GameObject or AudioSource not found in this build");
        return nullptr;
    }
    auto& m = BNM::Internal::il2cppMethods;
    Il2CppObject* go = m.il2cpp_object_new(go_klass);
    if (!go) {
        snprintf(err, err_size, "GameObject allocation failed");
        return nullptr;
    }
    *out_go = go;
    {
        const auto& ctors = find_candidates(go_klass, ".ctor", 1);
        if (!ctors.empty() && ctors.front()->methodPointer) {
            auto* name = CreateMonoString("LuaBNM_Audio");
            void* args[1] = {(void*)name};
            audio_call_raw(ctors.front(), go, args, err, err_size);
            if (err[0]) return nullptr;
        } else {
            snprintf(err, err_size, "GameObject(string) constructor not found");
            return nullptr;
        }
    }
    Il2CppObject* src = nullptr;
    {
        auto mono_type = Class(src_klass).GetMonoType();
        if (!mono_type) {
            snprintf(err, err_size, "AudioSource MonoType unavailable");
            return nullptr;
        }
        MethodInfo* mi = nullptr;
        for (auto* cand : find_candidates(go_klass, "AddComponent", 1)) {
            if (cand->methodPointer && type_full_name(bnm_method_parameter_type(cand, 0)) == "System.Type") {
                mi = cand;
                break;
            }
        }
        if (!mi) {
            snprintf(err, err_size, "GameObject.AddComponent(System.Type) not found");
            return nullptr;
        }
        void* args[1] = {(void*)mono_type};
        src = (Il2CppObject*)audio_call_raw(mi, go, args, err, err_size);
        if (!src) return nullptr;
    }
    void* clip_value = (void*)clip;
    if (!audio_set_instance_value(src, "clip", &clip_value, sizeof(void*), err, err_size)) return nullptr;
    if (!audio_set_instance_value(src, "volume", &volume, 4, err, err_size)) return nullptr;
    uint8_t loop_value = loop ? 1 : 0;
    if (!audio_set_instance_value(src, "loop", &loop_value, 1, err, err_size)) return nullptr;
    if (!audio_set_instance_value(src, "spatialBlend", &spatial, 4, err, err_size)) return nullptr;
    if (parent_go && address_plausible((uintptr_t)parent_go)) {
        const auto& parent_transform_get = find_candidates(parent_go->klass, "get_transform", 0);
        if (!parent_transform_get.empty()) {
            void* parent_transform = audio_call_raw(parent_transform_get.front(), parent_go, nullptr, err, err_size);
            err[0] = '\0';
            if (parent_transform) {
                const auto& self_transform_get = find_candidates(go_klass, "get_transform", 0);
                if (!self_transform_get.empty()) {
                    void* self_transform = audio_call_raw(self_transform_get.front(), go, nullptr, err, err_size);
                    err[0] = '\0';
                    if (self_transform) {
                        const auto& set_parent = find_candidates(((Il2CppObject*)self_transform)->klass, "SetParent", 2);
                        if (!set_parent.empty() && set_parent.front()->methodPointer) {
                            uint8_t stay_world = 0;
                            void* args[2] = {parent_transform, &stay_world};
                            audio_call_raw(set_parent.front(), self_transform, args, err, err_size);
                            err[0] = '\0';
                        }
                    }
                }
            }
        }
    }
    const auto& play = find_candidates(src->klass, "Play", 0);
    if (!play.empty()) {
        audio_call_raw(play.front(), src, nullptr, err, err_size);
        err[0] = '\0';
    }
    return src;
}

static void audio_destroy_source(Il2CppObject* src, Il2CppObject* go, char* err, size_t err_size) {
    if (src) {
        const auto& stop = find_candidates(src->klass, "Stop", 0);
        if (!stop.empty()) {
            audio_call_raw(stop.front(), src, nullptr, err, err_size);
            err[0] = '\0';
        }
    }
    Il2CppClass* uo = find_class_cached("UnityEngine.Object", "");
    if (uo && go && address_plausible((uintptr_t)go)) {
        const auto& destroy = find_candidates(uo, "Destroy", 1);
        if (!destroy.empty()) {
            void* args[1] = {(void*)go};
            audio_call_raw(destroy.front(), nullptr, args, err, err_size);
            err[0] = '\0';
        }
    }
}

static bool audio_route_mic(Il2CppObject* clip, bool loop, bool restore, char* err, size_t err_size) {
    Il2CppClass* tagger = find_class_cached("GorillaTagger", "");
    if (!tagger) {
        snprintf(err, err_size, "GorillaTagger not found in this game");
        return false;
    }
    Il2CppObject* instance = nullptr;
    const auto& instance_get = find_candidates(tagger, "get_Instance", 0);
    if (!instance_get.empty()) {
        instance = (Il2CppObject*)audio_call_raw(instance_get.front(), nullptr, nullptr, err, err_size);
        err[0] = '\0';
    }
    if (!instance || !address_plausible((uintptr_t)instance)) {
        member_info inst_m = find_member(tagger, "Instance");
        if (inst_m.field) {
            auto& m = BNM::Internal::il2cppMethods;
            uint8_t slot[16];
            memset(slot, 0, sizeof(slot));
            if (m.il2cpp_runtime_class_init && inst_m.field->parent) m.il2cpp_runtime_class_init(inst_m.field->parent);
            m.il2cpp_field_static_get_value(inst_m.field, slot);
            memcpy(&instance, slot, sizeof(instance));
        }
    }
    if (!instance || !address_plausible((uintptr_t)instance)) {
        snprintf(err, err_size, "GorillaTagger.Instance is not available");
        return false;
    }
    Il2CppObject* recorder = nullptr;
    member_info rec_m = find_member(instance->klass, "myRecorder");
    if (rec_m.getter && rec_m.getter->methodPointer) {
        recorder = (Il2CppObject*)audio_call_raw(rec_m.getter, instance, nullptr, err, err_size);
        err[0] = '\0';
    } else if (rec_m.field) {
        memcpy(&recorder, (uint8_t*)instance + rec_m.field->offset, sizeof(recorder));
    }
    if (!recorder || !address_plausible((uintptr_t)recorder)) {
        snprintf(err, err_size, "GorillaTagger.myRecorder is not available");
        return false;
    }
    if (restore) {
        int32_t type_player = 0;
        void* null_clip = nullptr;
        uint8_t loop_off = 0;
        if (!audio_set_instance_value(recorder, "SourceType", &type_player, 4, err, err_size)) return false;
        if (!audio_set_instance_value(recorder, "AudioClip", &null_clip, sizeof(void*), err, err_size)) return false;
        if (!audio_set_instance_value(recorder, "LoopAudioClip", &loop_off, 1, err, err_size)) return false;
    } else {
        int32_t type_mic = 1;
        uint8_t loop_on = loop ? 1 : 0;
        if (!audio_set_instance_value(recorder, "SourceType", &type_mic, 4, err, err_size)) return false;
        if (!audio_set_instance_value(recorder, "AudioClip", &clip, sizeof(void*), err, err_size)) return false;
        if (!audio_set_instance_value(recorder, "LoopAudioClip", &loop_on, 1, err, err_size)) return false;
    }
    MethodInfo* restart = nullptr;
    for (auto* cand : find_candidates(recorder->klass, "RestartRecording", 1)) {
        if (cand->methodPointer) {
            restart = cand;
            break;
        }
    }
    if (!restart) {
        snprintf(err, err_size, "RestartRecording(bool) not found on the recorder");
        return false;
    }
    uint8_t flag = 1;
    void* args[1] = {&flag};
    audio_call_raw(restart, recorder, args, err, err_size);
    return err[0] == '\0';
}

static int l_audio_read_source(lua_State* L, std::string& bytes) {
    size_t size = 0;
    const char* data = luaL_checklstring(L, 1, &size);
    std::string clean = ws_sanitize_path(data, false);
    if (!clean.empty()) {
        std::string full = ws_workspace_dir() + "/" + clean;
        struct stat st;
        if (stat(full.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
            if (!ws_read_file(clean, bytes)) return luaL_error(L, "audio: failed to read workspace file '%s'", clean.c_str());
            return 0;
        }
    }
    if (size > 8 * 1024 * 1024) return luaL_error(L, "audio: inline data is limited to 8 MB, put the file in the workspace and pass its path");
    bytes.assign(data, size);
    return 0;
}

static int l_audio_push_error(lua_State* L, const char* err) {
    lua_pushnil(L);
    lua_pushstring(L, err);
    return 2;
}

static int l_audio_decode(lua_State* L) {
    std::string bytes;
    if (l_audio_read_source(L, bytes)) return 2;
    float* pcm = nullptr;
    long frames = 0;
    int channels = 0;
    int rate = 0;
    char err[256] = {0};
    if (!bnm_audio_decode((const uint8_t*)bytes.data(), bytes.size(), &pcm, &frames, &channels, &rate, err, sizeof(err)))
        return l_audio_push_error(L, err);
    lua_newtable(L);
    lua_pushlstring(L, (const char*)pcm, (size_t)frames * channels * 4);
    lua_setfield(L, -2, "data");
    lua_pushnumber(L, rate);
    lua_setfield(L, -2, "rate");
    lua_pushnumber(L, channels);
    lua_setfield(L, -2, "channels");
    lua_pushnumber(L, frames);
    lua_setfield(L, -2, "frames");
    bnm_audio_free_pcm(pcm);
    return 1;
}

static int l_audio_load(lua_State* L) {
    std::string bytes;
    if (l_audio_read_source(L, bytes)) return 2;
    std::string name = lua_isstring(L, 2) ? lua_tostring(L, 2) : "lua_clip";
    float* pcm = nullptr;
    long frames = 0;
    int channels = 0;
    int rate = 0;
    char err[256] = {0};
    if (!bnm_audio_decode((const uint8_t*)bytes.data(), bytes.size(), &pcm, &frames, &channels, &rate, err, sizeof(err)))
        return l_audio_push_error(L, err);
    Il2CppObject* clip = nullptr;
    std::string job_error;
    audio_run_main([&]() {
        clip = audio_create_clip(name.c_str(), frames, channels, rate, err, sizeof(err));
        if (clip) audio_fill_clip(clip, pcm, frames * channels, 0, err, sizeof(err));
    }, job_error);
    bnm_audio_free_pcm(pcm);
    if (!job_error.empty()) return l_audio_push_error(L, job_error.c_str());
    if (!clip) return l_audio_push_error(L, err);
    push_object(L, clip);
    return 1;
}

static int l_audio_play(lua_State* L) {
    bool is_ref = false;
    uintptr_t clip_addr = ref_address_at(L, 1, is_ref);
    if (!clip_addr || !address_plausible(clip_addr)) return l_audio_push_error(L, "audio clip reference expected");
    float volume = 1.0f;
    float spatial = 0.0f;
    bool loop = false;
    uintptr_t parent = 0;
    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "volume");
        if (lua_isnumber(L, -1)) volume = (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "loop");
        loop = lua_toboolean(L, -1) != 0;
        lua_pop(L, 1);
        lua_getfield(L, 2, "spatial_blend");
        if (!lua_isnumber(L, -1)) {
            lua_pop(L, 1);
            lua_getfield(L, 2, "spatialBlend");
        }
        if (lua_isnumber(L, -1)) spatial = (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "parent");
        if (!lua_isnil(L, -1) && !lua_isnone(L, -1)) {
            bool r = false;
            parent = ref_address_at(L, -1, r);
        }
        lua_pop(L, 1);
    }
    char err[256] = {0};
    Il2CppObject* src = nullptr;
    Il2CppObject* go = nullptr;
    std::string job_error;
    audio_run_main([&]() {
        src = audio_make_source((Il2CppObject*)clip_addr, volume, loop, spatial, (Il2CppObject*)parent, &go, err, sizeof(err));
    }, job_error);
    if (!job_error.empty()) return l_audio_push_error(L, job_error.c_str());
    if (!src) return l_audio_push_error(L, err[0] ? err : "failed to create the audio source");
    lua_newtable(L);
    push_object(L, (Il2CppObject*)clip_addr);
    lua_setfield(L, -2, "clip");
    push_object(L, src);
    lua_setfield(L, -2, "src");
    push_object(L, go);
    lua_setfield(L, -2, "go");
    return 1;
}

static int l_audio_stop(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    bool is_ref = false;
    uintptr_t src_addr = 0;
    uintptr_t go_addr = 0;
    lua_getfield(L, 1, "src");
    if (!lua_isnil(L, -1)) src_addr = ref_address_at(L, -1, is_ref);
    lua_pop(L, 1);
    lua_getfield(L, 1, "go");
    if (!lua_isnil(L, -1)) go_addr = ref_address_at(L, -1, is_ref);
    lua_pop(L, 1);
    if (!src_addr && !go_addr) return l_audio_push_error(L, "handle has no src or go fields");
    char err[256] = {0};
    std::string job_error;
    audio_run_main([&]() {
        audio_destroy_source((Il2CppObject*)src_addr, (Il2CppObject*)go_addr, err, sizeof(err));
    }, job_error);
    if (!job_error.empty()) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, job_error.c_str());
        return 2;
    }
    lua_pushboolean(L, 1);
    return 1;
}

static int l_audio_route_mic(lua_State* L) {
    bool is_ref = false;
    uintptr_t clip_addr = ref_address_at(L, 1, is_ref);
    if (!clip_addr || !address_plausible(clip_addr)) return l_audio_push_error(L, "audio clip reference expected");
    bool loop = lua_toboolean(L, 2) != 0;
    char err[256] = {0};
    std::string job_error;
    bool ok = false;
    audio_run_main([&]() {
        ok = audio_route_mic((Il2CppObject*)clip_addr, loop, false, err, sizeof(err));
    }, job_error);
    if (!job_error.empty()) return l_audio_push_error(L, job_error.c_str());
    if (!ok) return l_audio_push_error(L, err[0] ? err : "routing failed");
    lua_pushboolean(L, 1);
    return 1;
}

static int l_audio_restore_mic(lua_State* L) {
    char err[256] = {0};
    std::string job_error;
    bool ok = false;
    audio_run_main([&]() {
        ok = audio_route_mic(nullptr, false, true, err, sizeof(err));
    }, job_error);
    if (!job_error.empty()) return l_audio_push_error(L, job_error.c_str());
    if (!ok) return l_audio_push_error(L, err[0] ? err : "restore failed");
    lua_pushboolean(L, 1);
    return 1;
}

static int l_audio_formats(lua_State* L) {
    lua_newtable(L);
    lua_pushstring(L, "wav");
    lua_rawseti(L, -2, 1);
    lua_pushstring(L, "ogg");
    lua_rawseti(L, -2, 2);
    lua_pushstring(L, "mp3");
    lua_rawseti(L, -2, 3);
    return 1;
}

static void register_lua_audio(lua_State* L) {
    lua_newtable(L);
    lua_pushcfunction(L, l_audio_decode);
    lua_setfield(L, -2, "decode");
    lua_pushcfunction(L, l_audio_load);
    lua_setfield(L, -2, "load");
    lua_pushcfunction(L, l_audio_play);
    lua_setfield(L, -2, "play");
    lua_pushcfunction(L, l_audio_stop);
    lua_setfield(L, -2, "stop");
    lua_pushcfunction(L, l_audio_route_mic);
    lua_setfield(L, -2, "route_to_mic");
    lua_pushcfunction(L, l_audio_restore_mic);
    lua_setfield(L, -2, "restore_mic");
    lua_pushcfunction(L, l_audio_formats);
    lua_setfield(L, -2, "formats");
    lua_setglobal(L, "audio");
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
