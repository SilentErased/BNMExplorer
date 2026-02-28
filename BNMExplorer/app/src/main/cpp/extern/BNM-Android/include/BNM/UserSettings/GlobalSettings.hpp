#if __cplusplus < 202002L
static_assert(false, "ByNameModding requires C++20 and above!");
#endif

#pragma once

//#define UNITY_VER 56 // 5.6.4f1
//#define UNITY_VER 171 // 2017.1.x
//#define UNITY_VER 172 // 2017.2.x - 2017.4.x
//#define UNITY_VER 181 // 2018.1.x
//#define UNITY_VER 182 // 2018.2.x
//#define UNITY_VER 183 // 2018.3.x - 2018.4.x
//#define UNITY_VER 191 // 2019.1.x - 2019.2.x
//#define UNITY_VER 193 // 2019.3.x
//#define UNITY_VER 194 // 2019.4.x
//#define UNITY_VER 201 // 2020.1.x
//#define UNITY_VER 202 // 2020.2.x - 2020.3.19
//#define UNITY_VER 203 // 2020.3.20 - 2020.3.xx
//#define UNITY_VER 211 // 2021.1.x (Need set UNITY_PATCH_VER to 24 if x (2021.1.x) >= 24)
//#define UNITY_VER 212 // 2021.2.x
#define UNITY_VER 213 // 2021.3.x
//#define UNITY_VER 221 // 2022.1.x
//#define UNITY_VER 222 // 2022.2.x - 2022.3.x
//#define UNITY_VER 231 // 2023.1.x
//#define UNITY_VER 232 // 2023.2.x+
//#define UNITY_VER 6000 // 6000.0.x

#define UNITY_PATCH_VER 32 // For special cases

// #define BNM_DEPRECATED
// #define BNM_ALLOW_MULTI_THREADING_SYNC
// #define BNM_DOTNET35
//#define BNM_CLASSES_MANAGEMENT
//#define BNM_COROUTINE
// #define BNM_AUTO_HOOK_DISABLE_VIRTUAL_HOOK
// #define BNM_OLD_GOOD_DAYS
#define BNM_USE_IL2CPP_ALLOCATOR

#ifndef NDEBUG
#define BNM_ALLOW_STR_METHODS
#define BNM_ALLOW_SAFE_IS_ALLOCATED
#define BNM_ALLOW_SELF_CHECKS
#define BNM_CHECK_INSTANCE_TYPE
#define BNM_DEBUG
#define BNM_INFO
#define BNM_ERROR
#define BNM_WARNING
#endif

#define BNM_OBFUSCATE(str) str
#define BNM_OBFUSCATE_TMP(str) str

#include <dobby.h>

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) {
    if ((void *) ptr != nullptr) DobbyHook((void *)ptr, (dobby_dummy_func_t) newMethod, (dobby_dummy_func_t *) &oldBytes);
    return (void *) ptr;
}

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &&oldBytes) {
    if ((void *) ptr != nullptr) DobbyHook((void *)ptr, (dobby_dummy_func_t) newMethod, (dobby_dummy_func_t *) &oldBytes);
    return (void *) ptr;
}

template<typename PTR_T>
inline void Unhook(PTR_T ptr) {
    if ((void *) ptr != nullptr) DobbyDestroy((void *)ptr);
}

#include <dlfcn.h>

#define BNM_dlopen dlopen
#define BNM_dlsym dlsym
#define BNM_dlclose dlclose
#define BNM_dladdr dladdr

#include <cstdlib>

#define BNM_malloc malloc
#define BNM_free free

#include <android/log.h>

#define BNM_TAG "ByNameModding"

#ifdef BNM_ALLOW_SELF_CHECKS
#define BNM_CHECK_SELF(returnValue) if (!SelfCheck()) return returnValue
#else
#define BNM_CHECK_SELF(returnValue) ((void)0)
#endif

#ifdef BNM_INFO
#define BNM_LOG_INFO(...) ((void)__android_log_print(4, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_INFO(...) ((void)0)
#endif

#ifdef BNM_DEBUG
#define BNM_LOG_DEBUG(...) ((void)__android_log_print(3, BNM_TAG, __VA_ARGS__))
#define BNM_LOG_DEBUG_IF(condition, ...) if (condition) ((void)__android_log_print(3, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_DEBUG(...) ((void)0)
#define BNM_LOG_DEBUG_IF(...) ((void)0)
#endif

#ifdef BNM_ERROR
#define BNM_LOG_ERR(...) ((void)__android_log_print(6, BNM_TAG, __VA_ARGS__))
#define BNM_LOG_ERR_IF(condition, ...) if (condition) ((void)__android_log_print(6, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_ERR(...) ((void)0)
#define BNM_LOG_ERR_IF(condition, ...) ((void)0)
#endif

#ifdef BNM_WARNING
#define BNM_LOG_WARN(...) ((void)__android_log_print(5, BNM_TAG, __VA_ARGS__))
#define BNM_LOG_WARN_IF(condition, ...) if (condition) ((void)__android_log_print(5, BNM_TAG, __VA_ARGS__))
#else
#define BNM_LOG_WARN(...) ((void)0)
#define BNM_LOG_WARN_IF(condition, ...) ((void)0)
#endif

namespace BNM {
#if defined(__LP64__)
    typedef long BNM_INT_PTR;
    typedef unsigned long BNM_PTR;
#else
    typedef int BNM_INT_PTR;
    typedef unsigned int BNM_PTR;
#endif
}

#define BNM_VER "2.5.2"