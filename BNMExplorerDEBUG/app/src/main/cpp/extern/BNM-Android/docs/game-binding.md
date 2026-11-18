# Static Game Bindings (`GameBinding.hpp`)

BNM includes a declarative C++ macro system in `BNM/GameBinding.hpp` that enables compile-time typed C++ bindings for Unity game classes, fields, properties, methods, and interfaces with zero runtime overhead and automatic lazy caching.

> **Note on macro syntax:** method binding macros take the parameter list twice, as two
> parenthesized groups — `(ParamsDecl), (ParamsCall)`:
> ```cpp
> BNM_BINDING_METHOD(TakeDamage, void, 2, (int damage, bool critical), (damage, critical));
> ```
> A zero-parameter method is `((), ())`. Overload lists (`_TYPES` / `_NAMES`) must also be
> wrapped in parentheses so commas inside them don't split macro arguments:
> `(std::initializer_list<std::string_view>{"text", "keyboardType"})`.

---

## 1. Declaring Game Classes (`BNM_GAME_CLASS`)

Declare a strongly-typed C++ class representing a C# class in the game:

```cpp
#include <BNM/GameBinding.hpp>
#include <BNM/UnityStructures/MonoBehaviour.hpp>

// Syntax: BNM_GAME_CLASS(Namespace, ClassName, BaseClass, [Optional Assembly DLL])
BNM_GAME_CLASS("Game.Characters", PlayerController, BNM::UnityEngine::MonoBehaviour, "Assembly-CSharp.dll")

    // 1. Instance Fields (direct reference access)
    BNM_BINDING_FIELD(currentHealth, int);
    BNM_BINDING_FIELD(movementSpeed, float);

    // 2. Static Fields
    BNM_BINDING_STATIC_FIELD(Instance, PlayerController *);

    // 3. Properties (generates GetIsDead() and SetIsDead(bool))
    BNM_BINDING_PROPERTY(IsDead, bool);

    // 4. Methods by Parameter Count: (ParamsDecl), (ParamsCall)
    BNM_BINDING_METHOD(TakeDamage, void, 2, (int damage, bool critical), (damage, critical));
    BNM_BINDING_METHOD(Respawn, void, 0, (), ());

    // 5. Static Methods
    BNM_BINDING_STATIC_METHOD(GetTotalPlayerCount, int, 0, (), ());

BNM_GAME_CLASS_END
```

---

## 2. Using Game Bindings in Code

Once declared, you can interact with game objects using native C++ syntax:

```cpp
void HandlePlayer(PlayerController *player) {
    if (!player) return;

    // 1. Reading & writing fields directly
    player->currentHealth() = 1000;
    float speed = player->movementSpeed();

    // 2. Accessing properties
    if (!player->GetIsDead()) {
        // 3. Calling methods
        player->TakeDamage(50, false);
    }

    // 4. Static members
    PlayerController *singleton = PlayerController::Instance();
    int totalPlayers = PlayerController::GetTotalPlayerCount();
}
```

---

## 3. Nested Game Classes (`BNM_GAME_INNER_CLASS`)

To bind a nested/inner class:

```cpp
BNM_GAME_CLASS("Game.Weapons", Gun, BNM::UnityEngine::MonoBehaviour)

    // Inner class representing Gun.GunStats
    BNM_GAME_INNER_CLASS(Gun, GunStats, BNM::IL2CPP::Il2CppObject)
        BNM_BINDING_FIELD(fireRate, float);
        BNM_BINDING_FIELD(ammoCapacity, int);
    BNM_GAME_INNER_CLASS_END

    // Outer class field using the inner class type
    BNM_BINDING_FIELD(stats, GunStats *);

BNM_GAME_CLASS_END
```

---

## 4. Game Interfaces & Dynamic Casting (`BNM_GAME_INTERFACE`, `As<T>`, `IsA<T>`)

Declare interfaces and dynamically query whether an object implements them:

```cpp
// Declare C# interface IDamageable
BNM_GAME_INTERFACE("Game.Combat", IDamageable, "Assembly-CSharp.dll")
    BNM_BINDING_METHOD(ApplyDamage, void, 1, (float amount), (amount));
BNM_GAME_INTERFACE_END

// Check and cast dynamically
void OnHit(BNM::UnityEngine::GameObject *hitObject) {
    auto component = hitObject->GetComponent<BNM::UnityEngine::MonoBehaviour *>();

    // Check if component implements IDamageable
    if (component->IsA<IDamageable>()) {
        // Cast to IDamageable interface pointer
        IDamageable *damageable = component->As<IDamageable>();
        damageable->ApplyDamage(100.0f);
    }
}
```

---

## 5. Overloaded Method Resolution

When methods share the same name and parameter count, resolve them using parameter types or parameter names:

### A. Overload by Parameter Types (`BNM_BINDING_METHOD_TYPES`)
```cpp
BNM_GAME_CLASS("Game", CombatSystem, BNM::IL2CPP::Il2CppObject)

    // Attack(float, Target)
    BNM_BINDING_METHOD_TYPES(Attack, void, (std::initializer_list<BNM::CompileTimeClass>{"System.Single", "Game.Target"}), (float power, void *target), (power, target));

    // Attack(int, int)
    BNM_BINDING_METHOD_TYPES(Attack, void, (std::initializer_list<BNM::CompileTimeClass>{"System.Int32", "System.Int32"}), (int minDmg, int maxDmg), (minDmg, maxDmg));

BNM_GAME_CLASS_END
```

### B. Overload by Parameter Names (`BNM_BINDING_METHOD_NAMES`)
```cpp
BNM_GAME_CLASS("Game", AudioSystem, BNM::IL2CPP::Il2CppObject)

    // PlaySound(clipName, volume)
    BNM_BINDING_METHOD_NAMES(PlaySound, void, (std::initializer_list<std::string_view>{"clipName", "volume"}), (const char *clipName, float volume), (clipName, volume));

    // PlaySound(audioId, delay)
    BNM_BINDING_METHOD_NAMES(PlaySound, void, (std::initializer_list<std::string_view>{"audioId", "delay"}), (int audioId, float delay), (audioId, delay));

BNM_GAME_CLASS_END
```

---

## 6. Generic Game Classes & Methods

C# generic types need an explicit `GetGeneric({...})` at resolution time. The `BNM_GAME_GENERIC_CLASS_*` family handles the backtick arity name and the type-argument resolution for you. **Complex types in generic macros are always written inside parentheses** so commas inside template arguments never split macro arguments.

### A. Generic Classes (`BNM_GAME_GENERIC_CLASS_1/2/3`, `_EX`)

```cpp
// C# class: class MySingleton<T> : MonoBehaviour
BNM_GAME_GENERIC_CLASS_1("", MySingleton, (BNM::UnityEngine::MonoBehaviour), T, "Assembly-CSharp.dll")
    BNM_BINDING_GENERIC_STATIC_FIELD(instance, (T *));
    BNM_BINDING_GENERIC_METHOD_1(Take, (T *), T, 0, (), ());
BNM_GAME_GENERIC_CLASS_END

// C# class: class Duo<F, S>
BNM_GAME_GENERIC_CLASS_2("", Duo, (BNM::IL2CPP::Il2CppObject), F, S)
    BNM_BINDING_GENERIC_FIELD(first, (F *));
    BNM_BINDING_GENERIC_FIELD(second, (S *));
BNM_GAME_GENERIC_CLASS_END
```

Type arguments resolve through `BNM::Detail::TypeClassResolver<T>`: game binding structs use their `StaticClass()`, primitives/BNM Unity types fall back to `BNM::Defaults::Get<T>()`. A generic base class (CRTP) works too: `BNM_GAME_GENERIC_CLASS_1("", GameSettings, (GB::MySingleton<T>), T)`.

For a C++ name that differs from the C# name (arity-suffixed overload sets, dotted nested names) or arity ≥ 4, use the `_EX` escape hatch, which takes the full metadata name and the template declaration explicitly:

```cpp
BNM_GAME_GENERIC_CLASS_EX("System", Action_2, "Action`2", (BNM::IL2CPP::Il2CppObject),
    (typename T1, typename T2), (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get()))
BNM_GAME_GENERIC_CLASS_END
```

### B. Generic Methods (`BNM_BINDING_[STATIC_]GENERIC_METHOD_*`)

Generic methods become C++ template methods resolving through `GetMethod(...).GetGeneric({...})`. Each family (by count / `_TYPES` / `_NAMES`) exists for instance and static (`STATIC_` after `BNM_BINDING_`), with arity wrappers `_1` / `_2` / `_3` and an `_EX` form taking `(RetType), (TypeDecl), (Resolvers)`:

```cpp
BNM_GAME_CLASS("", PlayerController, BNM::UnityEngine::MonoBehaviour)

    BNM_BINDING_GENERIC_METHOD_1(Take, (T *), T, 0, (), ());                             // T Take<T>()
    BNM_BINDING_STATIC_GENERIC_METHOD_1(Find, (T *), T, 1, (int id), (id));              // static T Find<T>(int)
    BNM_BINDING_GENERIC_METHOD_TYPES_1(Set, (void), T, (std::initializer_list<BNM::CompileTimeClass>{BNM::Defaults::Get<int>()}), (T * value), (value));

BNM_GAME_CLASS_END
```

> **Explicit template arguments:** a generic method whose type parameters appear only in the return type cannot be deduced and must be called explicitly — `PlayerController::Find<PlayerController>(1)`, `instance->Take<PlayerController>()`.

### C. Fields & Properties with Comma-Bearing Types (`BNM_BINDING_GENERIC_FIELD/PROPERTY`)

The plain field/property macros break on types containing commas (`Dictionary<K, V> *`). The `_GENERIC_` variants take the whole type parenthesized:

```cpp
BNM_BINDING_GENERIC_FIELD(cache, (BNM::Structures::Mono::Dictionary<BNM::Structures::Mono::String, PlayerController *> *));
BNM_BINDING_GENERIC_STATIC_FIELD(table, (BNM::Structures::Mono::List<Duo *> *));
BNM_BINDING_GENERIC_PROPERTY(Current, (PlayerController *));            // GetCurrent() / SetCurrent()
BNM_BINDING_GENERIC_STATIC_PROPERTY(Instance, (PlayerController *));
```

---

## 7. Generating Bindings Automatically

Writing these macros by hand for a whole game is impractical. The repository
ships `BNM_GameBinding_Generator.py` at its root: it reads a Cpp2IL /
Il2CppDumper `dump.cs` and emits one header per game type (plus an aggregate
`GameBinding.hpp`), using exactly the macros documented above — including the
BNM-native mappings for delegates, `IEnumerator`, Mono structures and Unity
types.

```bash
python BNM_GameBinding_Generator.py dump.cs ./generated
```

See **[Game Binding Generator](game-binding-generator.md)** for the full guide.
