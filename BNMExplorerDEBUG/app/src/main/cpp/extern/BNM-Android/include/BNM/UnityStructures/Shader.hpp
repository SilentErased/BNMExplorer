#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Object.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Shader scripts used for rendering in Unity.
    */
    struct Shader : public Object {
        /**
            @brief Finds a Shader with the given name.
            @param name Name of the shader (e.g. "Standard", "Hidden/Internal-Colored").
            @return Pointer to loaded Shader object or nullptr if not found.
        */
        static inline Shader *Find(const std::string_view &name) {
            static auto method = BNM::Defaults::Get<Shader>().ToClass().GetMethod(BNM_OBFUSCATE("Find"), 1).cast<Shader *>();
            if (!method.IsValid()) return nullptr;
            return method(Structures::Mono::String::Create(name));
        }

        /**
            @brief Finds a Shader with the given Mono string name.
            @param name Mono string shader name.
            @return Pointer to loaded Shader object or nullptr if not found.
        */
        static inline Shader *Find(Structures::Mono::String *name) {
            if (!name) return nullptr;
            static auto method = BNM::Defaults::Get<Shader>().ToClass().GetMethod(BNM_OBFUSCATE("Find"), 1).cast<Shader *>();
            if (!method.IsValid()) return nullptr;
            return method(name);
        }

        /**
            @brief Gets unique property name ID for fast property access in Materials.
            @param name Shader property name (e.g. "_Color", "_MainTex").
            @return Integer property ID.
        */
        static inline int PropertyToID(const std::string_view &name) {
            static auto method = BNM::Defaults::Get<Shader>().ToClass().GetMethod(BNM_OBFUSCATE("PropertyToID"), 1).cast<int>();
            if (!method.IsValid()) return 0;
            return method(Structures::Mono::String::Create(name));
        }

        /**
            @brief Gets unique property name ID for fast property access in Materials (Mono string overload).
            @param name Shader property name.
            @return Integer property ID.
        */
        static inline int PropertyToID(Structures::Mono::String *name) {
            if (!name) return 0;
            static auto method = BNM::Defaults::Get<Shader>().ToClass().GetMethod(BNM_OBFUSCATE("PropertyToID"), 1).cast<int>();
            if (!method.IsValid()) return 0;
            return method(name);
        }

        /**
            @brief Returns true if this shader can run on the current graphics device.
            @return True if supported.
        */
        inline bool GetIsSupported() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Shader>().ToClass().GetMethod(BNM_OBFUSCATE("get_isSupported"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Gets the maximum shader LOD allowed on this device.
            @return LOD level integer.
        */
        inline int GetMaximumLOD() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Shader>().ToClass().GetMethod(BNM_OBFUSCATE("get_maximumLOD"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the maximum shader LOD.
            @param value LOD level integer.
        */
        inline void SetMaximumLOD(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Shader>().ToClass().GetMethod(BNM_OBFUSCATE("set_maximumLOD"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Enables a global shader keyword across all materials.
            @param keyword Name of the keyword.
        */
        static inline void EnableKeyword(const std::string_view &keyword) {
            static auto method = BNM::Defaults::Get<Shader>().ToClass().GetMethod(BNM_OBFUSCATE("EnableKeyword"), 1).cast<void>();
            if (method.IsValid()) method(Structures::Mono::String::Create(keyword));
        }

        /**
            @brief Disables a global shader keyword.
            @param keyword Name of the keyword.
        */
        static inline void DisableKeyword(const std::string_view &keyword) {
            static auto method = BNM::Defaults::Get<Shader>().ToClass().GetMethod(BNM_OBFUSCATE("DisableKeyword"), 1).cast<void>();
            if (method.IsValid()) method(Structures::Mono::String::Create(keyword));
        }
    };
}
