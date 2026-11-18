# Mono Structures (.NET Data Types)

BNM provides native C++20 implementations matching the internal memory layout of standard .NET / C# objects in IL2CPP.

---

## 1. C# Strings (`BNM::Structures::Mono::String`)

C# Strings in IL2CPP are UTF-16 encoded with a length header. BNM supports bidirectional conversions between `std::string` (UTF-8), `std::string_view`, and `Il2CppString`.

```cpp
#include <BNM/MonoStructures/String.hpp>
using namespace BNM::Structures::Mono;

// 1. Create C# String using helper
String *csharpStr = BNM::CreateMonoString("Hello from C++20!");

// 2. Read C# String to std::string
std::string utf8Text = csharpStr->str();

// 3. String length & indexing
int length = csharpStr->length;
char16_t firstChar = csharpStr->chars[0];

// 4. Comparison & formatting
bool equals = csharpStr->str() == "Expected";
```

---

## 2. C# Arrays (`BNM::Structures::Mono::Array<T>`)

1D C# Arrays in IL2CPP contain a `max_length` header and a contiguous buffer `m_Items`.

```cpp
#include <BNM/MonoStructures/Array.hpp>
using namespace BNM::Structures::Mono;

// 1. Allocate a new C# array via Class or Array::Create
auto intArray = BNM::Defaults::Get<int>().ToClass().NewArray<int>(5);
// or:
auto directArray = Array<int>::Create(5);

// 2. Read & Write elements
intArray->m_Items[0] = 42;
intArray->m_Items[1] = 1337;

// 3. Array bounds & conversion
size_t len = intArray->GetCapacity(); // or ->max_length
std::vector<int> cppVector = intArray->ToVector();
```

---

## 3. C# Lists (`BNM::Structures::Mono::List<T>`)

Representation of `System.Collections.Generic.List<T>`.

```cpp
#include <BNM/MonoStructures/List.hpp>
using namespace BNM::Structures::Mono;

// Create a new List<int>
auto list = BNM::Defaults::Get<int>().ToClass().NewList<int>();

// Add elements
list->Add(100);
list->Add(200);

// Access elements
int count = list->GetSize();
int item0 = list->GetItem(0);

// Convert to std::vector
std::vector<int> vec = list->ToVector();
```

---

## 4. Tombstone-Safe C# Dictionaries (`BNM::Structures::Mono::Dictionary<TKey, TValue>`)

Representation of `System.Collections.Generic.Dictionary<TKey, TValue>`.

> **Safety Note**: C# Dictionaries maintain a freelist with deleted entry tombstones (`hashCode = -1` or negative). BNM automatically filters active entries and returns the true active count (`count - freeCount`).

```cpp
#include <BNM/MonoStructures/Dictionary.hpp>
using namespace BNM::Structures::Mono;

Dictionary<String *, int> *playerScores = ...;

// Active item count (excludes deleted tombstones)
int activeEntries = playerScores->GetSize();

// Convert to std::map
std::map<String *, int> scoreMap = playerScores->ToMap();

// Extract keys and values
std::vector<String *> keys = playerScores->GetKeys();
std::vector<int> values = playerScores->GetValues();

// Access key
int score = 0;
if (playerScores->TryGetValue(BNM::CreateMonoString("Player1"), &score)) {
    BNM_LOG_INFO("Player1 score: %d", score);
}
```

---

## 5. Collections: `Queue<T>`, `Stack<T>`, `HashSet<T>`, `IReadOnlyList<T>`

Native wrappers for the remaining `System.Collections.Generic` types that games
expose in public APIs. Read operations are native (layout-accurate); mutations
that need C#-side invariants (`HashSet` resize/rehash) go through IL2CPP.

```cpp
#include <BNM/MonoStructures/Queue.hpp>
#include <BNM/MonoStructures/Stack.hpp>
#include <BNM/MonoStructures/HashSet.hpp>
#include <BNM/MonoStructures/ReadOnlyList.hpp>
using namespace BNM::Structures::Mono;

// Queue<T> — native Enqueue/Dequeue/Peek/GetCount/ToVector/ToArray/Contains/Clear
Queue<int> *queue = ...;
queue->Enqueue(10);
int front = queue->Peek();
int item = queue->Dequeue();
std::vector<int> qv = queue->ToVector();

// Stack<T> — native Push/Pop/Peek/GetCount/ToVector/ToArray/Contains/Clear
Stack<int> *stack = ...;
stack->Push(42);
int top = stack->Peek();
int popped = stack->Pop();

// HashSet<T> — native GetCount/Contains/ToVector/ToArray;
// Add/Remove/Clear/UnionWith/IntersectWith/ExceptWith via IL2CPP
HashSet<String *> *set = ...;
bool has = set->Contains(BNM::CreateMonoString("Key"));
set->Add(BNM::CreateMonoString("NewKey"));
std::vector<String *> items = set->ToVector();

// IReadOnlyList<T> / IReadOnlyCollection<T> — reflection view (get_Count/get_Item)
IReadOnlyList<String *> *roList = ...;
int count = roList->GetCount();
String *first = roList->GetItem(0);   // or roList[0]
std::vector<String *> all = roList->ToVector();
```

---

## 6. Extended .NET Data Types

BNM provides native wrappers for standard .NET value and reference types:

```cpp
#include <BNM/MonoStructures/DateTime.hpp>
#include <BNM/MonoStructures/TimeSpan.hpp>
#include <BNM/MonoStructures/Guid.hpp>
#include <BNM/MonoStructures/Nullable.hpp>
#include <BNM/MonoStructures/KeyValuePair.hpp>
using namespace BNM::Structures::Mono;

// DateTime
DateTime now = DateTime::Now();
int64_t ticks = now.Ticks();
int hour = now.Hour();          // also Second() / Minute() / Kind()
DateTime later = now + TimeSpan::FromHours(2);

// TimeSpan
TimeSpan fiveMinutes = TimeSpan::FromMinutes(5.0);
double totalSec = fiveMinutes.TotalSeconds();   // also TotalMinutes() / TotalHours() / ...

// Guid
Guid empty = Guid::Empty();
bool isEmpty = empty.IsEmpty();
std::string guidStr = empty.ToString();

// Nullable<T>
Nullable<int> optInt(42);
if (optInt.HasValue()) {
    int val = optInt.GetValue();
}

// KeyValuePair<K, V>
KeyValuePair<String *, int> pair(BNM::CreateMonoString("Key"), 100);
```

---

## 7. Delegates, MulticastDelegates & Actions

BNM provides a rich delegate factory suite that converts native C++ lambdas, free functions, and member functions into GC-managed IL2CPP delegates.

```cpp
#include <BNM/Delegates.hpp>
```

### A. Creating `System.Action` (Void Callbacks)
```cpp
// 1. Parameterless Action
auto action = BNM::CreateAction([]() {
    BNM_LOG_INFO("Action callback fired!");
});

// 2. Parameterized Action<float, int> (types auto-deduced from lambda)
auto updateAction = BNM::CreateAction([](float speed, int score) {
    BNM_LOG_INFO("Speed: %.2f, Score: %d", speed, score);
});

// 3. Binding to C++ Member Functions
struct PlayerHandler {
    void OnSpeedChanged(float newSpeed) {
        BNM_LOG_INFO("Player speed updated: %.2f", newSpeed);
    }
};

PlayerHandler myHandler;
auto memberAction = BNM::CreateAction(&myHandler, &PlayerHandler::OnSpeedChanged);
```

### B. Creating `System.Func` (Value Returning Callbacks)
```cpp
// 1. Func<int, int, bool>
auto isGreater = BNM::CreateFunc([](int a, int b) -> bool {
    return a > b;
});

// 2. Invoking Func directly from C++
bool result = isGreater->Invoke(10, 5); // Returns true
```

### C. Creating `System.Predicate<T>`, `Comparison<T>`, & `EventHandler<T>`
```cpp
// Predicate<int>
auto predicate = BNM::CreatePredicate<int>([](int val) {
    return val % 2 == 0;
});

// Comparison<int>
auto comparison = BNM::CreateComparison<int>([](int a, int b) {
    return a - b;
});

// EventHandler<CustomEventArgs *>
auto eventHandler = BNM::CreateEventHandler<IL2CPP::Il2CppObject *>([](IL2CPP::Il2CppObject *sender, IL2CPP::Il2CppObject *e) {
    BNM_LOG_INFO("Event handler triggered by sender: %p", sender);
});
```

### D. MulticastDelegate Chaining
```cpp
BNM::MulticastDelegate<void> *multiDel = ...;

// Add delegate using operator+= or Add()
multiDel->Add(action);
*multiDel += updateAction;

// Remove delegate
multiDel->Remove(action);
*multiDel -= updateAction;

// Invoke multicast chain
multiDel->Invoke();
```

### E. Custom / Game Delegate Creation (`CreateDelegate`)
```cpp
BNM::Class customDelegateClass("Game.Events", "OnPlayerHitDelegate");

// Creates a delegate instance matching the custom delegate type
auto hitDelegate = BNM::CreateDelegate<void, int, bool>(customDelegateClass, [](int damage, bool crit) {
    BNM_LOG_INFO("Player hit delegate: damage=%d, crit=%d", damage, crit);
});
```
