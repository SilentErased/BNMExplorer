# Custom Classes & Method Hooking

BNM provides a powerful dual-system for runtime class creation and method interception:
1. **Custom Classes Management (`BNM_CustomClass`)**: Compile-time declarative macros to create new classes, inject fields/methods, or hook existing classes at the class metadata level.
2. **Dynamic Runtime Hooking (`BasicHook`, `InvokeHook`, `VirtualHook`)**: Dynamic interception of native methods, invokers, or virtual tables.

---

## 1. Creating Custom Classes (`BNM_CustomClass`)

BNM allows runtime registration of custom classes that can inherit from `UnityEngine.MonoBehaviour`, `UnityEngine.Component`, or any base C# class.

```cpp
#include <BNM/ClassesManagement.hpp>
#include <BNM/UnityStructures/MonoBehaviour.hpp>

// Define a custom MonoBehaviour component
struct MyCustomModComponent : public BNM::UnityEngine::MonoBehaviour {
    // 1. Declare custom class metadata (TargetType, BaseType, Owner, Interfaces...)
    BNM_CustomClass(MyCustomModComponent, 
        BNM::CompileTimeClassBuilder("MyMod.Components", "ModController").Build(),
        BNM::Defaults::Get<BNM::UnityEngine::MonoBehaviour *>(),
        {}
    );

    // Constructor override to properly initialize C++ fields/vtable
    void Constructor() {
        BNM::UnityEngine::MonoBehaviour tmp = *this;
        *this = MyCustomModComponent();
        *((BNM::UnityEngine::MonoBehaviour *)this) = tmp;
    }
    BNM_CustomMethod(Constructor, false, BNM::Defaults::Get<void>(), ".ctor");

    // 2. Custom Fields
    float speedMultiplier = 1.0f;
    BNM_CustomField(speedMultiplier, BNM::Defaults::Get<float>(), "speedMultiplier");

    // 3. Custom Methods & Lifecycle
    void Awake() {
        BNM_LOG_INFO("[ModController] Component Awake on GameObject: %p", this->GetGameObject());
    }
    BNM_CustomMethod(Awake, false, BNM::Defaults::Get<void>(), "Awake");

    void Update() {
        // Logic executed every frame
    }
    BNM_CustomMethod(Update, false, BNM::Defaults::Get<void>(), "Update");
};
```

### Adding Custom Component to a GameObject at Runtime:
```cpp
// Use BNMCustomClass.type to add the component to any GameObject
BNM::UnityEngine::GameObject *go = BNM::UnityEngine::GameObject::Find("Player");
auto modComponent = (MyCustomModComponent *)go->AddComponent(MyCustomModComponent::BNMCustomClass.type);
```

---

## 2. Modifying & Hooking via Custom Class Macros

You can intercept and hook existing Unity game classes directly using the custom class macro suite:

```cpp
#include <BNM/ClassesManagement.hpp>
#include <BNM/UnityStructures/MonoBehaviour.hpp>

struct HookedPlayer : public BNM::UnityEngine::MonoBehaviour {
    // Target an existing class in Assembly-CSharp.dll
    BNM_CustomClass(HookedPlayer,
        BNM::CompileTimeClassBuilder({}, "PlayerController", "Assembly-CSharp.dll").Build(),
        BNM::Defaults::Get<BNM::UnityEngine::MonoBehaviour *>(),
        {}
    );

    // Overridden/Hooked method
    void TakeDamage(int damage, bool crit) {
        BNM_LOG_INFO("TakeDamage intercepted! Original damage: %d", damage);

        // Call original implementation (God Mode: pass 0 damage)
        BNM_CallCustomMethodOrigin(TakeDamage, this, 0, crit);
    }

    BNM_CustomMethod(TakeDamage, false, BNM::Defaults::Get<void>(), "TakeDamage", BNM::Defaults::Get<int>(), BNM::Defaults::Get<bool>());
    BNM_CustomMethodMarkAsBasicHook(TakeDamage);
    BNM_CustomMethodSkipTypeMatch(TakeDamage);
};
```

### Available Custom Method Modifiers
- `BNM_CustomMethodMarkAsBasicHook(method)`: Prefers inline AOT machine code hook (using Dobby/ShadowHook).
- `BNM_CustomMethodMarkAsInvokeHook(method)`: Prefers MethodInfo invoker redirection (avoids modifying code pages).
- `BNM_CustomMethodSkipTypeMatch(method)`: Skips parameter type verification and matches by argument count.
- `BNM_CustomMethodCopyAttributes(method, copy_target)`: Copies runtime metadata attributes from an existing method.
- `BNM_CallCustomMethodOrigin(method, ...)`: Calls the original method implementation.

---

## 3. Dynamic Method Hooking Methods

BNM provides three distinct runtime hooking techniques:

### A. Inline Machine Code Hook (`BNM::BasicHook`)
Replaces instructions in memory using your configured hooking software (Dobby, ShadowHook, MSHookFunction):

```cpp
#include <BNM/MethodBase.hpp>

// Original function pointer holder
void (*orig_TakeDamage)(void *instance, int damage, bool crit) = nullptr;

// Hook function
void hook_TakeDamage(void *instance, int damage, bool crit) {
    BNM_LOG_INFO("TakeDamage called via BasicHook!");
    orig_TakeDamage(instance, 0, crit); // God mode
}

void SetupBasicHook() {
    auto method = BNM::Class("Game", "Player").GetMethod("TakeDamage", 2);

    // Hook native machine code at method pointer
    BNM::BasicHook(method, (void *)hook_TakeDamage, (void **)&orig_TakeDamage);
}
```

### B. MethodInfo Invoker Redirection (`BNM::InvokeHook`)
Modifies the `MethodInfo` pointer and invoker in memory without patching executable pages. Excellent for read-only memory or anti-cheat protected code segments:

```cpp
void (*orig_Invoke)(void *instance, int damage, bool crit) = nullptr;

void SetupInvokeHook() {
    auto method = BNM::Class("Game", "Player").GetMethod("TakeDamage", 2);

    // Replaces MethodInfo pointer and invoker
    BNM::InvokeHook(method, (void *)hook_TakeDamage, (void **)&orig_Invoke);
}
```

### C. Virtual Table Hook (`BNM::VirtualHook`)
Modifies the class virtual method table (`vtable`) slot for polymorphic virtual methods:

```cpp
void (*orig_VirtualMethod)(void *instance) = nullptr;

void SetupVirtualHook() {
    BNM::Class playerClass("Game", "Player");
    auto virtualMethod = playerClass.GetMethod("OnUpdate", 0);

    // Replaces the vtable slot in playerClass
    BNM::VirtualHook(playerClass, virtualMethod, (void *)hook_Update, (void **)&orig_VirtualMethod);
}
```

---

## 4. Late Runtime Class Processing (`ProcessClassRuntime`)

To register and process custom classes dynamically after initial BNM initialization:

```cpp
// Warning: Must be called from an attached IL2CPP thread!
BNM::AttachIl2Cpp();
BNM::ClassesManagement::ProcessClassRuntime(&MyCustomModComponent::BNMCustomClass);
```
