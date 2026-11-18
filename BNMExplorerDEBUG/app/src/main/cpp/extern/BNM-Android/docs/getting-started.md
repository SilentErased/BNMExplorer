# Getting Started with BNM

**ByNameModding (BNM)** is a high-performance C++20 static library designed to interact with Unity IL2CPP runtimes on Android. It allows native C++ developers to inspect metadata, invoke methods, access fields, hook functions, spawn game objects, build runtime UIs, and bind C++20 coroutines directly without hardcoded offsets.

---

## 🛠️ System Requirements & Toolchain

- **C++ Standard**: C++20 or later (uses concepts, coroutines, `std::string_view`, and `std::span`).
- **Android NDK**: NDK r23+ (NDK r25+ / r27+ / r29+ recommended).
- **Target Architectures**: `arm64-v8a`, `armeabi-v7a`, `x86_64`, `x86`.
- **Target Unity Versions**: Unity 5.6 up to Unity 2023.2+ and Unity 6000.x (Unity 6).

---

## 📦 Project Integration

### 1. Integrating via `Android.mk` (ndk-build)

Include BNM in your Android NDK build tree:

```makefile
# In your jni/Android.mk
LOCAL_PATH := $(call my-dir)

# 1. Include BNM static module
include $(CLEAR_VARS)
include $(LOCAL_PATH)/BNM-Android/Android.mk

# 2. Your Native Mod Library
include $(CLEAR_VARS)
LOCAL_MODULE := MyNativeMod
LOCAL_CPPFLAGS += -std=c++20
LOCAL_SRC_FILES := Main.cpp

# Link against BNM
LOCAL_STATIC_LIBRARIES := BNM
# Link Android log & system libraries
LOCAL_LDLIBS := -llog -landroid

include $(BUILD_SHARED_LIBRARY)
```

> **Note**: `Android.mk` in BNM exports `LOCAL_EXPORT_C_INCLUDES`, so you don't need to manually configure include paths for `include/` and `external/include/`.

---

### 2. Integrating via `CMakeLists.txt` (CMake)

```cmake
cmake_minimum_required(VERSION 3.22.1)
project(MyNativeMod CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add BNM subdirectory
add_subdirectory(BNM-Android)

add_library(MyNativeMod SHARED
    src/Main.cpp
)

target_link_libraries(MyNativeMod PRIVATE
    BNM
    log
    android
)
```

---

## ⚙️ Hooking Software Configuration (`BNM_Config.h`)

BNM requires a hook backend macro definition (such as Dobby, ShadowHook, Substrate, or KittyMemory) inside `include/BNM/UserSettings/BNM_Config.h` or via compiler definitions:

### A. Configuring Dobby Hook
```cpp
#include <dobby.h>
#define BNM_HOOK(target, replace, orig) DobbyHook((void *)(target), (void *)(replace), (void **)(orig))
#define BNM_UNHOOK(ptr) DobbyDestroy((void *)(ptr))
```

### B. Configuring ShadowHook
```cpp
#include <shadowhook.h>
#define BNM_HOOK(target, replace, orig) shadowhook_hook_func_addr((void *)(target), (void *)(replace), (void **)(orig))
#define BNM_UNHOOK(ptr) shadowhook_unhook((void *)(ptr))
```

### C. Configuring KittyMemory / Substrate
```cpp
#include <KittyMemory/KittyMemory.h>
// or substrate MSHookFunction:
#define BNM_HOOK(target, replace, orig) MSHookFunction((void *)(target), (void *)(replace), (void **)(orig))
```

### D. Disabling Hooking (Inspection / Reflection Only)
If you do not plan to use basic inline hooking, define:
```cpp
#define BNM_DISABLE_HOOKING
```

---

## ⚙️ Feature Configuration Macros

| Macro | Default | Description |
| :--- | :--- | :--- |
| `BNM_DEBUG` | `false` | Enables verbose debug logging for class resolution, method resolution, and hooks. |
| `BNM_CLASSES_MANAGEMENT` | `true` | Enables custom runtime class registration (`BNM_CustomClass`). |
| `BNM_COROUTINE` | `true` | Enables C++20 Unity coroutine engine (`BNM::Coroutine::IEnumerator`). |
| `BNM_USE_APPDOMAIN` | `false` | Enables deep AppDomain inspection when searching for assemblies. |
| `BNM_ALLOW_STRIP_MOD_OFFSET` | `true` | Enables fallback resolving for stripped method names. |
| `BNM_USE_UNSAFEMULTITHREADING` | `false` | Disables internal mutex locks for maximum single-threaded performance. |
| `UNITY_VER` | Auto | Overrides target Unity version (e.g. `20232` for 2023.2+, `6000` for Unity 6). Defaults to auto-detection from the loaded binary. |

---

## 🚀 Loading Strategies (`BNM::Loading`)

BNM must hook into IL2CPP runtime functions to parse internal structures before game scripts execute. BNM provides multiple loading strategies for different injection environments:

### Strategy 1: Automatic Loading via JNI (`TryLoadByJNI`)
Best for APK modding, zygote injection, or native activity hooks where a `JNIEnv *` is accessible.

```cpp
#include <jni.h>
#include <BNM/Loading.hpp>

void OnBNMLoaded() {
    BNM_LOG_INFO("[MyMod] BNM Initialized successfully via JNI!");
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    vm->GetEnv((void **)&env, JNI_VERSION_1_6);

    // Register callback for when IL2CPP runtime is ready
    BNM::Loading::AddOnLoadedEvent(OnBNMLoaded);

    // Automatically locate libil2cpp.so and set up loading hooks
    BNM::Loading::TryLoadByJNI(env);

    return JNI_VERSION_1_6;
}
```

---

### Strategy 2: Dynamic Handle Loading (`TryLoadByDlfcnHandle`)
Best when loading from an injected thread where `dlopen` is used:

```cpp
#include <dlfcn.h>
#include <thread>
#include <BNM/Loading.hpp>

void HackThread() {
    void *il2cppHandle = nullptr;
    while (!il2cppHandle) {
        il2cppHandle = dlopen("libil2cpp.so", RTLD_NOLOAD);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    BNM::Loading::AddOnLoadedEvent([]() {
        BNM_LOG_INFO("[MyMod] BNM Initialized via dlfcn handle!");
    });

    BNM::Loading::TryLoadByDlfcnHandle(il2cppHandle);
}
```

---

### Strategy 3: Loading with KittyMemory (`SetMethodFinder` & `TryLoadByUsersFinder`)
Best for protected games where standard dynamic symbols (`dlsym`) are stripped or blocked:

```cpp
#include <KittyMemory/KittyMemory.h>
#include <KittyScanner/KittyScanner.h>
#include <BNM/Loading.hpp>

void HackThreadKitty() {
    KittyScanner::ElfScanner il2cppScanner;
    while (!il2cppScanner.isValid()) {
        il2cppScanner = KittyScanner::findElf("libil2cpp.so");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Set custom symbol finder using KittyScanner
    BNM::Loading::SetMethodFinder([](const char *name, void *userData) -> void * {
        auto scanner = (KittyScanner::ElfScanner *)userData;
        return (void *)scanner->findSymbol(name);
    }, &il2cppScanner);

    BNM::Loading::AddOnLoadedEvent([]() {
        BNM_LOG_INFO("[MyMod] BNM Initialized via KittyMemory symbol finder!");
    });

    // Install loading hooks using custom finder
    BNM::Loading::TryLoadByUsersFinder();
}
```

---

### Strategy 4: Late Initialization Hook (`AllowLateInitHook`)
If your mod is injected after the game has already completed startup (e.g. main menu or mid-game), call `AllowLateInitHook()` before the loading method:

```cpp
// Allow BNM to hook il2cpp::vm::Class::FromIl2CppType for late initialization
BNM::Loading::AllowLateInitHook();
BNM::Loading::TryLoadByDlfcnHandle(il2cppHandle);
```

> **Warning**: When using late initialization, `BNM_UNHOOK` must be defined so BNM can unhook the initialization trap after loading to avoid frame drops.

---

### Checking Load Status
```cpp
if (BNM::Loading::IsLoaded()) {
    void *handle = BNM::Loading::GetIl2CppLibraryHandle();
    BNM_LOG_INFO("IL2CPP Handle: %p", handle);
}
```
