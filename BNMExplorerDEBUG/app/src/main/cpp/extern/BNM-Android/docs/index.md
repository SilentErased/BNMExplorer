# BNM (ByNameModding) Documentation

Welcome to the official documentation for **BNM (ByNameModding)** — a modern, high-performance C++20 library for inspecting, modifying, hooking, and extending Unity IL2CPP games at runtime on Android based on symbol and metadata name resolution.

---

## 📚 Complete Documentation Suite

1. **[Getting Started](getting-started.md)**
   - System requirements & Android NDK toolchain
   - CMake & ndk-build (`Android.mk`) integration
   - Hook backend setup: Dobby, ShadowHook, KittyMemory, Substrate, or `BNM_DISABLE_HOOKING`
   - Complete Loading strategies (`BNM::Loading`): JNI, dlfcn handles, KittyMemory / `KittyScanner` custom method finders, late initialization (`AllowLateInitHook`)

2. **[Configuration & Settings](configuration.md)**
   - Comprehensive `BNM_Config.h` & `GlobalSettings.hpp` reference
   - Full `UNITY_VER` compatibility table (Unity 5.6 to Unity 6000.x LTS)
   - Modular feature toggles (Physics, UI, Audio, Animation, Renderers, TextMeshPro)
   - Diagnostic macros, thread synchronization, memory allocators, and string obfuscation

3. **[Reflection API](reflection-api.md)**
   - Assemblies (`BNM::Image`) & Classes (`BNM::Class`)
   - Automatic nested class resolver (`Outer+Inner` / `Outer/Inner`)
   - Typed Methods (`BNM::Method<T>`) & dynamic invocations
   - Generic types & generic method specialization
   - Fields (`BNM::Field<T>`) & Properties (`BNM::Property<T>`)
   - Event listeners (`BNM::Event<Ret, Args...>`), lambda subscriptions, and RAII scoped guards (`ScopedEventListener`, `Listen`)
   - Pointer dispatch operators (`BNM::Operators`)
   - Exception handling (`BNM_try`, `BNM_catch`, `BNM::TryInvoke`)

4. **[Static Game Bindings](game-binding.md)**
   - Compile-time strongly-typed C++ game bindings (`BNM_GAME_CLASS`, `BNM_GAME_CLASS_END`)
   - Nested game classes (`BNM_GAME_INNER_CLASS`) & C# interfaces (`BNM_GAME_INTERFACE`)
   - Declarative fields (`BNM_BINDING_FIELD`), properties (`BNM_BINDING_PROPERTY`), and methods (`BNM_BINDING_METHOD`)
   - Method overload resolution via parameter types and parameter names
   - Dynamic interface querying & casting (`.As<T>()`, `.IsA<T>()`)
   - Generic classes & methods (`BNM_GAME_GENERIC_CLASS_*`, `BNM_BINDING_*GENERIC_METHOD_*`)
   - **[Game Binding Generator](game-binding-generator.md)** (`BNM_GameBinding_Generator.py`): generate the whole game's bindings from `dump.cs`

5. **[Custom Classes & Hooking](custom-classes-and-hooking.md)**
   - Creating runtime C# classes (`BNM_CustomClass`, `BNM_CustomField`, `BNM_CustomMethod`)
   - Compile-time class construction (`CompileTimeClassBuilder`)
   - Method hooking via custom classes (`BNM_CustomMethodMarkAsBasicHook`, `BNM_CustomMethodMarkAsInvokeHook`, `BNM_CustomMethodSkipTypeMatch`)
   - Calling original implementations (`BNM_CallCustomMethodOrigin`)
   - Dynamic AOT inline hooking (`BNM::InvokeHook`, `BNM::BasicHook`)
   - Runtime class registration (`BNM::ClassesManagement::ProcessClassRuntime`)

6. **[Mono Structures (.NET Data Types)](mono-structures.md)**
   - C# Strings (`Mono::String`, `BNM::CreateMonoString`)
   - C# Arrays (`Mono::Array<T>`)
   - C# Lists (`Mono::List<T>`)
   - Tombstone-safe C# Dictionaries (`Mono::Dictionary<TKey, TValue>`)
   - Extended types: `DateTime`, `TimeSpan`, `Guid`, `Nullable<T>`, `KeyValuePair<K, V>`
   - Delegates & Actions in depth (`BNM::Delegate`, `BNM::MulticastDelegate`, `BNM::Action`, `BNM::CreateAction`, `BNM::CreateFunc`, `BNM::CreatePredicate`, `BNM::CreateComparison`, `BNM::CreateEventHandler`, `BNM::CreateDelegate`)

7. **[Unity Structures (Engine Components & Math)](unity-structures.md)**
   - `GameObject` & `Transform` hierarchy operations
   - Global Scene Hierarchy Discovery (`SceneManager::GetAllRootGameObjects`)
   - 3D Math (`Vector2`, `Vector3`, `Vector4`, `Quaternion`, `Matrix3x3`, `Matrix4x4`, `Color`, `Rect`, `Ray`)
   - 3D/2D Physics & Raycasting (`Physics::Raycast`, `RaycastHit`, `Physics2D`)
   - Renderers (`Camera`, `MeshRenderer`, `SkinnedMeshRenderer`, `SpriteRenderer`, `Material`, `Shader`, `Texture2D`)
   - Audio (`AudioSource`, `AudioClip`) & Animation (`Animator`, `Animation`)
   - Engine Services (`Time`, `Screen`, `Input`, `PlayerPrefs`, `Application`, `SystemInfo`, `Resources`)

8. **[Runtime UI & Android Touch Input](runtime-ui-and-events.md)**
   - 1-line Overlay Canvas creation (`CanvasHelper::CreateOverlayCanvas`)
   - Automated `EventSystem` & Android Touch Input Routing (`EventSystem::EnsureEventSystem`)
   - UI Components: `Button`, `Text`, `Image`, `Slider`, `Toggle`, `InputField`, `TextMeshProUGUI`
   - UnityAction event wiring (`BNM::CreateUnityAction`)

9. **[Asynchronous, Coroutines & Tasks](async-coroutine-task.md)**
   - C++20 `co_await` Task Bridge (`BNM::Task<T>`) for `System.Threading.Tasks.Task<T>` & `UniTask`
   - Unity Coroutine Engine (`BNM::Coroutine::IEnumerator`, all `YieldInstruction` types, `.Get()`, `()`)
   - Asynchronous Scene Loading (`AsyncOperation`)

10. **[Utilities, Memory & Scanning](utilities-and-scanning.md)**
    - Built-in AOB Byte Pattern Scanner (`PatternScan`, `PatternScanModule`)
    - Bidirectional external icall resolution (`GetExternMethod`)
    - Tagged pointer GC handle unmarshaling (`UnmarshalUnityObject`)
    - Thread attach/detach (`AttachIl2Cpp`, `DetachIl2Cpp`) & GC memory management
    - GC handles (`NewGCHandle`, `FreeGCHandle`) for keeping managed objects alive from native code
    - Memory hex dump & path utilities (`HexDump`, `GetDirectory`)
