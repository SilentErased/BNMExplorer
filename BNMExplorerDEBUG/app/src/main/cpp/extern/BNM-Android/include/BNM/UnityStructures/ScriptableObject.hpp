#pragma once

#include <string_view>
#include "Object.hpp"

namespace BNM::UnityEngine {
    /**
        @brief UnityEngine.ScriptableObject implementation.
        A class you can derive from if you want to create objects that don't need to be attached to game objects.
    */
    struct ScriptableObject : public Object {
        constexpr ScriptableObject() : Object() {}

        /**
            @brief Creates an instance of a scriptable object with a given type.
            @param type Class/Type descriptor (CompileTimeClass, BNM::Class, Il2CppClass*, Il2CppType*, MonoType*).
            @return ScriptableObject instance pointer, or nullptr on failure.
        */
        static inline ScriptableObject *CreateInstance(CompileTimeClass type) {
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return nullptr;
            static auto method = BNM::Defaults::Get<ScriptableObject>().ToClass().GetMethod(BNM_OBFUSCATE("CreateInstance"), 1).cast<ScriptableObject *>();
            return method(monoType);
        }

        /**
            @brief Type-safe template CreateInstance.
            @tparam T Target ScriptableObject pointer type (e.g. MyCustomSettings*).
            @return Newly created instance casted to type T.
        */
        template<typename T>
        static inline T CreateInstance() {
            using CleanT = std::remove_pointer_t<T>;
            return (T) CreateInstance(BNM::Defaults::Get<CleanT>().ToClass());
        }

        /**
            @brief String-name helper CreateInstance.
            @param className Name of the ScriptableObject class to instantiate.
            @param namespaze Namespace of the class (defaults to "").
            @return Newly created ScriptableObject instance pointer.
        */
        static inline ScriptableObject *CreateInstance(const std::string_view &className, const std::string_view &namespaze = "") {
            return CreateInstance(BNM::Class(namespaze, className));
        }
    };
}
