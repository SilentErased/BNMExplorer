#pragma once

#include <string_view>
#include <type_traits>
#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Class.hpp"
#include "Method.hpp"
#include "Field.hpp"
#include "Property.hpp"
#include "Defaults.hpp"
#include "Operators.hpp"
#include "UnityStructures/Object.hpp"

namespace BNM {

    /// @cond
    namespace Detail {
        template<typename Ret>
        constexpr inline Ret DefaultReturnValue() {
            if constexpr (std::is_void_v<Ret>) {
                return;
            } else if constexpr (std::is_pointer_v<Ret>) {
                return nullptr;
            } else {
                return Ret{};
            }
        }

        /**
            @brief Maps a C++ type to a BNM::CompileTimeClass for generic type arguments.
            Game binding structs (which expose static StaticClass()) resolve through it;
            primitives, BNM Unity wrappers and value structs fall back to
            BNM::Defaults::Get<Type>(). Pointer forms (e.g. Foo *) resolve the pointee,
            so both `(Foo)` and `(Foo *)` work as type arguments.
        */
        template<typename Type, typename = void>
        struct TypeClassResolver {
            static inline BNM::CompileTimeClass Get() { return BNM::Defaults::Get<std::remove_pointer_t<Type>>(); }
        };

        template<typename Type>
        struct TypeClassResolver<Type, std::void_t<decltype(std::remove_pointer_t<Type>::StaticClass())>> {
            static inline BNM::CompileTimeClass Get() { return std::remove_pointer_t<Type>::StaticClass(); }
        };
    }
    /// @endcond

    /**
        @brief Base wrapper interface for game binding objects providing runtime querying and dynamic casting.
    */
    template<typename Derived, typename Base = BNM::IL2CPP::Il2CppObject>
    struct GameBindingWrapper : public Base {
        using Base::Base;

        /**
            @brief Dynamically cast this game object to an interface or derived type.
            @tparam Target Target game class or interface struct.
            @return Pointer to Target if implemented and valid, otherwise nullptr.
        */
        template<typename Target>
        inline Target *As() const {
            if (!this || !BNM::CheckForNull((void *)this)) return nullptr;
            if constexpr (requires { Target::StaticClass(); }) {
                if (BNM::IsA((IL2CPP::Il2CppObject *)this, Target::StaticClass())) {
                    return (Target *)this;
                }
            } else {
                return (Target *)this;
            }
            return nullptr;
        }

        /**
            @brief Check if this game object implements or inherits from Target type.
            @tparam Target Target game class or interface struct.
            @return True if object matches Target type in IL2CPP runtime.
        */
        template<typename Target>
        inline bool IsA() const {
            if (!this || !BNM::CheckForNull((void *)this)) return false;
            if constexpr (requires { Target::StaticClass(); }) {
                return BNM::IsA((IL2CPP::Il2CppObject *)this, Target::StaticClass());
            }
            return false;
        }
    };

}

//! =================================================================================================
//! Class & Interface Declaration Macros
//! =================================================================================================

/**
    @brief Declare a custom game class binding struct.
    @param NameSpace C# namespace (e.g. "Game.Core" or "" for global).
    @param ClassName C# class name (e.g. PlayerController).
    @param BaseClass C++ base class (e.g. BNM::UnityEngine::MonoBehaviour, BNM::UnityEngine::Component, or another GameClass).
    @param ... Optional DLL Image name (e.g. "Assembly-CSharp.dll").
*/
#define BNM_GAME_CLASS(NameSpace, ClassName, BaseClass, ...) \
    struct ClassName : public BaseClass { \
        ClassName() : BaseClass() {} \
        \
        static inline BNM::Class StaticClass() { \
            static auto cls = []() -> BNM::Class { \
                std::string_view imgName{"" __VA_ARGS__}; \
                if (!imgName.empty()) { \
                    return BNM::Class(BNM_OBFUSCATE(NameSpace), BNM_OBFUSCATE(#ClassName), BNM::Image(imgName)); \
                } \
                return BNM::Class(BNM_OBFUSCATE(NameSpace), BNM_OBFUSCATE(#ClassName)); \
            }(); \
            return cls; \
        } \
        \
        static inline BNM::Class InnerClass(const std::string_view &innerName) { \
            return StaticClass().GetInnerClass(innerName); \
        } \
        \
        template<typename Target> \
        inline Target *As() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return nullptr; \
            if constexpr (requires { Target::StaticClass(); }) { \
                if (BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass())) { \
                    return (Target *)this; \
                } \
            } else { \
                return (Target *)this; \
            } \
            return nullptr; \
        } \
        \
        template<typename Target> \
        inline bool IsA() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return false; \
            if constexpr (requires { Target::StaticClass(); }) { \
                return BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass()); \
            } \
            return false; \
        }

/**
    @brief Closes a BNM_GAME_CLASS declaration.
*/
#define BNM_GAME_CLASS_END };

/**
    @brief Declare an inner/nested game class binding struct.
    @param OuterClass Outer enclosing class struct.
    @param InnerClassName Name of the nested inner class.
    @param BaseClass C++ base class (e.g. BNM::IL2CPP::Il2CppObject, BNM::UnityEngine::Component).
*/
#define BNM_GAME_INNER_CLASS(OuterClass, InnerClassName, BaseClass) \
    struct InnerClassName : public BaseClass { \
        InnerClassName() : BaseClass() {} \
        \
        static inline BNM::Class StaticClass() { \
            static auto cls = OuterClass::StaticClass().GetInnerClass(BNM_OBFUSCATE(#InnerClassName)); \
            return cls; \
        } \
        \
        static inline BNM::Class InnerClass(const std::string_view &innerName) { \
            return StaticClass().GetInnerClass(innerName); \
        } \
        \
        template<typename Target> \
        inline Target *As() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return nullptr; \
            if constexpr (requires { Target::StaticClass(); }) { \
                if (BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass())) { \
                    return (Target *)this; \
                } \
            } else { \
                return (Target *)this; \
            } \
            return nullptr; \
        } \
        \
        template<typename Target> \
        inline bool IsA() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return false; \
            if constexpr (requires { Target::StaticClass(); }) { \
                return BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass()); \
            } \
            return false; \
        }

/**
    @brief Closes a BNM_GAME_INNER_CLASS declaration.
*/
#define BNM_GAME_INNER_CLASS_END };

/**
    @brief Declare a C# interface binding struct.
    @param NameSpace C# namespace (e.g. "Game.Core" or "" for global).
    @param InterfaceName C# interface name (e.g. IDamageable, IInteractable).
    @param ... Optional DLL Image name.
*/
#define BNM_GAME_INTERFACE(NameSpace, InterfaceName, ...) \
    struct InterfaceName : public BNM::IL2CPP::Il2CppObject { \
        constexpr InterfaceName() : BNM::IL2CPP::Il2CppObject({}) {} \
        \
        static inline BNM::Class StaticClass() { \
            static auto cls = []() -> BNM::Class { \
                std::string_view imgName{"" __VA_ARGS__}; \
                if (!imgName.empty()) { \
                    return BNM::Class(BNM_OBFUSCATE(NameSpace), BNM_OBFUSCATE(#InterfaceName), BNM::Image(imgName)); \
                } \
                return BNM::Class(BNM_OBFUSCATE(NameSpace), BNM_OBFUSCATE(#InterfaceName)); \
            }(); \
            return cls; \
        } \
        \
        template<typename Target> \
        inline Target *As() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return nullptr; \
            if constexpr (requires { Target::StaticClass(); }) { \
                if (BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass())) { \
                    return (Target *)this; \
                } \
            } else { \
                return (Target *)this; \
            } \
            return nullptr; \
        } \
        \
        template<typename Target> \
        inline bool IsA() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return false; \
            if constexpr (requires { Target::StaticClass(); }) { \
                return BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass()); \
            } \
            return false; \
        }

/**
    @brief Closes a BNM_GAME_INTERFACE declaration.
*/
#define BNM_GAME_INTERFACE_END };

//! =================================================================================================
//! Field & Property Binding Macros
//! =================================================================================================

/**
    @brief Bind an instance field with direct reference access.
    @param Name Field name in C#.
    @param Type Field data type.
*/
#define BNM_BINDING_FIELD(Name, Type) \
    inline Type &Name() { \
        static auto _field = StaticClass().GetField(BNM_OBFUSCATE(#Name)).template cast<Type>(); \
        return *_field[(void *)this].GetPointer(); \
    }

/**
    @brief Bind a static field with direct reference access.
    @param Name Static field name in C#.
    @param Type Static field data type.
*/
#define BNM_BINDING_STATIC_FIELD(Name, Type) \
    static inline Type &Name() { \
        static auto _field = StaticClass().GetField(BNM_OBFUSCATE(#Name)).template cast<Type>(); \
        return *_field.GetPointer(); \
    }

/**
    @brief Bind a C# property (generating Get<Name>() and Set<Name>(val)).
    @param Name Property name in C#.
    @param Type Property data type.
*/
#define BNM_BINDING_PROPERTY(Name, Type) \
    inline Type Get##Name() const { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<Type>(); \
        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE(#Name)).template cast<Type>(); \
        return _prop[(void *)this].Get(); \
    } \
    inline void Set##Name(Type value) { \
        if (!this || !BNM::CheckForNull((void *)this)) return; \
        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE(#Name)).template cast<Type>(); \
        _prop[(void *)this].Set(value); \
    }

/**
    @brief Bind a C# static property (generating Get<Name>() and Set<Name>(val)).
    @param Name Static property name in C#.
    @param Type Property data type.
*/
#define BNM_BINDING_STATIC_PROPERTY(Name, Type) \
    static inline Type Get##Name() { \
        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE(#Name)).template cast<Type>(); \
        return _prop.Get(); \
    } \
    static inline void Set##Name(Type value) { \
        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE(#Name)).template cast<Type>(); \
        _prop.Set(value); \
    }

//! =================================================================================================
//! Method Binding Macros (Parameter Count, Parameter Types, Parameter Names)
//! =================================================================================================

/**
    @brief Bind an instance method by parameter count.
    @param Name Method name in C#.
    @param RetType Return type.
    @param ParamCount Number of parameters in C#.
    @param ParamsDecl Parenthesized parameter declaration list, e.g. (int damage, bool critical).
    @param ParamsCall Parenthesized call argument list, e.g. (damage, critical).
*/
#define BNM_BINDING_METHOD(Name, RetType, ParamCount, ParamsDecl, ParamsCall) \
    inline RetType Name ParamsDecl { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<RetType>(); \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), ParamCount).template cast<RetType>(); \
        return _method[(void *)this] ParamsCall; \
    }

/**
    @brief Bind an instance method by specific parameter types (for overloading resolution).
    @param Name Method name in C#.
    @param RetType Return type.
    @param TypesList Parenthesized initializer list of BNM::CompileTimeClass / BNM::Class types.
                    Must be wrapped in parentheses so commas inside the list stay macro-safe:
                    (std::initializer_list<BNM::CompileTimeClass>{ClassA, ClassB}).
    @param ParamsDecl Parenthesized parameter declaration list.
    @param ParamsCall Parenthesized call argument list.
*/
#define BNM_BINDING_METHOD_TYPES(Name, RetType, TypesList, ParamsDecl, ParamsCall) \
    inline RetType Name ParamsDecl { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<RetType>(); \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), TypesList).template cast<RetType>(); \
        return _method[(void *)this] ParamsCall; \
    }

#define BNM_BINDING_METHOD_TYPE(Name, RetType, TypesList, ParamsDecl, ParamsCall) BNM_BINDING_METHOD_TYPES(Name, RetType, TypesList, ParamsDecl, ParamsCall)

/**
    @brief Bind an instance method by parameter names (for overloading resolution).
    @param Name Method name in C#.
    @param RetType Return type.
    @param NamesList Parenthesized initializer list of parameter name string_views.
                    Must be wrapped in parentheses so commas inside the list stay macro-safe:
                    (std::initializer_list<std::string_view>{"first", "second"}).
    @param ParamsDecl Parenthesized parameter declaration list.
    @param ParamsCall Parenthesized call argument list.
*/
#define BNM_BINDING_METHOD_NAMES(Name, RetType, NamesList, ParamsDecl, ParamsCall) \
    inline RetType Name ParamsDecl { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<RetType>(); \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), NamesList).template cast<RetType>(); \
        return _method[(void *)this] ParamsCall; \
    }

#define BNM_BINDING_METHOD_NAME(Name, RetType, NamesList, ParamsDecl, ParamsCall) BNM_BINDING_METHOD_NAMES(Name, RetType, NamesList, ParamsDecl, ParamsCall)

/**
    @brief Bind a static method by parameter count.
    @param Name Static method name in C#.
    @param RetType Return type.
    @param ParamCount Number of parameters in C#.
    @param ParamsDecl Parenthesized parameter declaration list, e.g. (int damage, bool critical).
    @param ParamsCall Parenthesized call argument list, e.g. (damage, critical).
*/
#define BNM_BINDING_STATIC_METHOD(Name, RetType, ParamCount, ParamsDecl, ParamsCall) \
    static inline RetType Name ParamsDecl { \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), ParamCount).template cast<RetType>(); \
        return _method ParamsCall; \
    }

/**
    @brief Bind a static method by parameter types (for overloading resolution).
    @param Name Static method name in C#.
    @param RetType Return type.
    @param TypesList Parenthesized initializer list of BNM::CompileTimeClass / BNM::Class types
                    (see BNM_BINDING_METHOD_TYPES for the parenthesis requirement).
    @param ParamsDecl Parenthesized parameter declaration list.
    @param ParamsCall Parenthesized call argument list.
*/
#define BNM_BINDING_STATIC_METHOD_TYPES(Name, RetType, TypesList, ParamsDecl, ParamsCall) \
    static inline RetType Name ParamsDecl { \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), TypesList).template cast<RetType>(); \
        return _method ParamsCall; \
    }

#define BNM_BINDING_STATIC_METHOD_TYPE(Name, RetType, TypesList, ParamsDecl, ParamsCall) BNM_BINDING_STATIC_METHOD_TYPES(Name, RetType, TypesList, ParamsDecl, ParamsCall)

/**
    @brief Bind a static method by parameter names (for overloading resolution).
    @param Name Static method name in C#.
    @param RetType Return type.
    @param NamesList Parenthesized initializer list of parameter name string_views
                    (see BNM_BINDING_METHOD_NAMES for the parenthesis requirement).
    @param ParamsDecl Parenthesized parameter declaration list.
    @param ParamsCall Parenthesized call argument list.
*/
#define BNM_BINDING_STATIC_METHOD_NAMES(Name, RetType, NamesList, ParamsDecl, ParamsCall) \
    static inline RetType Name ParamsDecl { \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), NamesList).template cast<RetType>(); \
        return _method ParamsCall; \
    }

#define BNM_BINDING_STATIC_METHOD_NAME(Name, RetType, NamesList, ParamsDecl, ParamsCall) BNM_BINDING_STATIC_METHOD_NAMES(Name, RetType, NamesList, ParamsDecl, ParamsCall)

//! =================================================================================================
//! Generic Class & Method Binding Macros
//! =================================================================================================
//
// C# generics need an explicit GetGeneric({...}) at resolution time:
//   * generic CLASS  -> BNM::Class(ns, "Name`N", img).GetGeneric({...}) per instantiation
//   * generic METHOD -> GetMethod(name, count).GetGeneric({...}).cast<Ret>()
//
// Complex types in these macros are ALWAYS written inside parentheses, e.g.
// (Dictionary<K, V> *) or (GB::Base<T>); BNM_UNPAREN strips the outer
// parentheses so commas inside template arguments never split the macro's
// parameter list.

/**
    @brief Strip the outer parentheses around a macro argument.
    Used by every generic-binding macro for comma-safe type slots.
*/
#define BNM_UNPAREN(...) __VA_ARGS__

/**
    @brief Declare a generic C# class binding as a C++ template struct.
    @param NameSpace C# namespace (e.g. "Game.Core" or "" for global).
    @param ClassName C++ struct name (kept identical to the C# class name unless arity-suffixed).
    @param CSharpName Full C# metadata name including the backtick arity, e.g. "MySingleton`1".
    @param BaseClass Parenthesized C++ base class, e.g. (BNM::UnityEngine::MonoBehaviour).
    @param TypeDecl Parenthesized template parameter declaration, e.g. (typename T1, typename T2).
    @param Resolvers Parenthesized GetGeneric type-argument list,
                    e.g. (BNM::Detail::TypeClassResolver<T1>::Get()).
    @param ... Optional DLL Image name (e.g. "Assembly-CSharp.dll").
*/
#define BNM_GAME_GENERIC_CLASS_EX(NameSpace, ClassName, CSharpName, BaseClass, TypeDecl, Resolvers, ...) \
    template<BNM_UNPAREN TypeDecl> \
    struct ClassName : public BNM_UNPAREN BaseClass { \
        ClassName() : BNM_UNPAREN BaseClass() {} \
        \
        static inline BNM::Class StaticClass() { \
            static auto cls = []() -> BNM::Class { \
                std::string_view imgName{"" __VA_ARGS__}; \
                if (!imgName.empty()) { \
                    return BNM::Class(BNM_OBFUSCATE(NameSpace), BNM_OBFUSCATE(CSharpName), BNM::Image(imgName)) \
                        .GetGeneric({BNM_UNPAREN Resolvers}); \
                } \
                return BNM::Class(BNM_OBFUSCATE(NameSpace), BNM_OBFUSCATE(CSharpName)) \
                    .GetGeneric({BNM_UNPAREN Resolvers}); \
            }(); \
            return cls; \
        } \
        \
        static inline BNM::Class InnerClass(const std::string_view &innerName) { \
            return StaticClass().GetInnerClass(innerName); \
        } \
        \
        template<typename Target> \
        inline Target *As() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return nullptr; \
            if constexpr (requires { Target::StaticClass(); }) { \
                if (BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass())) { \
                    return (Target *)this; \
                } \
            } else { \
                return (Target *)this; \
            } \
            return nullptr; \
        } \
        \
        template<typename Target> \
        inline bool IsA() const { \
            if (!this || !BNM::CheckForNull((void *)this)) return false; \
            if constexpr (requires { Target::StaticClass(); }) { \
                return BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass()); \
            } \
            return false; \
        }

/**
    @brief Closes a BNM_GAME_GENERIC_CLASS_* declaration.
*/
#define BNM_GAME_GENERIC_CLASS_END };

/**
    @brief Declare a 1-arity generic C# class binding (metadata name "<ClassName>`1").
    @param NameSpace C# namespace.
    @param ClassName C++ struct name; must match the C# class name exactly.
    @param BaseClass Parenthesized C++ base class.
    @param T1 Type parameter name.
    @param ... Optional DLL Image name.
*/
#define BNM_GAME_GENERIC_CLASS_1(NameSpace, ClassName, BaseClass, T1, ...) \
    BNM_GAME_GENERIC_CLASS_EX(NameSpace, ClassName, #ClassName "\x60" "1", BaseClass, \
        (typename T1), (BNM::Detail::TypeClassResolver<T1>::Get()), __VA_ARGS__)

/**
    @brief Declare a 2-arity generic C# class binding (metadata name "<ClassName>`2").
    See BNM_GAME_GENERIC_CLASS_1 for the parameter conventions.
*/
#define BNM_GAME_GENERIC_CLASS_2(NameSpace, ClassName, BaseClass, T1, T2, ...) \
    BNM_GAME_GENERIC_CLASS_EX(NameSpace, ClassName, #ClassName "\x60" "2", BaseClass, \
        (typename T1, typename T2), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get()), __VA_ARGS__)

/**
    @brief Declare a 3-arity generic C# class binding (metadata name "<ClassName>`3").
    See BNM_GAME_GENERIC_CLASS_1 for the parameter conventions.
*/
#define BNM_GAME_GENERIC_CLASS_3(NameSpace, ClassName, BaseClass, T1, T2, T3, ...) \
    BNM_GAME_GENERIC_CLASS_EX(NameSpace, ClassName, #ClassName "\x60" "3", BaseClass, \
        (typename T1, typename T2, typename T3), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get(), BNM::Detail::TypeClassResolver<T3>::Get()), __VA_ARGS__)

/**
    @brief Bind an instance field whose type contains commas (e.g. Dictionary<K, V> *).
    @param Name Field name in C#.
    @param Type Parenthesized field data type.
*/
#define BNM_BINDING_GENERIC_FIELD(Name, Type) \
    inline BNM_UNPAREN Type &Name() { \
        static auto _field = StaticClass().GetField(BNM_OBFUSCATE(#Name)).template cast<BNM_UNPAREN Type>(); \
        return *_field[(void *)this].GetPointer(); \
    }

/**
    @brief Bind a static field whose type contains commas.
    @param Name Static field name in C#.
    @param Type Parenthesized field data type.
*/
#define BNM_BINDING_GENERIC_STATIC_FIELD(Name, Type) \
    static inline BNM_UNPAREN Type &Name() { \
        static auto _field = StaticClass().GetField(BNM_OBFUSCATE(#Name)).template cast<BNM_UNPAREN Type>(); \
        return *_field.GetPointer(); \
    }

/**
    @brief Bind a C# property whose type contains commas (generating Get<Name>()/Set<Name>(val)).
    @param Name Property name in C#.
    @param Type Parenthesized property data type.
*/
#define BNM_BINDING_GENERIC_PROPERTY(Name, Type) \
    inline BNM_UNPAREN Type Get##Name() const { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<BNM_UNPAREN Type>(); \
        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE(#Name)).template cast<BNM_UNPAREN Type>(); \
        return _prop[(void *)this].Get(); \
    } \
    inline void Set##Name(BNM_UNPAREN Type value) { \
        if (!this || !BNM::CheckForNull((void *)this)) return; \
        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE(#Name)).template cast<BNM_UNPAREN Type>(); \
        _prop[(void *)this].Set(value); \
    }

/**
    @brief Bind a C# static property whose type contains commas.
    @param Name Static property name in C#.
    @param Type Parenthesized property data type.
*/
#define BNM_BINDING_GENERIC_STATIC_PROPERTY(Name, Type) \
    static inline BNM_UNPAREN Type Get##Name() { \
        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE(#Name)).template cast<BNM_UNPAREN Type>(); \
        return _prop.Get(); \
    } \
    static inline void Set##Name(BNM_UNPAREN Type value) { \
        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE(#Name)).template cast<BNM_UNPAREN Type>(); \
        _prop.Set(value); \
    }

/**
    @brief Bind an instance generic method by parameter count (template method).
    @param Name Method name in C#.
    @param RetType Parenthesized return type.
    @param TypeDecl Parenthesized template parameter declaration, e.g. (typename T).
    @param Resolvers Parenthesized GetGeneric type-argument list.
    @param ParamCount Number of parameters in C#.
    @param ParamsDecl Parenthesized parameter declaration list.
    @param ParamsCall Parenthesized call argument list.
    @note Generic methods whose type parameters appear only in the return type must be
          called with an explicit template argument list, e.g. GetOrCreate<PlayerController>().
*/
#define BNM_BINDING_GENERIC_METHOD_EX(Name, RetType, TypeDecl, Resolvers, ParamCount, ParamsDecl, ParamsCall) \
    template<BNM_UNPAREN TypeDecl> \
    inline BNM_UNPAREN RetType Name ParamsDecl { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<BNM_UNPAREN RetType>(); \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), ParamCount) \
            .GetGeneric({BNM_UNPAREN Resolvers}).template cast<BNM_UNPAREN RetType>(); \
        return _method[(void *)this] ParamsCall; \
    }

/**
    @brief Bind a static generic method by parameter count (template method).
    See BNM_BINDING_GENERIC_METHOD_EX for the parameter conventions.
*/
#define BNM_BINDING_STATIC_GENERIC_METHOD_EX(Name, RetType, TypeDecl, Resolvers, ParamCount, ParamsDecl, ParamsCall) \
    template<BNM_UNPAREN TypeDecl> \
    static inline BNM_UNPAREN RetType Name ParamsDecl { \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), ParamCount) \
            .GetGeneric({BNM_UNPAREN Resolvers}).template cast<BNM_UNPAREN RetType>(); \
        return _method ParamsCall; \
    }

/**
    @brief Bind an instance generic method by parameter types (for overloading resolution).
    @param TypesList Parenthesized initializer list of BNM::CompileTimeClass types, e.g.
                    (std::initializer_list<BNM::CompileTimeClass>{ClassA, ClassB}).
    See BNM_BINDING_GENERIC_METHOD_EX for the remaining parameters.
*/
#define BNM_BINDING_GENERIC_METHOD_TYPES_EX(Name, RetType, TypeDecl, Resolvers, TypesList, ParamsDecl, ParamsCall) \
    template<BNM_UNPAREN TypeDecl> \
    inline BNM_UNPAREN RetType Name ParamsDecl { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<BNM_UNPAREN RetType>(); \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), BNM_UNPAREN TypesList) \
            .GetGeneric({BNM_UNPAREN Resolvers}).template cast<BNM_UNPAREN RetType>(); \
        return _method[(void *)this] ParamsCall; \
    }

/**
    @brief Bind a static generic method by parameter types.
    See BNM_BINDING_GENERIC_METHOD_TYPES_EX for the parameter conventions.
*/
#define BNM_BINDING_STATIC_GENERIC_METHOD_TYPES_EX(Name, RetType, TypeDecl, Resolvers, TypesList, ParamsDecl, ParamsCall) \
    template<BNM_UNPAREN TypeDecl> \
    static inline BNM_UNPAREN RetType Name ParamsDecl { \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), BNM_UNPAREN TypesList) \
            .GetGeneric({BNM_UNPAREN Resolvers}).template cast<BNM_UNPAREN RetType>(); \
        return _method ParamsCall; \
    }

/**
    @brief Bind an instance generic method by parameter names (for overloading resolution).
    @param NamesList Parenthesized initializer list of parameter name string_views, e.g.
                    (std::initializer_list<std::string_view>{"first", "second"}).
    See BNM_BINDING_GENERIC_METHOD_EX for the remaining parameters.
*/
#define BNM_BINDING_GENERIC_METHOD_NAMES_EX(Name, RetType, TypeDecl, Resolvers, NamesList, ParamsDecl, ParamsCall) \
    template<BNM_UNPAREN TypeDecl> \
    inline BNM_UNPAREN RetType Name ParamsDecl { \
        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<BNM_UNPAREN RetType>(); \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), BNM_UNPAREN NamesList) \
            .GetGeneric({BNM_UNPAREN Resolvers}).template cast<BNM_UNPAREN RetType>(); \
        return _method[(void *)this] ParamsCall; \
    }

/**
    @brief Bind a static generic method by parameter names.
    See BNM_BINDING_GENERIC_METHOD_NAMES_EX for the parameter conventions.
*/
#define BNM_BINDING_STATIC_GENERIC_METHOD_NAMES_EX(Name, RetType, TypeDecl, Resolvers, NamesList, ParamsDecl, ParamsCall) \
    template<BNM_UNPAREN TypeDecl> \
    static inline BNM_UNPAREN RetType Name ParamsDecl { \
        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE(#Name), BNM_UNPAREN NamesList) \
            .GetGeneric({BNM_UNPAREN Resolvers}).template cast<BNM_UNPAREN RetType>(); \
        return _method ParamsCall; \
    }

#define BNM_BINDING_GENERIC_METHOD_1(Name, RetType, T1, ParamCount, ParamsDecl, ParamsCall) \
    BNM_BINDING_GENERIC_METHOD_EX(Name, RetType, (typename T1), \
        (BNM::Detail::TypeClassResolver<T1>::Get()), ParamCount, ParamsDecl, ParamsCall)

#define BNM_BINDING_GENERIC_METHOD_2(Name, RetType, T1, T2, ParamCount, ParamsDecl, ParamsCall) \
    BNM_BINDING_GENERIC_METHOD_EX(Name, RetType, (typename T1, typename T2), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get()), ParamCount, ParamsDecl, ParamsCall)

#define BNM_BINDING_GENERIC_METHOD_3(Name, RetType, T1, T2, T3, ParamCount, ParamsDecl, ParamsCall) \
    BNM_BINDING_GENERIC_METHOD_EX(Name, RetType, (typename T1, typename T2, typename T3), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get(), BNM::Detail::TypeClassResolver<T3>::Get()), ParamCount, ParamsDecl, ParamsCall)

#define BNM_BINDING_STATIC_GENERIC_METHOD_1(Name, RetType, T1, ParamCount, ParamsDecl, ParamsCall) \
    BNM_BINDING_STATIC_GENERIC_METHOD_EX(Name, RetType, (typename T1), \
        (BNM::Detail::TypeClassResolver<T1>::Get()), ParamCount, ParamsDecl, ParamsCall)

#define BNM_BINDING_STATIC_GENERIC_METHOD_2(Name, RetType, T1, T2, ParamCount, ParamsDecl, ParamsCall) \
    BNM_BINDING_STATIC_GENERIC_METHOD_EX(Name, RetType, (typename T1, typename T2), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get()), ParamCount, ParamsDecl, ParamsCall)

#define BNM_BINDING_STATIC_GENERIC_METHOD_3(Name, RetType, T1, T2, T3, ParamCount, ParamsDecl, ParamsCall) \
    BNM_BINDING_STATIC_GENERIC_METHOD_EX(Name, RetType, (typename T1, typename T2, typename T3), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get(), BNM::Detail::TypeClassResolver<T3>::Get()), ParamCount, ParamsDecl, ParamsCall)

#define BNM_BINDING_GENERIC_METHOD_TYPES_1(Name, RetType, T1, TypesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_GENERIC_METHOD_TYPES_EX(Name, RetType, (typename T1), \
        (BNM::Detail::TypeClassResolver<T1>::Get()), TypesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_GENERIC_METHOD_TYPES_2(Name, RetType, T1, T2, TypesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_GENERIC_METHOD_TYPES_EX(Name, RetType, (typename T1, typename T2), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get()), TypesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_GENERIC_METHOD_TYPES_3(Name, RetType, T1, T2, T3, TypesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_GENERIC_METHOD_TYPES_EX(Name, RetType, (typename T1, typename T2, typename T3), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get(), BNM::Detail::TypeClassResolver<T3>::Get()), TypesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_STATIC_GENERIC_METHOD_TYPES_1(Name, RetType, T1, TypesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_STATIC_GENERIC_METHOD_TYPES_EX(Name, RetType, (typename T1), \
        (BNM::Detail::TypeClassResolver<T1>::Get()), TypesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_STATIC_GENERIC_METHOD_TYPES_2(Name, RetType, T1, T2, TypesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_STATIC_GENERIC_METHOD_TYPES_EX(Name, RetType, (typename T1, typename T2), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get()), TypesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_STATIC_GENERIC_METHOD_TYPES_3(Name, RetType, T1, T2, T3, TypesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_STATIC_GENERIC_METHOD_TYPES_EX(Name, RetType, (typename T1, typename T2, typename T3), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get(), BNM::Detail::TypeClassResolver<T3>::Get()), TypesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_GENERIC_METHOD_NAMES_1(Name, RetType, T1, NamesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_GENERIC_METHOD_NAMES_EX(Name, RetType, (typename T1), \
        (BNM::Detail::TypeClassResolver<T1>::Get()), NamesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_GENERIC_METHOD_NAMES_2(Name, RetType, T1, T2, NamesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_GENERIC_METHOD_NAMES_EX(Name, RetType, (typename T1, typename T2), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get()), NamesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_GENERIC_METHOD_NAMES_3(Name, RetType, T1, T2, T3, NamesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_GENERIC_METHOD_NAMES_EX(Name, RetType, (typename T1, typename T2, typename T3), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get(), BNM::Detail::TypeClassResolver<T3>::Get()), NamesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_STATIC_GENERIC_METHOD_NAMES_1(Name, RetType, T1, NamesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_STATIC_GENERIC_METHOD_NAMES_EX(Name, RetType, (typename T1), \
        (BNM::Detail::TypeClassResolver<T1>::Get()), NamesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_STATIC_GENERIC_METHOD_NAMES_2(Name, RetType, T1, T2, NamesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_STATIC_GENERIC_METHOD_NAMES_EX(Name, RetType, (typename T1, typename T2), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get()), NamesList, ParamsDecl, ParamsCall)

#define BNM_BINDING_STATIC_GENERIC_METHOD_NAMES_3(Name, RetType, T1, T2, T3, NamesList, ParamsDecl, ParamsCall) \
    BNM_BINDING_STATIC_GENERIC_METHOD_NAMES_EX(Name, RetType, (typename T1, typename T2, typename T3), \
        (BNM::Detail::TypeClassResolver<T1>::Get(), BNM::Detail::TypeClassResolver<T2>::Get(), BNM::Detail::TypeClassResolver<T3>::Get()), NamesList, ParamsDecl, ParamsCall)
