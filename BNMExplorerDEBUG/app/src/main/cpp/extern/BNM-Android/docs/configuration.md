# Complete Configuration & Settings Guide

BNM is configured through `include/BNM/UserSettings/BNM_Config.h`, `GlobalSettings.hpp`, or compile-time preprocessor definitions (`-D...`).

---

## 1. External Configuration Files

BNM detects configuration files in the following priority order:
1. `BNM_USER_CONFIG` (Path defined by macro, e.g. `-DBNM_USER_CONFIG="MyModConfig.h"`)
2. `"BNM_Config.h"` or `"BNM_UserConfig.h"` (Local include path)
3. `<BNM_Config.h>` or `<BNM_UserConfig.h>` (System include path)

---

## 2. Unity Version Selection (`UNITY_VER`)

BNM automatically adapts IL2CPP metadata offsets, struct layouts, and method invoke signatures based on `UNITY_VER`:

| Macro Value | Target Unity Version |
| :--- | :--- |
| `#define UNITY_VER 56` | Unity 5.6.4f1 |
| `#define UNITY_VER 171` | Unity 2017.1.x |
| `#define UNITY_VER 172` | Unity 2017.2.x – 2017.4.x |
| `#define UNITY_VER 181` | Unity 2018.1.x |
| `#define UNITY_VER 182` | Unity 2018.2.x |
| `#define UNITY_VER 183` | Unity 2018.3.x – 2018.4.x |
| `#define UNITY_VER 191` | Unity 2019.1.x – 2019.2.x |
| `#define UNITY_VER 193` | Unity 2019.3.x |
| `#define UNITY_VER 194` | Unity 2019.4.x |
| `#define UNITY_VER 201` | Unity 2020.1.x |
| `#define UNITY_VER 202` | Unity 2020.2.x – 2020.3.19 |
| `#define UNITY_VER 203` | Unity 2020.3.20 – 2020.3.xx |
| `#define UNITY_VER 211` | Unity 2021.1.x |
| `#define UNITY_VER 212` | Unity 2021.2.x |
| `#define UNITY_VER 213` | Unity 2021.3.x |
| `#define UNITY_VER 221` | Unity 2022.1.x |
| `#define UNITY_VER 222` | Unity 2022.2.x – 2022.3.x *(Default)* |
| `#define UNITY_VER 231` | Unity 2023.1.x |
| `#define UNITY_VER 232` | Unity 2023.2.x – 2023.3.x |
| `#define UNITY_VER 600` | Unity 6000.0.x – 6000.4.x |
| `#define UNITY_VER 605` | Unity 6000.5.x |
| `#define UNITY_VER 606` | Unity 6000.6.x+ |

> **Special Patches**: Set `#define UNITY_PATCH_VER 24` if using Unity 2021.1 with patch version >= 24.

> **Unity 6 layout notes**: Unity 6000.3.5 replaced `Il2CppClass.rgctx_data` with the `Il2CppClass_InitDataUnion init_data` union (same offset; BNM handles this automatically). Unity 6000.5 moved `events`/`properties`/`nestedTypes` behind `const void*` and Unity 6000.6 merged `implementedInterfaces`/`interfaceOffsets` into `Il2CppRuntimeInterfaceData* interfaces` — which is why separate `UNITY_VER` values are required per Unity 6 layout.

---

## 3. Hooking Framework Integration

BNM requires hook macros for inline hooking features (`BasicHook` and `BNM_CustomMethodMarkAsBasicHook`):

### A. Dobby Hook
```cpp
#include <dobby.h>
#define BNM_HOOK(target, replace, orig) DobbyHook((void *)(target), (void *)(replace), (void **)(orig))
#define BNM_UNHOOK(ptr) DobbyDestroy((void *)(ptr))
```

### B. ShadowHook
```cpp
#include <shadowhook.h>
#define BNM_HOOK(target, replace, orig) shadowhook_hook_func_addr((void *)(target), (void *)(replace), (void **)(orig))
#define BNM_UNHOOK(ptr) shadowhook_unhook((void *)(ptr))
```

### C. KittyMemory / Substrate
```cpp
#include <KittyMemory/KittyMemory.h>
#define BNM_HOOK(target, replace, orig) MSHookFunction((void *)(target), (void *)(replace), (void **)(orig))
```

### D. Disabling Hooking (Inspection / Reflection Only)
```cpp
#define BNM_DISABLE_HOOKING
```

---

## 4. Modular Feature Toggles

By default, all modules are enabled. You can disable unused modules to reduce binary size:

| Module Macro | Disable Flag | Description |
| :--- | :--- | :--- |
| `BNM_CLASSES_MANAGEMENT` | `BNM_DISABLE_CLASSES_MANAGEMENT` | Runtime class registration & modification (`BNM_CustomClass`). |
| `BNM_COROUTINE` | `BNM_DISABLE_COROUTINE` | C++20 Unity coroutine engine (`BNM::Coroutine::IEnumerator`). |
| `BNM_UNITY_PHYSICS` | `BNM_DISABLE_UNITY_PHYSICS` | 3D Physics components (`Physics::Raycast`, `Collider`, `Rigidbody`). |
| `BNM_UNITY_PHYSICS2D` | `BNM_DISABLE_UNITY_PHYSICS2D` | 2D Physics components (`Physics2D::Raycast`, `RaycastHit2D`). |
| `BNM_UNITY_UI` | `BNM_DISABLE_UNITY_UI` | uGUI components (`Canvas`, `Button`, `Slider`, `Toggle`, `InputField`). |
| `BNM_UNITY_TEXTMESHPRO` | `BNM_DISABLE_UNITY_TEXTMESHPRO` | TextMeshPro integration (`TextMeshProUGUI`). |
| `BNM_UNITY_RENDERERS` | `BNM_DISABLE_UNITY_RENDERERS` | `MeshRenderer`, `SkinnedMeshRenderer`, `SpriteRenderer`, `Mesh`. |
| `BNM_UNITY_AUDIO` | `BNM_DISABLE_UNITY_AUDIO` | `AudioSource`, `AudioClip`, `AudioListener`. |
| `BNM_UNITY_ANIMATION` | `BNM_DISABLE_UNITY_ANIMATION` | `Animator`, `Animation`, `AnimationClip`, `AnimationState`. |

> **Manual Mode**: Define `#define BNM_MANUAL_MODULES` to disable all modules by default and selectively define only the ones you need.

---

## 5. Dynamic Loader Overrides

By default, BNM uses Android's `<dlfcn.h>` functions. You can redirect them to custom loaders (e.g. `xdl` or `KittyScanner`):

```cpp
#define BNM_dlopen(name, flags)    my_custom_dlopen(name, flags)
#define BNM_dlsym(handle, symbol)  my_custom_dlsym(handle, symbol)
#define BNM_dlclose(handle)        my_custom_dlclose(handle)
#define BNM_dladdr(addr, info)     my_custom_dladdr(addr, info)
```

---

## 6. Logging, Diagnostics & Debugging

| Macro | Description |
| :--- | :--- |
| `BNM_DEBUG` | Enables verbose debug logs for class resolution, method calls, and memory layouts. |
| `BNM_INFO` | Enables informational runtime logs. |
| `BNM_WARNING` | Enables runtime warning logs. |
| `BNM_ERROR` | Enables error logs. |
| `BNM_DISABLE_ALL_LOGS` / `BNM_NO_LOG` | Completely strips and disables all logging statements. |
| `BNM_TAG` | Custom logcat tag (Defaults to `"ByNameModding"`). |
| `BNM_ALLOW_STR_METHODS` | Enables `.str()` methods on structures for string representations. |
| `BNM_ALLOW_SAFE_IS_ALLOCATED` | Uses POSIX signal handling to safely check if a memory pointer is valid. |
| `BNM_ALLOW_SELF_CHECKS` | Performs self-checks on IL2CPP objects before method invocations. |
| `BNM_CHECK_INSTANCE_TYPE` | Verifies instance class compatibility when assigning to fields and methods. |

---

## 7. Thread Safety & Memory Settings

| Macro | Description |
| :--- | :--- |
| `BNM_ALLOW_MULTI_THREADING_SYNC` | Enables `std::shared_mutex` synchronization across BNM internal caching. |
| `BNM_USE_UNSAFEMULTITHREADING` | Disables internal mutex locks for maximum single-threaded performance. |
| `BNM_USE_IL2CPP_ALLOCATOR` | Allocates managed mono arrays via IL2CPP's internal GC allocator instead of libc `malloc`. |
| `BNM_malloc` / `BNM_free` | Custom memory allocator overrides. |

---

## 8. String & Metadata Obfuscation

You can secure sensitive class/method names in your mod binary:

```cpp
// Runtime or compile-time string encryption macro
#define BNM_OBFUSCATE(str) OBFUSCATE_KEY(str, 0x5A)

// Obfuscation macro for temporary initialization strings
#define BNM_OBFUSCATE_TMP(str) OBFUSCATE_TMP(str)
```

---

## 9. Public IL2CPP API Wrappers

BNM resolves a curated set of exported IL2CPP API functions (`il2cpp_*`) at load time and exposes them through safe public wrappers in `include/BNM/Utils.hpp`. All wrappers are null-safe: they return a default value (`nullptr` / `false` / `-1`) if BNM is not loaded yet or the target Unity version does not export the symbol.

| BNM Wrapper | Unity IL2CPP API | Availability |
| :--- | :--- | :--- |
| `BNM::Allocate` / `BNM::Free` | `il2cpp_gc_alloc_fixed` / `il2cpp_gc_free_fixed` | All versions (falls back to `BNM_malloc`/`BNM_free` on Unity < 2021.2) |
| `BNM::NewGCHandle` | `il2cpp_gchandle_new` | All versions |
| `BNM::FreeGCHandle` | `il2cpp_gchandle_free` | All versions |
| `BNM::GetGCHandleTarget` | `il2cpp_gchandle_get_target` | All versions |
| `BNM::NewWeakGCHandle` | `il2cpp_gchandle_new_weakref` | All versions |
| `BNM::RuntimeClassInit` | `il2cpp_runtime_class_init` | All versions |
| `BNM::RuntimeInvoke` | `il2cpp_runtime_invoke` | All versions |
| `BNM::GetObjectClass` | `il2cpp_object_get_class` | All versions |
| `BNM::GetClassType` | `il2cpp_class_get_type` | All versions |
| `BNM::GetClassName` | `il2cpp_class_get_name` | All versions |
| `BNM::GetClassNamespace` | `il2cpp_class_get_namespace` | All versions |
| `BNM::GetClassImage` | `il2cpp_class_get_image` | All versions |
| `BNM::GetClassParent` | `il2cpp_class_get_parent` | All versions |
| `BNM::IsClassValueType` | `il2cpp_class_is_valuetype` | All versions |
| `BNM::IsClassEnum` | `il2cpp_class_is_enum` | All versions |
| `BNM::GetMethodFromName` | `il2cpp_class_get_method_from_name` | All versions |
| `BNM::GetFieldFromName` | `il2cpp_class_get_field_from_name` | All versions |
| `BNM::GetClassUserDataOffset` | `il2cpp_class_get_userdata_offset` | Unity 2019.1+ (returns `-1` below) |
| `BNM::SetClassUserData` | `il2cpp_class_set_userdata` | Unity 2019.1+ (no-op below) |
| `BNM::CreateMonoStringLen` | `il2cpp_string_new_len` | All versions |

> **Internal note**: BNM previously resolved non-existent symbols `il2cpp_gc_gchandle_new`/`il2cpp_gc_gchandle_free`; these were renamed to the canonical Unity exports `il2cpp_gchandle_new`/`il2cpp_gchandle_free`. GC handles are passed as `Il2CppGCHandle` (`void*`), which is ABI-compatible with the `uint32_t` handles used by Unity ≤ 2022.3.

> **Version-gated internals**: `il2cpp_image_get_class` is only resolved on Unity ≥ 2018.4 and `il2cpp_gc_alloc_fixed`/`il2cpp_gc_free_fixed` only on Unity ≥ 2021.2; older versions automatically use BNM's manual fallbacks (image walking via `Image::GetTypes`, and `BNM_malloc` respectively).
