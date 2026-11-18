#pragma once

#include <vector>
#include <tuple>
#include <type_traits>
#include <utility>
#include <functional>
#include <string>

#include "UserSettings/GlobalSettings.hpp"
#include "Method.hpp"
#include "BasicMonoStructures.hpp"
#include "Utils.hpp"
#include "Class.hpp"
#include "Defaults.hpp"

// NOLINTBEGIN
namespace BNM {

    template<typename Ret>
    struct Delegate;
    template<typename Ret>
    struct MulticastDelegate;

    /**
        @brief Wrapper of Il2CppDelegate
    */
    struct DelegateBase : public IL2CPP::Il2CppDelegate {
        inline DelegateBase() = default;

        /**
            @brief Get instance of delegate.
            @return Instance if delegate isn't null, otherwise null.
        */
        inline IL2CPP::Il2CppObject* GetInstance() const {
            return CheckForNull(this) ? target : nullptr;
        }

        /**
            @brief Get method of delegate.
            @return MethodBase if delegate isn't null, otherwise empty MethodBase.
        */
        BNM::MethodBase GetMethod() const;

        /**
            @brief Create delegate from method.
            @param method Method that will be used in new delegate
            @return New DelegateBase
        */
        DelegateBase *Create(BNM::MethodBase method);

        /**
            @brief Check if delegate is valid.
        */
        [[nodiscard]] inline bool IsValid() const noexcept { return CheckForNull(this); }

        /**
            @brief Check if delegate is valid.
        */
        inline operator bool() const noexcept { return IsValid(); }

        /**
            @brief Cast delegate to be able to invoke it.
        */
        template<typename NewRet>
        inline Delegate<NewRet> &cast() const { return (Delegate<NewRet> &) *this; }
    };

    /**
        @brief Wrapper of Il2CppMulticastDelegate
    */
    struct MulticastDelegateBase : public IL2CPP::Il2CppMulticastDelegate {
        inline MulticastDelegateBase() = default;

        /**
            @brief Get methods of delegate.
            @return Vector of MethodBase if delegate isn't null, otherwise empty vector.
        */
        std::vector<BNM::MethodBase> GetMethods() const;

        /**
            @brief Add delegate.
            @param delegate Delegate to add
        */
        void Add(DelegateBase *delegate);

        /**
            @brief Add method to delegate.
            @param method Method to add
            @return New added DelegateBase of method
        */
        DelegateBase *Add(BNM::MethodBase method);

        /**
            @brief Remove delegate.
            @param delegate Delegate to remove
        */
        void Remove(DelegateBase *delegate);

        inline void operator +=(DelegateBase *delegate) { return Add(delegate); }
        inline void operator +=(BNM::MethodBase method) { return (void) Add(method); }
        inline void operator -=(DelegateBase *delegate) { return Remove(delegate); }

        /**
            @brief Cast delegate to be able to invoke it.
        */
        template<typename NewRet>
        inline MulticastDelegate<NewRet> &cast() const { return (MulticastDelegate<NewRet> &) *this; }
    };

    /**
        @brief Typed wrapper of Il2CppDelegate
        @tparam Ret Return type
    */
    template<typename Ret>
    struct Delegate : public DelegateBase {

        /**
            @brief Invoke delegate.
            @tparam Parameters Delegate parameter types
            @param parameters Delegate parameters
        */
        template<typename ...Parameters>
        inline Ret Invoke(Parameters ...parameters) {
            return GetMethod().template cast<Ret>().Call(parameters...);
        }

        template<typename ...Parameters>
        inline Ret operator()(Parameters ...parameters) { return Invoke(parameters...); }
    };

    /**
        @brief Typed wrapper of Il2CppMulticastDelegate
        @tparam Ret Return type
    */
    template<typename Ret>
    struct MulticastDelegate : public MulticastDelegateBase {

        /**
            @brief Invoke delegate.
            @tparam Parameters Delegate parameter types
            @param parameters Delegate parameters
        */
        template<typename ...Parameters>
        inline Ret Invoke(Parameters ...parameters) {
            if (!CheckForNull(this)) return BNM::PRIVATE_INTERNAL::ReturnEmpty<Ret>();

            auto delegates = (Structures::Mono::Array<DelegateBase *> *) this->delegates;
            if (!delegates || delegates->capacity == 0) return ((Delegate<Ret>*)this)->Invoke(parameters...);

            for (IL2CPP::il2cpp_array_size_t i = 0; i < delegates->capacity - 1; ++i) {
                auto d = delegates->At(i);
                if (d) d->GetMethod().template cast<Ret>().Call(parameters...);
            }
            auto last = delegates->At(delegates->capacity - 1);
            if (last) return last->GetMethod().template cast<Ret>().Call(parameters...);
            return BNM::PRIVATE_INTERNAL::ReturnEmpty<Ret>();
        }

        template<typename ...Parameters>
        inline Ret operator()(Parameters ...parameters) { return Invoke(parameters...); }
    };

    namespace Structures::Mono {
        /**
           @brief System.Action type implementation
        */
        template <typename ...Parameters>
        struct Action : public MulticastDelegate<void> {};
    }

    /// @cond
    namespace PRIVATE_DELEGATES {

        // --- Function Traits for Type Deduction ---
        template<typename T>
        struct function_traits : public function_traits<decltype(&std::decay_t<T>::operator())> {};

        // Free function pointer
        template<typename Ret, typename ...Args>
        struct function_traits<Ret(*)(Args...)> {
            using return_type = Ret;
            using args_tuple = std::tuple<Args...>;
            static constexpr size_t arity = sizeof...(Args);
            template<size_t N>
            using arg = std::tuple_element_t<N, args_tuple>;
            using function_type = Ret(Args...);
        };

        // Member function pointer (non-const)
        template<typename ClassType, typename Ret, typename ...Args>
        struct function_traits<Ret(ClassType::*)(Args...)> {
            using return_type = Ret;
            using class_type = ClassType;
            using args_tuple = std::tuple<Args...>;
            static constexpr size_t arity = sizeof...(Args);
            template<size_t N>
            using arg = std::tuple_element_t<N, args_tuple>;
            using function_type = Ret(Args...);
        };

        // Member function pointer (const)
        template<typename ClassType, typename Ret, typename ...Args>
        struct function_traits<Ret(ClassType::*)(Args...) const> {
            using return_type = Ret;
            using class_type = ClassType;
            using args_tuple = std::tuple<Args...>;
            static constexpr size_t arity = sizeof...(Args);
            template<size_t N>
            using arg = std::tuple_element_t<N, args_tuple>;
            using function_type = Ret(Args...);
        };

#if __cpp_noexcept_function_type >= 201510L
        // Member function pointer (noexcept)
        template<typename ClassType, typename Ret, typename ...Args>
        struct function_traits<Ret(ClassType::*)(Args...) noexcept> : function_traits<Ret(ClassType::*)(Args...)> {};

        template<typename ClassType, typename Ret, typename ...Args>
        struct function_traits<Ret(ClassType::*)(Args...) const noexcept> : function_traits<Ret(ClassType::*)(Args...) const> {};
#endif

        // --- Closure Holder on Managed GC Heap ---
        template<typename Functor, typename Ret, typename ...Args>
        struct DelegateClosureHolder : public BNM::IL2CPP::Il2CppObject {
            Functor _functor;

            template<typename F>
            DelegateClosureHolder(F &&f) : _functor(std::forward<F>(f)) {}

            static Ret Invoke(DelegateClosureHolder *self, Args ...args) {
                // Auto-attach current thread to IL2CPP VM for thread-safety
                BNM::AttachIl2Cpp();
                if (!self) return BNM::PRIVATE_INTERNAL::ReturnEmpty<Ret>();

                if constexpr (std::is_void_v<Ret>) {
                    try {
                        self->_functor(args...);
                    } catch (...) {
                        BNM_LOG_ERR("BNM: Exception occurred inside Delegate invocation!");
                    }
                } else {
                    try {
                        return self->_functor(args...);
                    } catch (...) {
                        BNM_LOG_ERR("BNM: Exception occurred inside Delegate invocation!");
                        return BNM::PRIVATE_INTERNAL::ReturnEmpty<Ret>();
                    }
                }
            }
        };

        // Delegate Class Resolvers
        inline BNM::Class GetActionClass(size_t paramCount) {
            if (paramCount == 0) return BNM::Class("System", "Action");
            std::string name = "Action`" + std::to_string(paramCount);
            return BNM::Class("System", name);
        }

        inline BNM::Class GetFuncClass(size_t paramCount) {
            std::string name = "Func`" + std::to_string(paramCount + 1);
            return BNM::Class("System", name);
        }

        inline BNM::Class GetUnityActionClass(size_t paramCount) {
            if (paramCount == 0) return BNM::Class("UnityEngine.Events", "UnityAction");
            std::string name = "UnityAction`" + std::to_string(paramCount);
            return BNM::Class("UnityEngine.Events", name);
        }

        // Generic Delegate Factory from Any Callable
        template<typename Ret, typename ...Args, typename F>
        inline Delegate<Ret> *CreateDelegateFromCallable(const BNM::Class &delegateClass, F &&callable) {
            using FunctorType = std::decay_t<F>;
            using HolderType = DelegateClosureHolder<FunctorType, Ret, Args...>;

            if (!delegateClass.IsValid()) {
                BNM_LOG_ERR("BNM: Failed to create delegate, target delegate class is invalid!");
                return nullptr;
            }

            // Allocate memory on GC heap for the closure holder
            void *mem = BNM::Allocate(sizeof(HolderType));
            if (!mem) {
                BNM_LOG_ERR("BNM: Failed to allocate GC memory for DelegateClosureHolder!");
                return nullptr;
            }

            auto holder = new (mem) HolderType(std::forward<F>(callable));
            holder->klass = (IL2CPP::Il2CppClass *) BNM::Defaults::Get<IL2CPP::Il2CppObject>().ToClass().GetClass();

            // Create new delegate object of target class
            auto delegateObj = (Delegate<Ret> *) delegateClass.CreateNewInstance();
            if (!delegateObj) {
                BNM_LOG_ERR("BNM: Failed to create delegate instance for class %s!", delegateClass.str().c_str());
                return nullptr;
            }

            delegateObj->target = (IL2CPP::Il2CppObject *) holder;
            delegateObj->method_ptr = (IL2CPP::Il2CppMethodPointer) &HolderType::Invoke;
            return delegateObj;
        }

        template<typename F, typename Tuple, size_t... Is>
        auto CreateActionFromTuple(F &&callable, std::index_sequence<Is...>);

        template<typename F, typename Ret, typename Tuple, size_t... Is>
        auto CreateFuncFromTuple(F &&callable, std::index_sequence<Is...>);
    }
    /// @endcond

    // ==========================================
    // --- Public Delegate Factory Functions ---
    // ==========================================

    /// @cond
    namespace PRIVATE_DELEGATES {
        template<typename Tuple, typename F, size_t... Is>
        auto CreateActionFromTuple(F &&callable, std::index_sequence<Is...>) {
            auto cls = GetActionClass(sizeof...(Is));
            if constexpr (sizeof...(Is) > 0) {
                cls = cls.GetGeneric({BNM::Defaults::Get<std::tuple_element_t<Is, Tuple>>()...});
            }
            return (Structures::Mono::Action<std::tuple_element_t<Is, Tuple>...> *) CreateDelegateFromCallable<void, std::tuple_element_t<Is, Tuple>...>(cls, std::forward<F>(callable));
        }

        template<typename Ret, typename Tuple, typename F, size_t... Is>
        auto CreateFuncFromTuple(F &&callable, std::index_sequence<Is...>) {
            auto cls = GetFuncClass(sizeof...(Is));
            cls = cls.GetGeneric({BNM::Defaults::Get<std::tuple_element_t<Is, Tuple>>()..., BNM::Defaults::Get<Ret>()});
            return (Delegate<Ret> *) CreateDelegateFromCallable<Ret, std::tuple_element_t<Is, Tuple>...>(cls, std::forward<F>(callable));
        }
    }
    /// @endcond

    /**
        @brief Creates a System.Action delegate instance with explicit template parameters.
        @tparam FirstArg First argument type.
        @tparam OtherArgs Remaining argument types.
        @param callable Functor, lambda, or callable object to invoke.
        @return Pointer to newly created System.Action delegate.
    */
    template<typename FirstArg, typename ...OtherArgs, typename F>
    inline Structures::Mono::Action<FirstArg, OtherArgs...> *CreateAction(F &&callable) {
        auto cls = PRIVATE_DELEGATES::GetActionClass(sizeof...(OtherArgs) + 1);
        cls = cls.GetGeneric({BNM::Defaults::Get<FirstArg>(), BNM::Defaults::Get<OtherArgs>()...});
        return (Structures::Mono::Action<FirstArg, OtherArgs...> *) PRIVATE_DELEGATES::CreateDelegateFromCallable<void, FirstArg, OtherArgs...>(cls, std::forward<F>(callable));
    }

    /**
        @brief Creates a System.Action delegate instance by binding a member function on an object instance.
        @param instance Pointer to class instance.
        @param method Pointer to member function.
        @return Pointer to newly created System.Action delegate.
    */
    template<typename InstanceT, typename ClassT, typename ...Args>
    inline auto CreateAction(InstanceT *instance, void (ClassT::*method)(Args...)) {
        using ArgsTuple = std::tuple<Args...>;
        return PRIVATE_DELEGATES::CreateActionFromTuple<ArgsTuple>(
            [instance, method](Args ...args) { (instance->*method)(args...); },
            std::make_index_sequence<sizeof...(Args)>{}
        );
    }

    /**
        @brief Creates a System.Action delegate instance by binding a const member function on an object instance.
        @param instance Pointer to class instance.
        @param method Pointer to const member function.
        @return Pointer to newly created System.Action delegate.
    */
    template<typename InstanceT, typename ClassT, typename ...Args>
    inline auto CreateAction(InstanceT *instance, void (ClassT::*method)(Args...) const) {
        using ArgsTuple = std::tuple<Args...>;
        return PRIVATE_DELEGATES::CreateActionFromTuple<ArgsTuple>(
            [instance, method](Args ...args) { (instance->*method)(args...); },
            std::make_index_sequence<sizeof...(Args)>{}
        );
    }

    /**
        @brief Creates a System.Action delegate instance with parameter types deduced automatically from the lambda.
        @param callable Lambda or functor with auto-deducible parameters.
        @return Pointer to newly created System.Action delegate.
    */
    template<typename F, typename = std::enable_if_t<!std::is_member_function_pointer_v<std::decay_t<F>>>>
    inline auto CreateAction(F &&callable) {
        using Traits = PRIVATE_DELEGATES::function_traits<std::decay_t<F>>;
        using ArgsTuple = typename Traits::args_tuple;
        return PRIVATE_DELEGATES::CreateActionFromTuple<ArgsTuple>(std::forward<F>(callable), std::make_index_sequence<Traits::arity>{});
    }

    /**
        @brief Creates a System.Func delegate instance from a callable object.
        @tparam Ret Return type.
        @tparam Args Parameter types.
        @param callable Functor, lambda, or callable object to invoke.
        @return Pointer to newly created System.Func delegate.
    */
    template<typename Ret, typename ...Args, typename F>
    inline Delegate<Ret> *CreateFunc(F &&callable) {
        auto cls = PRIVATE_DELEGATES::GetFuncClass(sizeof...(Args));
        cls = cls.GetGeneric({BNM::Defaults::Get<Args>()..., BNM::Defaults::Get<Ret>()});
        return PRIVATE_DELEGATES::CreateDelegateFromCallable<Ret, Args...>(cls, std::forward<F>(callable));
    }

    /**
        @brief Creates a System.Func delegate instance by binding a member function on an object instance.
        @param instance Pointer to class instance.
        @param method Pointer to member function.
        @return Pointer to newly created System.Func delegate.
    */
    template<typename InstanceT, typename ClassT, typename Ret, typename ...Args>
    inline auto CreateFunc(InstanceT *instance, Ret (ClassT::*method)(Args...)) {
        return CreateFunc<Ret, Args...>([instance, method](Args ...args) -> Ret {
            return (instance->*method)(args...);
        });
    }

    /**
        @brief Creates a System.Func delegate instance by binding a const member function on an object instance.
        @param instance Pointer to class instance.
        @param method Pointer to const member function.
        @return Pointer to newly created System.Func delegate.
    */
    template<typename InstanceT, typename ClassT, typename Ret, typename ...Args>
    inline auto CreateFunc(InstanceT *instance, Ret (ClassT::*method)(Args...) const) {
        return CreateFunc<Ret, Args...>([instance, method](Args ...args) -> Ret {
            return (instance->*method)(args...);
        });
    }

    /**
        @brief Creates a System.Func delegate instance with return and parameter types deduced automatically from the lambda.
        @param callable Lambda or functor with auto-deducible return and parameter types.
        @return Pointer to newly created System.Func delegate.
    */
    template<typename F, typename = std::enable_if_t<!std::is_member_function_pointer_v<std::decay_t<F>>>>
    inline auto CreateFunc(F &&callable) {
        using Traits = PRIVATE_DELEGATES::function_traits<std::decay_t<F>>;
        using Ret = typename Traits::return_type;
        using ArgsTuple = typename Traits::args_tuple;
        return PRIVATE_DELEGATES::CreateFuncFromTuple<Ret, ArgsTuple>(std::forward<F>(callable), std::make_index_sequence<Traits::arity>{});
    }

    /**
        @brief Creates a System.Predicate<T> delegate instance.
        @tparam T Parameter type evaluated by predicate.
        @param callable Functor or lambda returning bool.
        @return Pointer to newly created System.Predicate<T> delegate.
    */
    template<typename T, typename F>
    inline Delegate<bool> *CreatePredicate(F &&callable) {
        auto cls = BNM::Class("System", "Predicate`1").GetGeneric({BNM::Defaults::Get<T>()});
        return PRIVATE_DELEGATES::CreateDelegateFromCallable<bool, T>(cls, std::forward<F>(callable));
    }

    /**
        @brief Creates a System.Comparison<T> delegate instance.
        @tparam T Parameter type compared.
        @param callable Functor or lambda returning int (comparison result).
        @return Pointer to newly created System.Comparison<T> delegate.
    */
    template<typename T, typename F>
    inline Delegate<int> *CreateComparison(F &&callable) {
        auto cls = BNM::Class("System", "Comparison`1").GetGeneric({BNM::Defaults::Get<T>()});
        return PRIVATE_DELEGATES::CreateDelegateFromCallable<int, T, T>(cls, std::forward<F>(callable));
    }

    /**
        @brief Creates a System.EventHandler<T> delegate instance.
        @tparam T Event arguments type.
        @param callable Functor or lambda accepting (sender, eventArgs).
        @return Pointer to newly created System.EventHandler<T> delegate.
    */
    template<typename T, typename F>
    inline Delegate<void> *CreateEventHandler(F &&callable) {
        auto cls = BNM::Class("System", "EventHandler`1").GetGeneric({BNM::Defaults::Get<T>()});
        return PRIVATE_DELEGATES::CreateDelegateFromCallable<void, IL2CPP::Il2CppObject *, T>(cls, std::forward<F>(callable));
    }

    /**
        @brief Creates a delegate of any custom/game delegate class.
        @tparam Ret Return type of the custom delegate.
        @tparam Args Parameter types accepted by the delegate.
        @param delegateClass Target BNM::Class of the delegate type.
        @param callable Functor or lambda to invoke.
        @return Pointer to newly created Delegate<Ret>.
    */
    template<typename Ret = void, typename ...Args, typename F>
    inline Delegate<Ret> *CreateDelegate(const BNM::Class &delegateClass, F &&callable) {
        return PRIVATE_DELEGATES::CreateDelegateFromCallable<Ret, Args...>(delegateClass, std::forward<F>(callable));
    }

}
// NOLINTEND
