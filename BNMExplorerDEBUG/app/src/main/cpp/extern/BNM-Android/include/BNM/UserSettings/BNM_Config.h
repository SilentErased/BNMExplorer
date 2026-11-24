#pragma once


/**
 * =========================================================================================
 * BNM_Config.example.h - External Configuration Template for ByNameModding (BNM)
 * =========================================================================================
 *
 * USAGE GUIDE:
 * 1. Copy this file to your mod project folder and rename it to "BNM_Config.h".
 *
 * 2. In your mod project's CMakeLists.txt, set `BNM_CONFIG_INCLUDE_DIRS` to the folder
 *    containing your `BNM_Config.h`, for example:
 *
 *    set(BNM_CONFIG_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Core/Config)
 *    add_subdirectory(external/BNM-Android EXCLUDE_FROM_ALL)
 *    get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)
 *    target_include_directories(Core PUBLIC ${BNM_INCLUDE_DIRECTORIES})
 *    target_link_libraries(Core PUBLIC BNM::BNM)
 *
 * 3. You do NOT need to modify BNM source files directly! All configurations defined in
 *    `BNM_Config.h` will automatically override BNM's internal defaults.
 * =========================================================================================
 */

//! =========================================================================================
//! 1. TARGET UNITY VERSION CONFIGURATION
//! =========================================================================================
//! Select the target Unity version for your game (uncomment one):
//#define UNITY_VER 56  // Unity 5.6.4f1
//#define UNITY_VER 171 // Unity 2017.1.x
//#define UNITY_VER 172 // Unity 2017.2.x - 2017.4.x
//#define UNITY_VER 181 // Unity 2018.1.x
//#define UNITY_VER 182 // Unity 2018.2.x
//#define UNITY_VER 183 // Unity 2018.3.x - 2018.4.x
//#define UNITY_VER 191 // Unity 2019.1.x - 2019.2.x
//#define UNITY_VER 193 // Unity 2019.3.x
//#define UNITY_VER 194 // Unity 2019.4.x
//#define UNITY_VER 201 // Unity 2020.1.x
//#define UNITY_VER 202 // Unity 2020.2.x - 2020.3.19
//#define UNITY_VER 203 // Unity 2020.3.20 - 2020.3.xx
//#define UNITY_VER 211 // Unity 2021.1.x (Set UNITY_PATCH_VER = 24 if patch >= 24)
//#define UNITY_VER 212 // Unity 2021.2.x
#ifndef UNITY_VER
#define UNITY_VER 213
#endif

#ifndef UNITY_PATCH_VER
#define UNITY_PATCH_VER 0
#endif
//#define UNITY_VER 221 // Unity 2022.1.x
//#define UNITY_VER 222 // Unity 2022.2.x - 2022.3.x (LTS)
//#define UNITY_VER 231 // Unity 2023.1.x
//#define UNITY_VER 232 // Unity 2023.2.x - 2023.3.x
//#define UNITY_VER 600 // Unity 6000.0.x - 6000.4.x
//#define UNITY_VER 605 // Unity 6000.5.x
//#define UNITY_VER 606 // Unity 6000.6.x+

//#define UNITY_PATCH_VER 7 // Specific patch version (if required)

//! =========================================================================================
//! 2. HOOKING FRAMEWORK INTEGRATION (MACRO BINDINGS)
//! =========================================================================================
//! Bind any inline hooking library to BNM using `BNM_HOOK` and `BNM_UNHOOK` macros:

//! --- Example A: Dobby Hook ---
#include <dobby.h>
#define BNM_HOOK(target, replace, orig) (DobbyHook((void *)(target), (dobby_dummy_func_t)(replace), (dobby_dummy_func_t *)(orig)) == RT_SUCCESS ? (void *)(target) : nullptr)
#define BNM_UNHOOK(ptr)                 DobbyDestroy((void *)(ptr))

//! --- Example B: ByteDance ShadowHook (Recommended for Android 7 - 14+) ---
/*
#include <shadowhook.h>
#define BNM_HOOK(target, replace, orig) shadowhook_hook_func_addr((void *)(target), (void *)(replace), (void **)(orig))
#define BNM_UNHOOK(ptr)                 shadowhook_unhook((void *)(ptr))
*/

//! --- Example C: Cydia Substrate / MSHookFunction ---
/*
#include <substrate.h>
#define BNM_HOOK(target, replace, orig) (MSHookFunction((void *)(target), (void *)(replace), (void **)(orig)), (void *)(target))
#define BNM_UNHOOK(ptr)                 ((void)0)
*/

//! --- Example D: Custom Hook Wrapper ---
/*
#include "MyHookWrapper.hpp"
#define BNM_HOOK(target, replace, orig) MyHook::Install((void *)(target), (void *)(replace), (void **)(orig))
#define BNM_UNHOOK(ptr)                 MyHook::Remove((void *)(ptr))
*/

//! --- Example E: Hookless Mode ---
//! Enable if you only use BNM for reflection, field access, method invocation, and ESP/Aimbot,
//! without runtime class creation / auto-hooking.
// #define BNM_DISABLE_HOOKING

//! =========================================================================================
//! 3. DYNAMIC LOADER INTEGRATION (CUSTOM DLOPEN / DLSYM / XDL)
//! =========================================================================================
//! By default, BNM uses standard <dlfcn.h> (dlopen, dlsym, dlclose, dladdr, Dl_info).
//! You can bind any custom dynamic loader (e.g. xDL to bypass Android 7.0+ linker restrictions):

//! --- Example A: xDL (hexhacking/xDL) ---
/*
#include <xdl.h>
#define BNM_dlopen(name, flags)    xdl_open(name, flags)
#define BNM_dlsym(handle, symbol)  xdl_sym(handle, symbol, nullptr)
#define BNM_dlclose(handle)        xdl_close(handle)
#define BNM_dladdr(addr, info)     xdl_addr(addr, info, nullptr)
#define BNM_Dl_info                xdl_info_t
*/

//! --- Example B: Custom Loader Wrapper ---
/*
#include "CustomLoader.hpp"
#define BNM_dlopen(name, flags)    CustomLoader::Open(name, flags)
#define BNM_dlsym(handle, symbol)  CustomLoader::Sym(handle, symbol)
#define BNM_dlclose(handle)        CustomLoader::Close(handle)
#define BNM_dladdr(addr, info)     CustomLoader::Addr(addr, info)
#define BNM_Dl_info                CustomDlInfo
*/

//! =========================================================================================
//! 4. ZERO-COST MODULAR ENGINE BINDINGS
//! =========================================================================================
//! By default, all modular engine bindings are enabled. You can disable unused modules
//! to minimize binary size:

// #define BNM_DISABLE_UNITY_PHYSICS      // Disable UnityEngine.Physics (3D) bindings
// #define BNM_DISABLE_UNITY_PHYSICS2D    // Disable UnityEngine.Physics2D (2D) bindings
// #define BNM_DISABLE_UNITY_UI           // Disable UnityEngine.UI (uGUI) bindings
// #define BNM_DISABLE_UNITY_TEXTMESHPRO  // Disable TextMeshPro (TMP) bindings
// #define BNM_DISABLE_UNITY_RENDERERS    // Disable Renderers & Mesh bindings
// #define BNM_DISABLE_UNITY_AUDIO        // Disable UnityEngine.Audio bindings
// #define BNM_DISABLE_UNITY_ANIMATION    // Disable UnityEngine.Animation / Animator bindings
// #define BNM_DISABLE_CLASSES_MANAGEMENT // Disable runtime class creation & management
// #define BNM_DISABLE_COROUTINE          // Disable Coroutine helper features

//! Pure Whitelist Mode: When defined, all modules are disabled by default except those
//! explicitly enabled with `#define BNM_UNITY_<MODULE_NAME>`.
// #define BNM_MANUAL_MODULES

//! =========================================================================================
//! 5. STRING ENCRYPTION / OBFUSCATOR INTEGRATION
//! =========================================================================================
//! Integrate your project's compile-time string obfuscator (e.g. OBFUSCATE, AY_OBFUSCATE):
// #include "obfuscate.h"
// #define BNM_OBFUSCATE(str) AY_OBFUSCATE(str)
// #define BNM_OBFUSCATE_TMP(str) AY_OBFUSCATE(str)

//! =========================================================================================
//! 6. LOGGING, MEMORY ALLOCATOR & RUNTIME SETTINGS
//! =========================================================================================
//! Custom Logcat Tag:
// #define BNM_TAG "MyGameMod"

//! Disable all BNM logging in release builds:
// #define BNM_DISABLE_ALL_LOGS

//! Custom memory allocator (if using a custom memory pool):
// #define BNM_malloc(size) my_custom_malloc(size)
// #define BNM_free(ptr)    my_custom_free(ptr)

//! Enable internal multi-threading synchronization if calling BNM from multiple background threads:
// #define BNM_ALLOW_MULTI_THREADING_SYNC

//! Enable if the target game uses legacy .NET 3.5 runtime:
// #define BNM_DOTNET35