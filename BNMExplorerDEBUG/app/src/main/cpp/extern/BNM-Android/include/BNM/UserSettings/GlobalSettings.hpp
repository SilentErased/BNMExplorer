#if __cplusplus < 202002L
static_assert(false, "ByNameModding requires C++20 and above!");
#endif

#pragma once

//! =========================================================================================
//! External Configuration File Detection
//! =========================================================================================
#if defined(BNM_USER_CONFIG)
#  include BNM_USER_CONFIG
#elif defined(__has_include)
#  if __has_include("BNM_Config.h")
#    include "BNM_Config.h"
#  elif __has_include("BNM_UserConfig.h")
#    include "BNM_UserConfig.h"
#  elif __has_include(<BNM_Config.h>)
#    include <BNM_Config.h>
#  elif __has_include(<BNM_UserConfig.h>)
#    include <BNM_UserConfig.h>
#  endif
#endif

//! =========================================================================================
//! Unity Version Configuration
//! =========================================================================================


//! Allow to use deprecated methods (if any)
// #define BNM_DEPRECATED

//! Allow thread synchronization when accessing BNM from multiple threads
// #define BNM_ALLOW_MULTI_THREADING_SYNC

//! For legacy .NET 3.5 Dictionary compatibility
// #define BNM_DOTNET35

//! =========================================================================================
//! Modular Feature Bindings
//! =========================================================================================
#ifndef BNM_MANUAL_MODULES

//! Code for creating new classes and modifying existing ones
#if !defined(BNM_CLASSES_MANAGEMENT) && !defined(BNM_DISABLE_CLASSES_MANAGEMENT) && !defined(BNM_NO_CLASSES_MANAGEMENT)
#define BNM_CLASSES_MANAGEMENT
#endif

//! Coroutine creation code (Requires ClassesManagement!)
#if defined(BNM_CLASSES_MANAGEMENT) && !defined(BNM_COROUTINE) && !defined(BNM_DISABLE_COROUTINE) && !defined(BNM_NO_COROUTINE)
#define BNM_COROUTINE
#endif

//! Structures and methods for UnityEngine.Physics (3D Physics)
#if !defined(BNM_UNITY_PHYSICS) && !defined(BNM_DISABLE_UNITY_PHYSICS) && !defined(BNM_NO_UNITY_PHYSICS)
#define BNM_UNITY_PHYSICS
#endif

//! Structures and methods for UnityEngine.Physics2D (2D Physics)
#if !defined(BNM_UNITY_PHYSICS2D) && !defined(BNM_DISABLE_UNITY_PHYSICS2D) && !defined(BNM_NO_UNITY_PHYSICS2D)
#define BNM_UNITY_PHYSICS2D
#endif

//! Structures and methods for UnityEngine.UI (uGUI)
#if !defined(BNM_UNITY_UI) && !defined(BNM_DISABLE_UNITY_UI) && !defined(BNM_NO_UNITY_UI)
#define BNM_UNITY_UI
#endif

//! Structures and methods for TextMeshPro
#if !defined(BNM_UNITY_TEXTMESHPRO) && !defined(BNM_DISABLE_UNITY_TEXTMESHPRO) && !defined(BNM_NO_UNITY_TEXTMESHPRO)
#define BNM_UNITY_TEXTMESHPRO
#endif

//! Structures and methods for Renderers (MeshRenderer, SkinnedMeshRenderer, SpriteRenderer, Mesh)
#if !defined(BNM_UNITY_RENDERERS) && !defined(BNM_DISABLE_UNITY_RENDERERS) && !defined(BNM_NO_UNITY_RENDERERS)
#define BNM_UNITY_RENDERERS
#endif

//! Structures and methods for UnityEngine.Audio (AudioSource, AudioClip, AudioListener)
#if !defined(BNM_UNITY_AUDIO) && !defined(BNM_DISABLE_UNITY_AUDIO) && !defined(BNM_NO_UNITY_AUDIO)
#define BNM_UNITY_AUDIO
#endif

//! Structures and methods for UnityEngine.Animation (Animator, Animation)
#if !defined(BNM_UNITY_ANIMATION) && !defined(BNM_DISABLE_UNITY_ANIMATION) && !defined(BNM_NO_UNITY_ANIMATION)
#define BNM_UNITY_ANIMATION
#endif

#endif // BNM_MANUAL_MODULES

//! Disable auto hook via virtual method table in ClassesManagement
// #define BNM_AUTO_HOOK_DISABLE_VIRTUAL_HOOK

//! The good old days...
// #define BNM_OLD_GOOD_DAYS

//! Use il2cpp allocator for Mono arrays instead of standard allocator
#if !defined(BNM_USE_IL2CPP_ALLOCATOR) && !defined(BNM_DISABLE_IL2CPP_ALLOCATOR) && !defined(BNM_NO_IL2CPP_ALLOCATOR)
#define BNM_USE_IL2CPP_ALLOCATOR
#endif

#ifndef NDEBUG

//! str() methods in structures
#ifndef BNM_ALLOW_STR_METHODS
#define BNM_ALLOW_STR_METHODS
#endif

//! Use signal handling in IsAllocated
#ifndef BNM_ALLOW_SAFE_IS_ALLOCATED
#define BNM_ALLOW_SAFE_IS_ALLOCATED
#endif

//! Check mono objects in methods
#ifndef BNM_ALLOW_SELF_CHECKS
#define BNM_ALLOW_SELF_CHECKS
#endif

//! Check classes when setting an instance to fields and methods
#ifndef BNM_CHECK_INSTANCE_TYPE
#define BNM_CHECK_INSTANCE_TYPE
#endif

#ifndef BNM_DEBUG
#define BNM_DEBUG
#endif

#ifndef BNM_INFO
#define BNM_INFO
#endif

#ifndef BNM_ERROR
#define BNM_ERROR
#endif

#ifndef BNM_WARNING
#define BNM_WARNING
#endif

#endif

//! Custom string encryptor
#ifndef BNM_OBFUSCATE
#define BNM_OBFUSCATE(str) str // const char *
#endif

//! Temporary data obfuscation macro (can be freed after BNM initialization)
#ifndef BNM_OBFUSCATE_TMP
#define BNM_OBFUSCATE_TMP(str) str // const char *
#endif

//! =========================================================================================
//! Hooking Framework Integration (Macro Function Binding)
//! =========================================================================================
#if defined(BNM_HOOK)
template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) {
    if ((void *) ptr != nullptr) return (void *) BNM_HOOK((void *) ptr, (void *) newMethod, (void **) &oldBytes);
    return nullptr;
}

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &&oldBytes) {
    if ((void *) ptr != nullptr) return (void *) BNM_HOOK((void *) ptr, (void *) newMethod, (void **) &oldBytes);
    return nullptr;
}

template<typename PTR_T>
inline void Unhook(PTR_T ptr) {
#if defined(BNM_UNHOOK)
    if ((void *) ptr != nullptr) BNM_UNHOOK((void *) ptr);
#else
    ((void) 0);
#endif
}
#elif defined(BNM_TEST_HOOKING)
template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) { return nullptr; }
template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &&oldBytes) { return nullptr; }
template<typename PTR_T>
inline void Unhook(PTR_T ptr) {}
#elif defined(BNM_DISABLE_HOOKING) || defined(BNM_NO_HOOKING)
template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) { return nullptr; }
template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &&oldBytes) { return nullptr; }
template<typename PTR_T>
inline void Unhook(PTR_T ptr) {}
#else
#include <cassert>
static_assert(false, "No hooking software configured! Define BNM_HOOK(target, replace, orig) and optionally BNM_UNHOOK(ptr) in BNM_Config.h, or define BNM_DISABLE_HOOKING.");
#endif

//! =========================================================================================
//! Dynamic Loader Integration (dlopen, dlsym, dlclose, dladdr, Dl_info)
//! =========================================================================================
#if defined(BNM_TEST_HOOKING)
struct Dl_info { const char *dli_fname{}; void *dli_fbase{}; const char *dli_sname{}; void *dli_saddr{}; };
#define BNM_dlopen(x, y) nullptr
#define BNM_dlsym(x, y) nullptr
#define BNM_dlclose(x) 0
#define BNM_dladdr(x, y) 0
#define BNM_Dl_info Dl_info
#else

#ifndef BNM_dlopen
#  include <dlfcn.h>
#  define BNM_dlopen(name, flags)    dlopen(name, flags)
#endif

#ifndef BNM_dlsym
#  include <dlfcn.h>
#  define BNM_dlsym(handle, symbol)  dlsym(handle, symbol)
#endif

#ifndef BNM_dlclose
#  include <dlfcn.h>
#  define BNM_dlclose(handle)        dlclose(handle)
#endif

#ifndef BNM_dladdr
#  include <dlfcn.h>
#  define BNM_dladdr(addr, info)     dladdr(addr, info)
#endif

#ifndef BNM_Dl_info
#  include <dlfcn.h>
#  define BNM_Dl_info                Dl_info
#endif

#endif

#include <cstdlib>

// Custom memory management
#ifndef BNM_malloc
#define BNM_malloc malloc
#endif

#ifndef BNM_free
#define BNM_free free
#endif

#ifndef BNM_TAG
#define BNM_TAG "ByNameModding"
#endif

#ifndef BNM_PRINT_LOG
#  ifdef __ANDROID__
#    include <android/log.h>
#    define BNM_PRINT_LOG(prio, ...) ((void)__android_log_print(prio, BNM_TAG, __VA_ARGS__))
#  else
#    include <cstdio>
#    define BNM_PRINT_LOG(prio, ...) ((void)printf(__VA_ARGS__))
#  endif
#endif

#ifdef BNM_ALLOW_SELF_CHECKS
#define BNM_CHECK_SELF(returnValue) if (!SelfCheck()) return returnValue
#else
#define BNM_CHECK_SELF(returnValue) ((void)0)
#endif

#if !defined(BNM_DISABLE_ALL_LOGS) && !defined(BNM_NO_LOG)

#ifdef BNM_INFO
#define BNM_LOG_INFO(...) BNM_PRINT_LOG(4, __VA_ARGS__)
#else
#define BNM_LOG_INFO(...) ((void)0)
#endif

#ifdef BNM_DEBUG
#define BNM_LOG_DEBUG(...) BNM_PRINT_LOG(3, __VA_ARGS__)
#define BNM_LOG_DEBUG_IF(condition, ...) if (condition) BNM_PRINT_LOG(3, __VA_ARGS__)
#else
#define BNM_LOG_DEBUG(...) ((void)0)
#define BNM_LOG_DEBUG_IF(...) ((void)0)
#endif

#ifdef BNM_ERROR
#define BNM_LOG_ERR(...) BNM_PRINT_LOG(6, __VA_ARGS__)
#define BNM_LOG_ERR_IF(condition, ...) if (condition) BNM_PRINT_LOG(6, __VA_ARGS__)
#else
#define BNM_LOG_ERR(...) ((void)0)
#define BNM_LOG_ERR_IF(...) ((void)0)
#endif

#ifdef BNM_WARNING
#define BNM_LOG_WARN(...) BNM_PRINT_LOG(5, __VA_ARGS__)
#define BNM_LOG_WARN_IF(condition, ...) if (condition) BNM_PRINT_LOG(5, __VA_ARGS__)
#else
#define BNM_LOG_WARN(...) ((void)0)
#define BNM_LOG_WARN_IF(...) ((void)0)
#endif

#else // BNM_DISABLE_ALL_LOGS

#define BNM_LOG_INFO(...) ((void)0)
#define BNM_LOG_DEBUG(...) ((void)0)
#define BNM_LOG_DEBUG_IF(...) ((void)0)
#define BNM_LOG_ERR(...) ((void)0)
#define BNM_LOG_ERR_IF(...) ((void)0)
#define BNM_LOG_WARN(...) ((void)0)
#define BNM_LOG_WARN_IF(...) ((void)0)

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