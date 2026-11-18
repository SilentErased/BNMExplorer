# Reflection API: Images, Classes, Methods, Fields, Properties & Events

BNM provides an expressive C++ abstraction layer modeled after C# Reflection. You can look up classes, invoke methods, read/write fields, interact with properties, bind event handlers, and declare custom events without needing hardcoded memory offsets.

---

## 1. Images & Assemblies (`BNM::Image`)

An Image represents a loaded .NET assembly (e.g. `Assembly-CSharp.dll`, `UnityEngine.CoreModule.dll`, `mscorlib.dll`).

```cpp
#include <BNM/Image.hpp>
#include <BNM/Class.hpp>

// 1. Find an image by name
BNM::Image assemblyCSharp("Assembly-CSharp.dll");
BNM::Image coreModule("UnityEngine.CoreModule.dll");

// 2. Look up classes scoped to an image
BNM::Class playerClass = assemblyCSharp.GetClass("Game.Characters", "PlayerController");

// 3. Enumerate all classes in an assembly
std::vector<BNM::Class> allClasses = assemblyCSharp.GetClasses();
```

---

## 2. Classes (`BNM::Class`)

### A. Finding Classes
```cpp
#include <BNM/Class.hpp>

// Look up class by namespace and name across all loaded images
BNM::Class playerClass("Game.Characters", "PlayerController");

// Validate existence
if (playerClass.IsValid()) {
    BNM_LOG_INFO("Class found! Instance size: %zu", playerClass.GetInstanceSize());
}
```

### B. Nested Class Resolver (`Outer+Inner` & `Outer/Inner`)
BNM automatically parses and resolves nested class hierarchies using `+` or `/` delimiters:

```cpp
// Automatically resolves root "UnityEngine.UI.Dropdown" then "OptionData" inner class
BNM::Class optionDataClass("UnityEngine.UI", "Dropdown+OptionData");

// Deep nested class parsing:
BNM::Class subInnerClass("MyNamespace", "RootClass+ChildClass+GrandChildClass");
```

### C. Class Inspection & Hierarchy
```cpp
// Get parent base class
BNM::Class parentClass = playerClass.GetParent();

// Get implemented interfaces
std::vector<BNM::Class> interfaces = playerClass.GetInterfaces();

// Check if class derives from or implements another class
bool isMonoBehaviour = playerClass.IsSubclassOf(BNM::Defaults::Get<BNM::UnityEngine::MonoBehaviour *>().ToClass());

// Box & Unbox value types
int number = 42;
BNM::IL2CPP::Il2CppObject *boxed = BNM::Defaults::Get<int>().ToClass().BoxObject(&number);
int unboxed = *(int *)BNM::Defaults::Get<int>().ToClass().UnboxObject(boxed);
```

### D. Instantiation (`CreateNewInstance`, `CreateNewObject`, `NewArray`, `NewList`)
```cpp
// 1. Allocate raw object WITHOUT calling constructor (.ctor)
BNM::IL2CPP::Il2CppObject *rawInstance = playerClass.CreateNewInstance();

// 2. Allocate object AND call constructor with parameters
auto player = playerClass.CreateNewObjectParameters(100 /* maxHealth */, true /* isVip */);
// or by parameter names:
auto player2 = playerClass.CreateNewObjectTypes({"maxHealth", "isVip"}, 100, true);

// 3. Allocate C# 1D Array
auto playerArray = playerClass.NewArray<BNM::IL2CPP::Il2CppObject *>(10);

// 4. Allocate standard C# List<T>
auto playerList = playerClass.NewList<BNM::IL2CPP::Il2CppObject *>();

// 5. Allocate List with BNM custom vtable initialization (useful for custom structs)
auto customList = playerClass.NewListBNM<BNM::IL2CPP::Il2CppObject *>();
```

### E. Generic Classes
```cpp
// System.Collections.Generic.Dictionary`2
BNM::Class dictClass = BNM::Class("System.Collections.Generic", "Dictionary`2")
    .GetGeneric({BNM::Defaults::Get<BNM::Structures::Mono::String *>(), BNM::Defaults::Get<int>()});

auto myDict = dictClass.CreateNewInstance();
```

---

## 3. Methods (`BNM::Method<T>` & `BNM::MethodBase`)

### A. Typed Methods & Invocations
```cpp
#include <BNM/Method.hpp>

auto playerClass = BNM::Class("Game", "Player");

// Method signature: void TakeDamage(int amount, bool critical)
BNM::Method<void> takeDamageMethod = playerClass.GetMethod("TakeDamage", 2);

// Invoke on an instance via Call() or operator[]
takeDamageMethod[playerInstance](50, true);
// or:
takeDamageMethod.Call(playerInstance, 50, true);
```

### B. Static Methods
```cpp
auto timeClass = BNM::Class("UnityEngine", "Time");
BNM::Method<float> getDeltaTime = timeClass.GetMethod("get_deltaTime", 0);

// Invoke static method
float dt = getDeltaTime(); // or getDeltaTime.Call();
```

### C. Overloaded Methods by Parameter Types or Names
```cpp
// Overloaded by parameter type names: void Attack(float power, Target target)
auto attackMethod = playerClass.GetMethod("Attack", {"System.Single", "Game.Target"});

// Overloaded by parameter argument names:
auto playAudio = audioClass.GetMethod("Play", {"clipName", "volume"});
```

### D. Generic Methods
```cpp
// Method: T GetComponent<T>()
auto getComponentMethod = gameObjectClass.GetMethod("GetComponent", 0);

// Specialize for Rigidbody component
auto getRigidbody = getComponentMethod.GetGeneric({rigidbodyClass});
auto rb = getRigidbody[goInstance].cast<BNM::IL2CPP::Il2CppObject *>()();
```

---

## 4. Fields (`BNM::Field<T>` & `BNM::FieldBase`)

### A. Instance Fields
```cpp
#include <BNM/Field.hpp>

BNM::Field<int> healthField = playerClass.GetField("currentHealth");

// Read value via operator[]() or Get()
int currentHp = healthField[playerInstance]();

// Write value via operator[]= or Set()
healthField[playerInstance] = currentHp + 50;
healthField[playerInstance].Set(1000);

// Get direct pointer to value in object memory
int *hpPtr = healthField[playerInstance].GetPointer();

// Get field offset in class memory
size_t offset = healthField.GetOffset();
```

### B. Static Fields
```cpp
BNM::Field<BNM::IL2CPP::Il2CppObject *> instanceField = playerClass.GetField("Instance");

// Read static field
auto singleton = instanceField(); // or instanceField.Get();

// Set static field
instanceField = nullptr;
```

---

## 5. Properties (`BNM::Property<T>` & `BNM::PropertyBase`)

Properties in C# encapsulate getter (`get_`) and setter (`set_`) methods:

```cpp
#include <BNM/Property.hpp>

BNM::Property<BNM::Structures::Mono::String *> nameProp = playerClass.GetProperty("PlayerName");

// Read property (invokes get_PlayerName)
auto nameStr = nameProp[playerInstance]();

// Write property (invokes set_PlayerName)
nameProp[playerInstance].Set(BNM::CreateMonoString("ProGamer"));

// Access underlying getter/setter methods
BNM::MethodBase getter = nameProp.GetGetter();
BNM::MethodBase setter = nameProp.GetSetter();
```

---

## 6. Events (`BNM::Event<Ret, Parameters...>` & `BNM::EventBase`)

C# events wrap `add_` and `remove_` event subscription methods.

### A. Subscribing with Delegates & Callables
```cpp
#include <BNM/Event.hpp>
#include <BNM/Delegates.hpp>

// Event signature: public event Action<int, bool> OnPlayerStateChanged;
BNM::Event<void, int, bool> stateEvent = playerClass.GetEvent("OnPlayerStateChanged");

// 1. Subscribe directly using a C++ lambda
auto delegatePtr = stateEvent[playerInstance].Add([](int state, bool active) {
    BNM_LOG_INFO("Player state changed to %d (active: %d)", state, active);
});

// Or using operator+=
stateEvent[playerInstance] += [](int state, bool active) {
    BNM_LOG_INFO("Second event listener triggered!");
};

// 2. Unsubscribe
stateEvent[playerInstance].Remove(delegatePtr);
// or:
stateEvent[playerInstance] -= delegatePtr;

// 3. Raising the event manually
stateEvent[playerInstance].Raise(1, true);
// or:
stateEvent[playerInstance](1, true);
```

### B. RAII Scoped Event Listeners (`ScopedEventListener` & `Listen`)
Automatically unregisters the event listener when the RAII guard leaves scope:

```cpp
void RegisterTemporaryListener(BNM::IL2CPP::Il2CppObject *player) {
    auto deathEvent = playerClass.GetEvent<void>("OnPlayerDied");

    // Scoped listener will automatically unsubscribe on function exit or object destruction
    auto guard = BNM::Listen(deathEvent[player], []() {
        BNM_LOG_INFO("Player died during scoped monitoring period!");
    });
}
```

---

## 7. Custom Thread-Safe C++ Events (`BNM::CustomEvent<Signature>`)

BNM provides a standalone thread-safe, re-entrant event dispatcher in `BNM/CustomEvent.hpp` for mod architectures:

```cpp
#include <BNM/CustomEvent.hpp>

// Declare an event with signature void(int, float)
BNM::CustomEvent<void(int, float)> OnScoreAdded;

// Register listeners (lambdas, std::function, or IL2CPP Delegate*)
auto listenerId = OnScoreAdded += [](int score, float multiplier) {
    BNM_LOG_INFO("Score added: %d with multiplier %.2f", score, multiplier);
};

// Fire event
OnScoreAdded(500, 1.5f);

// Unregister listener
OnScoreAdded -= listenerId;
```

---

## 8. Pointer Dispatch Operators (`BNM::Operators`)

For cleaner syntax, include `BNM/Operators.hpp` to bind instances directly:

```cpp
#include <BNM/Operators.hpp>
using namespace BNM::Operators;

// Using operator ->*
playerInstance ->* takeDamageMethod(50, true);
int hp = *(playerInstance ->* healthField);
playerInstance ->* nameProperty.Set(BNM::CreateMonoString("Hero"));

// Using operator >>
playerInstance >> healthField = 1000;
playerInstance >> takeDamageMethod(100, false);
```

---

## 9. Exception Handling

BNM provides macro-based and lambda-based mechanisms to catch IL2CPP C# runtime exceptions:

### A. Macro Exception Guard (`BNM_try` / `BNM_catch`)
```cpp
#include <BNM/Exceptions.hpp>

BNM_try {
    dangerMethod[instance]();
} BNM_catch(ex) {
    BNM_LOG_ERR("Caught C# Exception: %s", ex.Message().c_str());
    BNM_LOG_ERR("Stack Trace: %s", ex.StackTrace().c_str());
} BNM_end_try;
```

### B. Lambda-based Guard (`BNM::TryInvoke`)
```cpp
auto ex = BNM::TryInvoke([&]() {
    dangerMethod[instance]();
});

if (ex.IsValid()) {
    BNM_LOG_ERR("Exception in Class %s: %s", ex.ClassName().c_str(), ex.Message().c_str());
}
```
