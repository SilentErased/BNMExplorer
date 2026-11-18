#pragma once

#include <string_view>
#include "Behaviour.hpp"

namespace BNM::UnityEngine {
    /**
        @brief UnityEngine.MonoBehaviour implementation.
        MonoBehaviour is the base class from which every Unity script derives.
    */
    struct MonoBehaviour : public Behaviour {
        constexpr MonoBehaviour() : Behaviour() {}

#if UNITY_VER >= 222
        /**
            @brief CancellationTokenSource for Unity 2022.2+.
        */
        void *m_CancellationTokenSource{};
#endif

        // --- Coroutines ---

        /**
            @brief Starts a Coroutine using an IEnumerator routine object.
            @param routine IEnumerator object instance.
            @return Coroutine reference pointer, or nullptr on failure.
        */
        inline IL2CPP::Il2CppObject *StartCoroutine(IL2CPP::Il2CppObject *routine) {
            if (!IsValid() || !routine) return nullptr;
            static auto method = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("StartCoroutine"), 1).cast<IL2CPP::Il2CppObject *>();
            return method[(void *)this](routine);
        }

        /**
            @brief Starts a coroutine named methodName.
            @param methodName Name of the coroutine method as a Mono String pointer.
            @param value Optional parameter value to pass to the coroutine method.
            @return Coroutine reference pointer, or nullptr on failure.
        */
        inline IL2CPP::Il2CppObject *StartCoroutine(Structures::Mono::String *methodName, IL2CPP::Il2CppObject *value = nullptr) {
            if (!IsValid() || !methodName) return nullptr;
            if (value) {
                static auto method2 = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("StartCoroutine"), 2).cast<IL2CPP::Il2CppObject *>();
                return method2[(void *)this](methodName, value);
            }
            static auto method1 = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("StartCoroutine"), 1).cast<IL2CPP::Il2CppObject *>();
            return method1[(void *)this](methodName);
        }

        /**
            @brief Starts a coroutine named methodName using std::string_view.
            @param methodName Name of the coroutine method.
            @param value Optional parameter value.
            @return Coroutine reference pointer, or nullptr on failure.
        */
        inline IL2CPP::Il2CppObject *StartCoroutine(const std::string_view &methodName, IL2CPP::Il2CppObject *value = nullptr) {
            return StartCoroutine(CreateMonoString(methodName), value);
        }

        /**
            @brief Stops the first coroutine running the specified routine.
            @param routine Coroutine or IEnumerator object instance to stop.
        */
        inline void StopCoroutine(IL2CPP::Il2CppObject *routine) {
            if (!IsValid() || !routine) return;
            static auto method = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("StopCoroutine"), 1).cast<void>();
            method[(void *)this](routine);
        }

        /**
            @brief Stops all coroutines named methodName running on this behaviour.
            @param methodName Name of the coroutine method as a Mono String pointer.
        */
        inline void StopCoroutine(Structures::Mono::String *methodName) {
            if (!IsValid() || !methodName) return;
            static auto method = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("StopCoroutine"), 1).cast<void>();
            method[(void *)this](methodName);
        }

        /**
            @brief Stops all coroutines named methodName running on this behaviour using std::string_view.
            @param methodName Name of the coroutine method.
        */
        inline void StopCoroutine(const std::string_view &methodName) {
            StopCoroutine(CreateMonoString(methodName));
        }

        /**
            @brief Stops all coroutines running on this behaviour.
        */
        inline void StopAllCoroutines() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("StopAllCoroutines"), 0).cast<void>();
            method[(void *)this]();
        }

        // --- Invokes ---

        /**
            @brief Invokes the method methodName in time seconds.
            @param methodName Method name as a Mono String pointer.
            @param time Delay in seconds.
        */
        inline void Invoke(Structures::Mono::String *methodName, float time) {
            if (!IsValid() || !methodName) return;
            static auto method = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("Invoke"), 2).cast<void>();
            method[(void *)this](methodName, time);
        }

        /**
            @brief Invokes the method methodName in time seconds using std::string_view.
            @param methodName Method name.
            @param time Delay in seconds.
        */
        inline void Invoke(const std::string_view &methodName, float time) {
            Invoke(CreateMonoString(methodName), time);
        }

        /**
            @brief Invokes the method methodName in time seconds, then repeatedly every repeatRate seconds.
            @param methodName Method name as a Mono String pointer.
            @param time Initial delay in seconds.
            @param repeatRate Repeat interval in seconds.
        */
        inline void InvokeRepeating(Structures::Mono::String *methodName, float time, float repeatRate) {
            if (!IsValid() || !methodName) return;
            static auto method = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("InvokeRepeating"), 3).cast<void>();
            method[(void *)this](methodName, time, repeatRate);
        }

        /**
            @brief Invokes the method methodName in time seconds, then repeatedly every repeatRate seconds using std::string_view.
            @param methodName Method name.
            @param time Initial delay in seconds.
            @param repeatRate Repeat interval in seconds.
        */
        inline void InvokeRepeating(const std::string_view &methodName, float time, float repeatRate) {
            InvokeRepeating(CreateMonoString(methodName), time, repeatRate);
        }

        /**
            @brief Cancels all Invoke calls on this MonoBehaviour.
        */
        inline void CancelInvoke() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("CancelInvoke"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Cancels all Invoke calls with name methodName on this behaviour.
            @param methodName Method name as a Mono String pointer.
        */
        inline void CancelInvoke(Structures::Mono::String *methodName) {
            if (!IsValid() || !methodName) return;
            static auto method = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("CancelInvoke"), 1).cast<void>();
            method[(void *)this](methodName);
        }

        /**
            @brief Cancels all Invoke calls with name methodName on this behaviour using std::string_view.
            @param methodName Method name.
        */
        inline void CancelInvoke(const std::string_view &methodName) {
            CancelInvoke(CreateMonoString(methodName));
        }

        /**
            @brief Is any invoked method pending on this MonoBehaviour?
            @return True if any invoke is pending.
        */
        inline bool IsInvoking() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("IsInvoking"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Is any invoked method named methodName pending on this MonoBehaviour?
            @param methodName Method name as a Mono String pointer.
            @return True if invoke for methodName is pending.
        */
        inline bool IsInvoking(Structures::Mono::String *methodName) const {
            if (!IsValid() || !methodName) return false;
            static auto method = BNM::Defaults::Get<MonoBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("IsInvoking"), 1).cast<bool>();
            return method[(void *)this](methodName);
        }

        /**
            @brief Is any invoked method named methodName pending on this MonoBehaviour using std::string_view?
            @param methodName Method name.
            @return True if invoke for methodName is pending.
        */
        inline bool IsInvoking(const std::string_view &methodName) const {
            return IsInvoking(CreateMonoString(methodName));
        }
    };
}
