#pragma once

#include <utility>
#include <tuple>
#include <type_traits>

#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../Delegates.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Event.hpp"

namespace BNM::UnityEngine {
    struct Object;

    /**
        @brief UnityEngine.Events.UnityAction implementation.
        A zero or multi-argument delegate used by UnityEvents.
        @tparam Parameters Argument types accepted by the action delegate.
    */
    template <typename ...Parameters>
    struct UnityAction : public MulticastDelegate<void> {
        /**
            @brief Invokes the UnityAction delegate with the given parameters.
            @param parameters Arguments to pass to the delegate invocation.
        */
        inline void Invoke(Parameters ...parameters) { (MulticastDelegate<void>(*this)).Invoke(parameters...); }
    };

    /// @cond
    namespace PRIVATE_UNITY_EVENTS {
        template<typename Tuple, typename F, size_t... Is>
        auto CreateUnityActionFromTuple(F &&callable, std::index_sequence<Is...>) {
            auto cls = PRIVATE_DELEGATES::GetUnityActionClass(sizeof...(Is));
            if constexpr (sizeof...(Is) > 0) {
                cls = cls.GetGeneric({BNM::Defaults::Get<std::tuple_element_t<Is, Tuple>>()...});
            }
            return (UnityAction<std::tuple_element_t<Is, Tuple>...> *) PRIVATE_DELEGATES::CreateDelegateFromCallable<void, std::tuple_element_t<Is, Tuple>...>(cls, std::forward<F>(callable));
        }
    }
    /// @endcond

    /**
        @brief Creates a UnityEngine.Events.UnityAction delegate instance with explicit parameter types.
        @tparam FirstArg First argument type.
        @tparam OtherArgs Remaining parameter types.
        @param callable Functor, lambda, or callable object to invoke.
        @return Pointer to newly created UnityAction delegate.
    */
    template<typename FirstArg, typename ...OtherArgs, typename F>
    inline UnityAction<FirstArg, OtherArgs...> *CreateUnityAction(F &&callable) {
        auto cls = PRIVATE_DELEGATES::GetUnityActionClass(sizeof...(OtherArgs) + 1);
        cls = cls.GetGeneric({BNM::Defaults::Get<FirstArg>(), BNM::Defaults::Get<OtherArgs>()...});
        return (UnityAction<FirstArg, OtherArgs...> *) PRIVATE_DELEGATES::CreateDelegateFromCallable<void, FirstArg, OtherArgs...>(cls, std::forward<F>(callable));
    }

    /**
        @brief Creates a UnityEngine.Events.UnityAction delegate instance by binding a member function on an object instance.
        @param instance Pointer to class instance.
        @param method Pointer to member function.
        @return Pointer to newly created UnityAction delegate.
    */
    template<typename InstanceT, typename ClassT, typename ...Args>
    inline auto CreateUnityAction(InstanceT *instance, void (ClassT::*method)(Args...)) {
        using ArgsTuple = std::tuple<Args...>;
        return PRIVATE_UNITY_EVENTS::CreateUnityActionFromTuple<ArgsTuple>(
            [instance, method](Args ...args) { (instance->*method)(args...); },
            std::make_index_sequence<sizeof...(Args)>{}
        );
    }

    /**
        @brief Creates a UnityEngine.Events.UnityAction delegate instance by binding a const member function on an object instance.
        @param instance Pointer to class instance.
        @param method Pointer to const member function.
        @return Pointer to newly created UnityAction delegate.
    */
    template<typename InstanceT, typename ClassT, typename ...Args>
    inline auto CreateUnityAction(InstanceT *instance, void (ClassT::*method)(Args...) const) {
        using ArgsTuple = std::tuple<Args...>;
        return PRIVATE_UNITY_EVENTS::CreateUnityActionFromTuple<ArgsTuple>(
            [instance, method](Args ...args) { (instance->*method)(args...); },
            std::make_index_sequence<sizeof...(Args)>{}
        );
    }

    /**
        @brief Creates a UnityEngine.Events.UnityAction delegate instance with parameter types deduced automatically from the lambda.
        @param callable Lambda or functor with auto-deducible parameters.
        @return Pointer to newly created UnityAction delegate.
    */
    template<typename F, typename = std::enable_if_t<!std::is_member_function_pointer_v<std::decay_t<F>>>>
    inline auto CreateUnityAction(F &&callable) {
        using Traits = PRIVATE_DELEGATES::function_traits<std::decay_t<F>>;
        using ArgsTuple = typename Traits::args_tuple;
        return PRIVATE_UNITY_EVENTS::CreateUnityActionFromTuple<ArgsTuple>(std::forward<F>(callable), std::make_index_sequence<Traits::arity>{});
    }

    /**
        @brief UnityEngine.Events.ArgumentCache implementation.
        Stores cached serialized arguments for persistent UnityEvent calls in inspector/scene data.
    */
    struct ArgumentCache : public BNM::IL2CPP::Il2CppObject {
        constexpr ArgumentCache() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityEngine::Object *m_ObjectArgument{};
        Structures::Mono::String *m_ObjectArgumentAssemblyTypeName{};
        int m_IntArgument{};
        float m_FloatArgument{};
        Structures::Mono::String *m_StringArgument{};
        bool m_BoolArgument{};
    };

    /**
        @brief UnityEngine.Events.PersistentListenerMode implementation.
        Enum denoting the type of argument mode configured for a persistent call.
    */
    enum class PersistentListenerMode : int {
        EventDefined = 0, Void = 1, Object = 2, Int = 3, Float = 4, String = 5, Bool = 6
    };

    /**
        @brief UnityEngine.Events.UnityEventCallState implementation.
        Controls whether persistent listeners are executed in runtime, editor, or off.
    */
    enum class UnityEventCallState : int {
        Off = 0, EditorAndRuntime = 1, RuntimeOnly = 2
    };

    /**
        @brief UnityEngine.Events.PersistentCall implementation.
        Represents a single persistent listener registered in the Unity scene/prefab.
    */
    struct PersistentCall : public BNM::IL2CPP::Il2CppObject {
        constexpr PersistentCall() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityEngine::Object *m_Target{};
        Structures::Mono::String *m_TargetAssemblyTypeName{};
        Structures::Mono::String *m_MethodName{};
        PersistentListenerMode m_Mode{};
        ArgumentCache *m_Arguments{};
        UnityEventCallState m_CallState{};

        /**
            @brief Checks if the persistent call has valid target assembly type and method names.
            @return True if call is valid for invocation.
        */
        [[nodiscard]] inline bool IsValid() const { return m_TargetAssemblyTypeName && m_TargetAssemblyTypeName->length && m_MethodName && m_MethodName->length; }
    };

    /**
        @brief UnityEngine.Events.PersistentCallGroup implementation.
        Holds a list of PersistentCall objects.
    */
    struct PersistentCallGroup : public BNM::IL2CPP::Il2CppObject {
        constexpr PersistentCallGroup() : BNM::IL2CPP::Il2CppObject({}) {}
        Structures::Mono::List<PersistentCall *> *m_Calls{};
    };

    /**
        @brief UnityEngine.Events.InvokableCallBase implementation.
        Base class for invokable runtime/persistent listener delegates.
    */
    struct InvokableCallBase : public BNM::IL2CPP::Il2CppObject {
        constexpr InvokableCallBase() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityAction<> *action{};
    };

    /**
        @brief UnityEngine.Events.InvokableCall implementation.
        Generic container holding a strongly-typed UnityAction delegate.
        @tparam Parameters Argument types accepted by the action.
    */
    template <typename ...Parameters>
    struct InvokableCall : public BNM::IL2CPP::Il2CppObject {
        constexpr InvokableCall() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityAction<Parameters...> *action{};
    };

    /**
        @brief UnityEngine.Events.InvokableCallList implementation.
        Manages persistent, runtime, and executing call lists for a UnityEvent.
    */
    struct InvokableCallList : public BNM::IL2CPP::Il2CppObject {
        constexpr InvokableCallList() : BNM::IL2CPP::Il2CppObject({}) {}
        Structures::Mono::List<InvokableCallBase *> *m_PersistentCalls{};
        Structures::Mono::List<InvokableCallBase *> *m_RuntimeCalls{};
        Structures::Mono::List<InvokableCallBase *> *m_ExecutingCalls{};
        bool m_NeedsUpdate{};
    };

    /**
        @brief UnityEngine.Events.UnityEventBase implementation.
        Abstract base class for all UnityEvent variations.
    */
    struct UnityEventBase : public IL2CPP::Il2CppObject {
        constexpr UnityEventBase() : BNM::IL2CPP::Il2CppObject({}) {}
        InvokableCallList *m_Calls{};
        PersistentCallGroup *m_PersistentCalls{};
        bool m_CallsDirty = true;

        /**
            @brief Gets the BNM Class for the argument required by the persistent call.
            @param call Pointer to PersistentCall.
            @return BNM::Class descriptor of the argument.
        */
        static BNM::Class GetArgumentType(PersistentCall *call);

        /**
            @brief Gets the BNM Class for the target object in the persistent call.
            @param call Pointer to PersistentCall.
            @return BNM::Class descriptor of the target object.
        */
        static BNM::Class GetTargetType(PersistentCall *call);
    };

    /**
        @brief UnityEngine.Events.UnityEvent implementation.
        Zero or multi-argument UnityEvent for event dispatching and listeners.
        @tparam Parameters Argument types passed upon event invocation.
    */
    template<typename ...Parameters>
    struct UnityEvent : public UnityEventBase {
        Structures::Mono::Array<IL2CPP::Il2CppObject *> *m_InvokeArray{};

        /**
            @brief Add a non-persistent listener to the UnityEvent.
            @param action UnityAction delegate pointer to register.
        */
        inline void AddListener(UnityAction<Parameters...> *action) { BNM::Class(klass).GetMethod(BNM_OBFUSCATE("AddListener")).template cast<void>()[this](action); }

        /**
            @brief Add a non-persistent listener from a lambda/functor directly.
            @param callable Lambda or functor to invoke.
            @return Pointer to newly created UnityAction delegate.
        */
        template<typename F, typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, UnityAction<Parameters...> *>>>
        inline UnityAction<Parameters...> *AddListener(F &&callable) {
            auto action = CreateUnityAction<Parameters...>(std::forward<F>(callable));
            AddListener(action);
            return action;
        }

        /**
            @brief Operator overload for adding a UnityAction listener.
            @param action UnityAction delegate pointer.
            @return Reference to current UnityEvent.
        */
        inline UnityEvent &operator+=(UnityAction<Parameters...> *action) { AddListener(action); return *this; }

        /**
            @brief Operator overload for adding a callable lambda/functor listener.
            @param callable Lambda or functor to invoke.
            @return Reference to current UnityEvent.
        */
        template<typename F, typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, UnityAction<Parameters...> *>>>
        inline UnityEvent &operator+=(F &&callable) { AddListener(std::forward<F>(callable)); return *this; }

        /**
            @brief Remove a non-persistent listener from the UnityEvent.
            @param action UnityAction delegate pointer to unregister.
        */
        inline void RemoveListener(UnityAction<Parameters...> *action) { BNM::Class(klass).GetMethod(BNM_OBFUSCATE("RemoveListener")).template cast<void>()[this](action); }

        /**
            @brief Operator overload for removing a UnityAction listener.
            @param action UnityAction delegate pointer.
            @return Reference to current UnityEvent.
        */
        inline UnityEvent &operator-=(UnityAction<Parameters...> *action) { RemoveListener(action); return *this; }

        /**
            @brief Invokes all registered persistent and runtime listeners for this event.
            @param parameters Arguments to supply to listener invocations.
        */
        inline void Invoke(Parameters ...parameters) {
            if (m_CallsDirty) InvokePersistent(parameters...);
            else InvokeCalls(parameters...);
        }

        /**
            @brief Invokes all persistent calls stored in m_PersistentCalls.
            @param parameters Arguments to pass to persistent calls.
        */
        inline void InvokePersistent(Parameters ...parameters) {
            if (!m_PersistentCalls || !m_PersistentCalls->m_Calls) return;

            auto calls = m_PersistentCalls->m_Calls;
            for (int i = 0; i < calls->size; ++i) {
                auto call = calls->At(i);
                if (!call || !call->IsValid()) continue;

                auto target = call->m_Target;
                auto targetType = GetTargetType(call);
                if (!target || !targetType.IsValid()) continue;

                switch (call->m_Mode) {
                    case PersistentListenerMode::EventDefined: {
                        auto method = targetType.GetMethod(call->m_MethodName->str(), {BNM::Defaults::Get<Parameters>()...});
                        if (method.IsValid()) method.template cast<void>()[target](parameters...);
                        break;
                    }
                    case PersistentListenerMode::Void: {
                        auto method = targetType.GetMethod(call->m_MethodName->str(), 0);
                        if (method.IsValid()) method.template cast<void>()[target]();
                        break;
                    }
                    case PersistentListenerMode::Object: {
                        auto argumentType = GetArgumentType(call);
                        if (!argumentType.IsValid()) continue;

                        auto method = targetType.GetMethod(call->m_MethodName->str(), {argumentType});
                        if (method.IsValid()) method.template cast<void>()[target](call->m_Arguments->m_ObjectArgument);
                        break;
                    }
                    case PersistentListenerMode::Int: {
                        auto method = targetType.GetMethod(call->m_MethodName->str(), {BNM::Defaults::Get<int>()});
                        if (method.IsValid()) method.template cast<void>()[target](call->m_Arguments->m_IntArgument);
                        break;
                    }
                    case PersistentListenerMode::Float: {
                        auto method = targetType.GetMethod(call->m_MethodName->str(), {BNM::Defaults::Get<float>()});
                        if (method.IsValid()) method.template cast<void>()[target](call->m_Arguments->m_FloatArgument);
                        break;
                    }
                    case PersistentListenerMode::String: {
                        auto method = targetType.GetMethod(call->m_MethodName->str(), {BNM::Defaults::Get<Structures::Mono::String *>()});
                        if (method.IsValid()) method.template cast<void>()[target](call->m_Arguments->m_StringArgument);
                        break;
                    }
                    case PersistentListenerMode::Bool: {
                        auto method = targetType.GetMethod(call->m_MethodName->str(), {BNM::Defaults::Get<bool>()});
                        if (method.IsValid()) method.template cast<void>()[target](call->m_Arguments->m_BoolArgument);
                        break;
                    }
                }
            }
        }

        /**
            @brief Invokes event using the invokable calls list.
            @param parameters Event arguments.
        */
        inline void InvokeCalls(Parameters ...parameters) {
            if (!m_Calls) return;

            InvokeList((Structures::Mono::List<InvokableCall<Parameters...> *> *) m_Calls->m_PersistentCalls, parameters...);
            InvokeList((Structures::Mono::List<InvokableCall<Parameters...> *> *) m_Calls->m_RuntimeCalls, parameters...);
        }

    private:
        void InvokeList(Structures::Mono::List<InvokableCall<Parameters...> *> *list, Parameters ...parameters) {
            if (!list) return;

            for (int i = 0; i < list->size; ++i) {
                auto call = list->At(i);
                if (!call || !call->action) continue;
                call->action->Invoke(parameters...);
            }
        }
    };

    /**
        @brief Convenience function to attach a scoped listener to a UnityEvent using a callable lambda.
        @tparam Parameters Event parameters.
        @tparam F Callable functor/lambda type.
        @param unityEvent Target UnityEvent pointer.
        @param callable Callable to invoke.
        @return ScopedEventListener guard managing the subscription lifecycle.
    */
    template<typename ...Parameters, typename F>
    inline BNM::ScopedEventListener Listen(UnityEvent<Parameters...> *unityEvent, F &&callable) {
        if (!unityEvent) return BNM::ScopedEventListener();
        auto action = unityEvent->AddListener(std::forward<F>(callable));
        return BNM::ScopedEventListener([unityEvent, action]() {
            unityEvent->RemoveListener(action);
        });
    }
}
