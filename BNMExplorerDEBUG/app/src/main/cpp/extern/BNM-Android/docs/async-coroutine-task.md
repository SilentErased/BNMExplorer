# Asynchronous, Coroutines & Tasks

BNM provides native C++20 support for Unity Coroutines (`IEnumerator`) and asynchronous C# Tasks (`System.Threading.Tasks.Task<T>` and `UniTask`) using standard C++20 `co_await` syntax.

---

## 1. Unity Coroutines (`BNM::Coroutine::IEnumerator`)

BNM allows you to write custom coroutines in C++ using C++20 `co_yield` that are pumped directly by Unity Engine's coroutine scheduler.

### A. Authoring Coroutine Functions
```cpp
#include <BNM/Coroutine.hpp>

// Define a C++20 coroutine returning BNM::Coroutine::IEnumerator
BNM::Coroutine::IEnumerator MyModCoroutine() {
    BNM_LOG_INFO("Coroutine Step 1: Starting");

    // 1. Wait for 2.0 seconds in Unity Engine
    co_yield BNM::Coroutine::WaitForSeconds(2.0f);

    BNM_LOG_INFO("Coroutine Step 2: 2 seconds elapsed");

    // 2. Wait for real-time seconds (ignores Time.timeScale)
    co_yield BNM::Coroutine::WaitForSecondsRealtime(1.0f);

    // 3. Wait for physics FixedUpdate step
    co_yield BNM::Coroutine::WaitForFixedUpdate();

    // 4. Wait until condition returns true
    co_yield BNM::Coroutine::WaitUntil([]() -> bool {
        return true; // Return custom condition
    });

    // 5. Wait while condition remains true
    co_yield BNM::Coroutine::WaitWhile([]() -> bool {
        return false;
    });

    // 6. Wait until end of rendered frame
    co_yield BNM::Coroutine::WaitForEndOfFrame();

    BNM_LOG_INFO("Coroutine Step 3: Finished!");
    co_return;
}
```

### B. Starting Coroutines on a `MonoBehaviour`
To pass the coroutine to Unity's `MonoBehaviour.StartCoroutine`, obtain the IL2CPP `IEnumerator *` object using `.Get()`, the call operator `()`, or implicit pointer conversion:

```cpp
void RunCoroutine(BNM::UnityEngine::MonoBehaviour *behaviour) {
    // 1. Using .Get()
    auto enumerator = MyModCoroutine().Get();
    behaviour->StartCoroutine(enumerator);

    // 2. Or using reflection StartCoroutine
    auto startCoroutineMethod = behaviour->GetClass().GetMethod("StartCoroutine", 1);
    startCoroutineMethod[behaviour](MyModCoroutine().Get());
}
```

### C. Typed `IEnumerator<T>` view
C# `System.Collections.Generic.IEnumerator<T>` maps to `BNM::Coroutine::Generic::IEnumerator<T>` - a typed view that derives from `BNM::Coroutine::IEnumerator` and adds no state of its own (the `T` is a compile-time tag):

```cpp
// Generated game bindings use it for IEnumerator<T> returns/parameters:
BNM::Coroutine::Generic::IEnumerator<BNM::IL2CPP::Il2CppObject> *e = gameObject->GetEnumerator();
behaviour->StartCoroutine(e);   // any Coroutine::* pointer is a valid IL2CPP IEnumerator
```

---

## 2. C++20 Task Bridge (`BNM::Task<T>`)

BNM provides a C++20 awaitable wrapper for `System.Threading.Tasks.Task<T>` and `System.Threading.Tasks.Task` (void) that supports native `co_await`.

### A. Using `co_await` with C# Tasks
```cpp
#include <BNM/Task.hpp>

// C++20 Coroutine awaiting a C# async Task
BNM::Task<void> DownloadPlayerDataAsync(int playerId) {
    // Call C# async method returning Task<PlayerData>
    auto getPlayerTaskObj = BNM::Class("Game.Net", "NetworkManager")
        .GetMethod("FetchPlayerAsync", 1)
        .cast<BNM::IL2CPP::Il2CppObject *>()(playerId);

    // Wrap into BNM::Task<PlayerData *>
    BNM::Task<BNM::IL2CPP::Il2CppObject *> task(getPlayerTaskObj);

    // Asynchronously await completion without blocking the main thread
    BNM::IL2CPP::Il2CppObject *playerData = co_await task;

    BNM_LOG_INFO("Player data received asynchronously: %p", playerData);
}
```

### B. Polling & Status Inspection
```cpp
BNM::Task<int> task = ...;

// Check task status non-blocking
if (task.IsCompleted()) {
    if (task.IsCompletedSuccessfully()) {
        int result = task.GetResult();
        BNM_LOG_INFO("Task result: %d", result);
    } else if (task.IsFaulted()) {
        auto exc = task.GetException();
        BNM_LOG_ERR("Task failed with exception: %p", exc);
    } else if (task.IsCanceled()) {
        BNM_LOG_WARN("Task was canceled.");
    }
}

// Blocking wait if necessary
task.Wait();
```

---

## 3. AsyncOperation Wrapper

```cpp
#include <BNM/UnityStructures/AsyncOperation.hpp>
#include <BNM/UnityStructures/SceneManager.hpp>

// Load scene asynchronously
BNM::UnityEngine::AsyncOperation *asyncOp = BNM::UnityEngine::SceneManagement::SceneManager::LoadSceneAsync("GameLevel1");

// Inspect progress
float progress = asyncOp->GetProgress(); // 0.0f - 1.0f
bool isDone = asyncOp->GetIsDone();
```
