#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Class.hpp"
#include "Method.hpp"
#include "Property.hpp"
#include "Utils.hpp"
#include "Defaults.hpp"
#include "Exceptions.hpp"

#if __has_include(<coroutine>)
#include <coroutine>
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace std {
    template<typename T = void>
    using coroutine_handle = std::experimental::coroutine_handle<T>;
    using suspend_always = std::experimental::suspend_always;
    using suspend_never = std::experimental::suspend_never;
}
#endif

namespace BNM {

    template<typename T = void>
    struct Task;

    /// @cond
    namespace Detail {
        inline BNM::Class GetSystemTaskClass() {
            static auto cls = BNM::Class(BNM_OBFUSCATE("System.Threading.Tasks"), BNM_OBFUSCATE("Task"));
            return cls;
        }

        inline BNM::Class GetSystemGenericTaskClass() {
            static auto cls = BNM::Class(BNM_OBFUSCATE("System.Threading.Tasks"), BNM_OBFUSCATE("Task`1"));
            return cls;
        }

        template<typename Type, typename = void>
        struct TaskTypeResolver {
            static inline BNM::CompileTimeClass Get() {
                return Defaults::Get<Type>();
            }
        };

        template<typename Type>
        struct TaskTypeResolver<Type, std::void_t<decltype(Type::BNMCustomClass)>> {
            static inline BNM::CompileTimeClass Get() {
                return Type::BNMCustomClass._targetType;
            }
        };
    }
    /// @endcond

    /**
        @brief C++20 Coroutine Task wrapper for C# System.Threading.Tasks.Task<T>.
        Allows seamless co_await, result polling, and execution of asynchronous operations.
        @tparam T Return type of the Task.
    */
    template<typename T>
    struct Task {
        IL2CPP::Il2CppObject *_instance = nullptr;

        constexpr Task() = default;
        constexpr Task(IL2CPP::Il2CppObject *instance) : _instance(instance) {}
        constexpr Task(std::nullptr_t) : _instance(nullptr) {}

        /**
            @brief Check if task object pointer is valid and non-null.
        */
        [[nodiscard]] inline bool IsValid() const noexcept {
            return _instance != nullptr && CheckForNull((void *)_instance);
        }

        [[nodiscard]] inline explicit operator bool() const noexcept { return IsValid(); }
        inline operator IL2CPP::Il2CppObject *() const noexcept { return _instance; }

        /**
            @brief Returns the static IL2CPP generic Task<T> class representation.
        */
        static inline BNM::Class StaticClass() {
            static auto genericClass = []() -> BNM::Class {
                auto baseGeneric = Detail::GetSystemGenericTaskClass();
                if constexpr (std::is_void_v<T>) {
                    return Detail::GetSystemTaskClass();
                } else {
                    return baseGeneric.GetGeneric({ Detail::TaskTypeResolver<T>::Get() });
                }
            }();
            return genericClass;
        }

        /**
            @brief Gets whether the task has completed (successfully, faulted, or canceled).
        */
        [[nodiscard]] inline bool IsCompleted() const {
            if (!IsValid()) return false;
            static auto prop = Detail::GetSystemTaskClass().GetProperty(BNM_OBFUSCATE("IsCompleted")).template cast<bool>();
            return prop[(void *)_instance].Get();
        }

        /**
            @brief Gets whether the task completed due to an unhandled exception.
        */
        [[nodiscard]] inline bool IsFaulted() const {
            if (!IsValid()) return false;
            static auto prop = Detail::GetSystemTaskClass().GetProperty(BNM_OBFUSCATE("IsFaulted")).template cast<bool>();
            return prop[(void *)_instance].Get();
        }

        /**
            @brief Gets whether the task was canceled.
        */
        [[nodiscard]] inline bool IsCanceled() const {
            if (!IsValid()) return false;
            static auto prop = Detail::GetSystemTaskClass().GetProperty(BNM_OBFUSCATE("IsCanceled")).template cast<bool>();
            return prop[(void *)_instance].Get();
        }

        /**
            @brief Gets whether the task completed successfully without being canceled or faulting.
        */
        [[nodiscard]] inline bool IsCompletedSuccessfully() const {
            return IsCompleted() && !IsFaulted() && !IsCanceled();
        }

        /**
            @brief Gets the AggregateException that caused the task to end prematurely, or nullptr.
        */
        [[nodiscard]] inline IL2CPP::Il2CppException *GetException() const {
            if (!IsValid()) return nullptr;
            static auto prop = Detail::GetSystemTaskClass().GetProperty(BNM_OBFUSCATE("Exception")).template cast<IL2CPP::Il2CppException *>();
            return prop[(void *)_instance].Get();
        }

        /**
            @brief Synchronously blocks the calling thread until the task has completed.
        */
        inline void Wait() const {
            if (!IsValid()) return;
            static auto method = Detail::GetSystemTaskClass().GetMethod(BNM_OBFUSCATE("Wait"), 0).template cast<void>();
            method[(void *)_instance]();
        }

        /**
            @brief Gets the result value of this Task<T> (blocks until completion if needed).
        */
        [[nodiscard]] inline T GetResult() const {
            if (!IsValid()) return T{};
            static auto prop = StaticClass().GetProperty(BNM_OBFUSCATE("Result")).template cast<T>();
            return prop[(void *)_instance].Get();
        }

        /**
            @brief Alias for GetResult().
        */
        [[nodiscard]] inline T Result() const { return GetResult(); }

        // --- C++20 Coroutine Awaitable Interface ---

        [[nodiscard]] inline bool await_ready() const noexcept {
            return IsCompleted();
        }

        inline void await_suspend(std::coroutine_handle<> handle) const {
            if (await_ready()) {
                handle.resume();
                return;
            }

            // Yield / spin-wait until task completes
            while (!IsCompleted()) {
                #if defined(__aarch64__) || defined(__ARM_ARCH_7A__)
                __asm__ volatile("yield");
                #elif defined(__i386__) || defined(__x86_64__)
                __asm__ volatile("pause");
                #endif
            }
            handle.resume();
        }

        inline T await_resume() const {
            if (IsFaulted()) {
                auto exc = GetException();
                if (exc) BNM_LOG_ERR("[BNM::Task] Awaited task faulted with exception: %p", exc);
            }
            return GetResult();
        }

        template<typename Target>
        inline Target *As() const {
            if (!IsValid()) return nullptr;
            return (Target *)_instance;
        }

        template<typename Target>
        inline bool IsA() const {
            if (!IsValid()) return false;
            return BNM::IsA(_instance, Target::StaticClass());
        }
    };

    /**
        @brief Specialization of Task for void operations (System.Threading.Tasks.Task).
    */
    template<>
    struct Task<void> {
        IL2CPP::Il2CppObject *_instance = nullptr;

        constexpr Task() = default;
        constexpr Task(IL2CPP::Il2CppObject *instance) : _instance(instance) {}
        constexpr Task(std::nullptr_t) : _instance(nullptr) {}

        [[nodiscard]] inline bool IsValid() const noexcept {
            return _instance != nullptr && CheckForNull((void *)_instance);
        }

        [[nodiscard]] inline explicit operator bool() const noexcept { return IsValid(); }
        inline operator IL2CPP::Il2CppObject *() const noexcept { return _instance; }

        static inline BNM::Class StaticClass() {
            return Detail::GetSystemTaskClass();
        }

        [[nodiscard]] inline bool IsCompleted() const {
            if (!IsValid()) return false;
            static auto prop = Detail::GetSystemTaskClass().GetProperty(BNM_OBFUSCATE("IsCompleted")).template cast<bool>();
            return prop[(void *)_instance].Get();
        }

        [[nodiscard]] inline bool IsFaulted() const {
            if (!IsValid()) return false;
            static auto prop = Detail::GetSystemTaskClass().GetProperty(BNM_OBFUSCATE("IsFaulted")).template cast<bool>();
            return prop[(void *)_instance].Get();
        }

        [[nodiscard]] inline bool IsCanceled() const {
            if (!IsValid()) return false;
            static auto prop = Detail::GetSystemTaskClass().GetProperty(BNM_OBFUSCATE("IsCanceled")).template cast<bool>();
            return prop[(void *)_instance].Get();
        }

        [[nodiscard]] inline bool IsCompletedSuccessfully() const {
            return IsCompleted() && !IsFaulted() && !IsCanceled();
        }

        [[nodiscard]] inline IL2CPP::Il2CppException *GetException() const {
            if (!IsValid()) return nullptr;
            static auto prop = Detail::GetSystemTaskClass().GetProperty(BNM_OBFUSCATE("Exception")).template cast<IL2CPP::Il2CppException *>();
            return prop[(void *)_instance].Get();
        }

        inline void Wait() const {
            if (!IsValid()) return;
            static auto method = Detail::GetSystemTaskClass().GetMethod(BNM_OBFUSCATE("Wait"), 0).template cast<void>();
            method[(void *)_instance]();
        }

        // --- C++20 Coroutine Awaitable Interface ---

        [[nodiscard]] inline bool await_ready() const noexcept {
            return IsCompleted();
        }

        inline void await_suspend(std::coroutine_handle<> handle) const {
            if (await_ready()) {
                handle.resume();
                return;
            }

            while (!IsCompleted()) {
                #if defined(__aarch64__) || defined(__ARM_ARCH_7A__)
                __asm__ volatile("yield");
                #elif defined(__i386__) || defined(__x86_64__)
                __asm__ volatile("pause");
                #endif
            }
            handle.resume();
        }

        inline void await_resume() const {
            if (IsFaulted()) {
                auto exc = GetException();
                if (exc) BNM_LOG_ERR("[BNM::Task] Awaited task faulted with exception: %p", exc);
            }
        }

        template<typename Target>
        inline Target *As() const {
            if (!IsValid()) return nullptr;
            return (Target *)_instance;
        }

        template<typename Target>
        inline bool IsA() const {
            if (!IsValid()) return false;
            return BNM::IsA(_instance, Target::StaticClass());
        }
    };

}
