# Game Binding Generator (`BNM_GameBinding_Generator.py`)

`BNM_GameBinding_Generator.py` (repository root) converts a Cpp2IL / Il2CppDumper
`dump.cs` into strongly-typed C++20 binding headers (`GB::...`) that plug into the
macro layer of `include/BNM/GameBinding.hpp` (see [Static Game Bindings](game-binding.md)).

You write bindings by hand with `BNM_GAME_CLASS`/`BNM_BINDING_*`; this generator
does it for the whole game at once, so mods get compile-time typed access to every
class, field, property, method, enum, and interface in the dump.

---

## 1. Quick start

```bash
# Generate into GameBinding/GameBinding/
python BNM_GameBinding_Generator.py dump.cs

# Or choose an output directory
python BNM_GameBinding_Generator.py dump.cs ./generated
```

Then in your mod project:

```cpp
#include <GameBinding.hpp>   // aggregate header (all bindings)
// or a single class header:
#include "ExampleClient.hpp"
```

`#include <BNM/GameBinding.hpp>` is pulled in automatically by every generated
header; BNM native dependencies (`BNM/Delegates.hpp`, `BNM/Coroutine.hpp`,
`BNM/MonoStructures/*.hpp`, `BNM/UnityStructures/*.hpp`) are added per-file only
when a binding actually uses them.

### Requirements

- A `dump.cs` produced by **Cpp2IL** or **Il2CppDumper** with `// Image N:` headers
  and `// TypeDefIndex:` markers (the format both tools emit by default).
- Python 3.6+ (standard library only — no external packages).

---

## 2. Command line

```
python BNM_GameBinding_Generator.py <path_to_dump.cs> [output_directory]
```

| Argument | Required | Default | Description |
| :--- | :---: | :--- | :--- |
| `path_to_dump.cs` | yes | — | Path to the `dump.cs` file. |
| `output_directory` | no | `GameBinding` | Headers go to `<output_directory>/GameBinding/`. |

Output layout mirrors C# namespaces:

```
GameBinding/
├─ GameBinding.hpp                 <- aggregate: includes every generated header
├─ ExampleClient.hpp               <- global-namespace classes at the root
├─ Game/
│  └─ Common/
│     └─ OutcomeCode.hpp
└─ System/
   └─ Collections/
      └─ IEnumerable.hpp
```

---

## 3. What gets generated

| Dump C# construct | Generated C++ |
| :--- | :--- |
| class / struct | `BNM_GAME_CLASS` struct (`GB::Namespace::Name`) |
| C# interface | `BNM_GAME_INTERFACE` struct (usable with `.As<T>()` / `.IsA<T>()`) |
| nested class `Outer.Inner` | flattened struct `Outer_Inner.hpp` |
| enum | `enum class : underlying` (e.g. `GB::GamePanel_widgetType`) |
| generic class `T<T1,...>` | `BNM_GAME_GENERIC_CLASS_1/2/3` when safe, `_EX` otherwise |
| field / property | `BNM_BINDING_[STATIC_|GENERIC_]FIELD/PROPERTY` |
| method | `BNM_BINDING_[STATIC_]METHOD` (resolved by parameter count) |
| overloaded method | emitted per C# signature; duplicates C++ cannot distinguish (same name + parameter types, or return-type-only differences) are dropped |
| generic method | `BNM_BINDING_[STATIC_]GENERIC_METHOD_1/2/3` or `_EX` |

### Mapped to BNM native types (not generated)

| Dump C# construct | Mapped to |
| :--- | :--- |
| `System.Action[<...>]` | `BNM::Structures::Mono::Action<...> *` |
| `System.Func<..., R>` / `System.Converter<A, B>` | `BNM::Delegate<R> *` / `BNM::Delegate<B> *` |
| `System.Predicate<T>` / `System.Comparison<T>` | `BNM::Delegate<bool> *` / `BNM::Delegate<int> *` |
| `System.EventHandler`, `System.Delegate`, `System.AsyncCallback` | `BNM::Delegate<void> *` |
| `System.MulticastDelegate` | `BNM::MulticastDelegate<void> *` |
| `System.Collections.IEnumerator` | `BNM::Coroutine::IEnumerator *` |
| `System.Collections.Generic.IEnumerator<T>` | `BNM::Coroutine::Generic::IEnumerator<T> *` |
| `HashSet<T>` / `Queue<T>` / `Stack<T>` | `BNM::Structures::Mono::HashSet/Queue/Stack<T> *` |
| `IReadOnlyList<T>` / `IReadOnlyCollection<T>` | `BNM::Structures::Mono::IReadOnlyList/IReadOnlyCollection<T> *` |
| `string` / `T[]` / `List<T>` / `Dictionary<K, V>` | `BNM::Structures::Mono::{String, Array, List, Dictionary}` |
| Unity engine types (`GameObject`, `MonoBehaviour`, ...) | `BNM::UnityEngine::...` |

### Skipped entirely

- Compiler-generated C# types: state machines (`<Method>d__N`), closures
  (`<>c`, `<>c__DisplayClass`), anonymous types, `<Module>`,
  `<PrivateImplementationDetails>` — they are never referenced by real types.
- BCL delegate families, the two `IEnumerator` interfaces, and the
  `HashSet/Queue/Stack/IReadOnlyList/IReadOnlyCollection` collections listed
  above (their files are not emitted; references map to BNM native types).
- BNM-native Unity types (already provided by `include/BNM/UnityStructures/`).
- Explicit interface implementations keep a C++ identifier (`NameSpace_Method`)
  with the original name passed to the runtime lookup; C# names colliding with
  environment macros (glibc/CRT) are emitted through `#pragma push_macro` guards.

---

## 4. How name resolution works

- **Namespaces** are preserved (`GB` mirrors the C# namespace tree).
- **Colliding simple names** (same class name in different namespaces) get a
  numeric suffix (`SharedState_0`, `SharedState_1`, ...)
  so C++ stays valid; the original C# name is still used for the runtime lookup.
- **Forward declarations**: pointer-only dependencies are forward-declared in
  the header (one `struct X;` per referenced type), full `#include`s only when a
  field/property/base needs the complete definition. This breaks include
  cycles (e.g. mutually referencing classes, outer/inner nesting).
- **Nested types** are referenced through an alias registry so `Outer.Inner`
  resolves to the flattened `Outer_Inner` struct.

```cpp
// Example of a generated header (System/Collections/Generic/IEnumerable.hpp):
#pragma once

#include <BNM/GameBinding.hpp>
#include <BNM/Coroutine.hpp>

// C# Namespace: System.Collections.Generic
// Assembly: mscorlib.dll

namespace GB {
namespace System {
namespace Collections {
namespace Generic {
BNM_GAME_GENERIC_CLASS_1("System.Collections.Generic", IEnumerable, (BNM::IL2CPP::Il2CppObject), T, "mscorlib.dll")

    // --- Methods ---
    BNM_BINDING_METHOD(GetEnumerator, BNM::Coroutine::Generic::IEnumerator<T> *, 0, (), ());

BNM_GAME_GENERIC_CLASS_END

} // namespace Generic
} // namespace Collections
} // namespace System
} // namespace GB
```

---

## 5. Tips & limitations

- **Regenerate fully**: delete the previous output directory before regenerating
  so removed/skipped types don't leave stale headers behind.
- **Type names collide at C++ level by design of C#**: the generator prefers the
  current namespace, then `GB`, then the first candidate; check the generated
  `// Assembly:` / `// Forward declarations` comments when in doubt.
- Generated output is **not committed** (large); the aggregate
  `GameBinding.hpp` alone pulls in every header.
- The generator is a build-time helper: BNM itself never includes its output.
- `BNM_COROUTINE` / `BNM_CLASSES_MANAGEMENT` (both default-on) must stay enabled
  when bindings reference `BNM::Coroutine::*` types.

---

**See also:** [Static Game Bindings](game-binding.md) (macro reference) ·
[Getting Started](getting-started.md) · script at repository root
`BNM_GameBinding_Generator.py`.
