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
#include "Shader.hpp"
#include "Texture.hpp"
#include "Color.hpp"
#include "Vector2.hpp"
#include "Vector4.hpp"
#include "Matrix4x4.hpp"

namespace BNM::UnityEngine {

    /**
        @brief The Material class manages shaders, textures, and properties used in rendering.
    */
    struct Material : public Object {

        /**
            @brief Creates a new Material with the specified Shader.
            @param shader The Shader to assign to the new Material.
            @return Pointer to new Material instance.
        */
        static inline Material *Create(Shader *shader) {
            if (!shader || !shader->IsValid()) return nullptr;
            auto cls = BNM::Defaults::Get<Material>().ToClass();
            auto instance = (Material *) cls.CreateNewInstance();
            if (!instance) return nullptr;
            static auto ctor = cls.GetMethod(BNM_OBFUSCATE(".ctor"), 1).cast<void>();
            if (ctor.IsValid()) ctor[(void *)instance](shader);
            return instance;
        }

        /**
            @brief Creates a copy of the specified Material.
            @param source The source material to clone.
            @return Cloned Material.
        */
        static inline Material *Create(Material *source) {
            if (!source || !source->IsValid()) return nullptr;
            auto cls = BNM::Defaults::Get<Material>().ToClass();
            auto instance = (Material *) cls.CreateNewInstance();
            if (!instance) return nullptr;
            static auto ctor = cls.GetMethod(BNM_OBFUSCATE(".ctor"), 1).cast<void>();
            if (ctor.IsValid()) ctor[(void *)instance](source);
            return instance;
        }

        /**
            @brief Gets the Shader assigned to this Material.
            @return Assigned Shader pointer.
        */
        inline Shader *GetShader() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("get_shader"), 0).cast<Shader *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the Shader assigned to this Material.
            @param shader The Shader to assign.
        */
        inline void SetShader(Shader *shader) {
            if (!IsValid() || !shader) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("set_shader"), 1).cast<void>();
            method[(void *)this](shader);
        }

        /**
            @brief Gets the main color of the Material (usually property "_Color").
            @return Main color.
        */
        inline Structures::Unity::Color GetColor() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("get_color"), 0).cast<Structures::Unity::Color>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the main color of the Material.
            @param value New color.
        */
        inline void SetColor(Structures::Unity::Color value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("set_color"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets a named color value from the material.
            @param name Property name (e.g. "_Color", "_EmissionColor").
            @return Color value.
        */
        inline Structures::Unity::Color GetColor(const std::string_view &name) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("GetColor"), 1).cast<Structures::Unity::Color>();
            return method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Gets a color value by shader property ID.
            @param nameID Shader property ID (obtained via Shader::PropertyToID).
            @return Color value.
        */
        inline Structures::Unity::Color GetColor(int nameID) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("GetColor"), 1).cast<Structures::Unity::Color>();
            return method[(void *)this](nameID);
        }

        /**
            @brief Sets a named color value.
            @param name Property name.
            @param value Color value.
        */
        inline void SetColor(const std::string_view &name, Structures::Unity::Color value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("SetColor"), 2).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(name), value);
        }

        /**
            @brief Sets a color value by shader property ID.
            @param nameID Shader property ID.
            @param value Color value.
        */
        inline void SetColor(int nameID, Structures::Unity::Color value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("SetColor"), 2).cast<void>();
            method[(void *)this](nameID, value);
        }

        /**
            @brief Gets the main Texture of the Material (property "_MainTex").
            @return Main Texture pointer.
        */
        inline Texture *GetMainTexture() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("get_mainTexture"), 0).cast<Texture *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the main Texture of the Material.
            @param texture New texture.
        */
        inline void SetMainTexture(Texture *texture) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("set_mainTexture"), 1).cast<void>();
            method[(void *)this](texture);
        }

        /**
            @brief Gets a named Texture from the material.
            @param name Texture property name.
            @return Texture pointer.
        */
        inline Texture *GetTexture(const std::string_view &name) const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("GetTexture"), 1).cast<Texture *>();
            return method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Gets a Texture by shader property ID.
            @param nameID Shader property ID.
            @return Texture pointer.
        */
        inline Texture *GetTexture(int nameID) const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("GetTexture"), 1).cast<Texture *>();
            return method[(void *)this](nameID);
        }

        /**
            @brief Sets a named Texture on the material.
            @param name Texture property name.
            @param texture Texture pointer.
        */
        inline void SetTexture(const std::string_view &name, Texture *texture) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("SetTexture"), 2).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(name), texture);
        }

        /**
            @brief Sets a Texture by shader property ID.
            @param nameID Shader property ID.
            @param texture Texture pointer.
        */
        inline void SetTexture(int nameID, Texture *texture) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("SetTexture"), 2).cast<void>();
            method[(void *)this](nameID, texture);
        }

        /**
            @brief Gets the main texture offset (UV offset).
            @return Vector2 offset.
        */
        inline Structures::Unity::Vector2 GetMainTextureOffset() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("get_mainTextureOffset"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the main texture offset.
            @param offset Vector2 offset.
        */
        inline void SetMainTextureOffset(Structures::Unity::Vector2 offset) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("set_mainTextureOffset"), 1).cast<void>();
            method[(void *)this](offset);
        }

        /**
            @brief Gets the main texture scale (UV tiling).
            @return Vector2 scale.
        */
        inline Structures::Unity::Vector2 GetMainTextureScale() const {
            if (!IsValid()) return {1.f, 1.f};
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("get_mainTextureScale"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the main texture scale.
            @param scale Vector2 scale.
        */
        inline void SetMainTextureScale(Structures::Unity::Vector2 scale) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("set_mainTextureScale"), 1).cast<void>();
            method[(void *)this](scale);
        }

        /**
            @brief Gets the render queue index of this material.
            @return Render queue integer (e.g. 2000 = Geometry, 3000 = Transparent).
        */
        inline int GetRenderQueue() const {
            if (!IsValid()) return 2000;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("get_renderQueue"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the render queue index of this material.
            @param value Render queue integer.
        */
        inline void SetRenderQueue(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("set_renderQueue"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets a named float property.
            @param name Property name.
            @return Float value.
        */
        inline float GetFloat(const std::string_view &name) const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("GetFloat"), 1).cast<float>();
            return method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Gets a float property by ID.
            @param nameID Shader property ID.
            @return Float value.
        */
        inline float GetFloat(int nameID) const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("GetFloat"), 1).cast<float>();
            return method[(void *)this](nameID);
        }

        /**
            @brief Sets a named float property.
            @param name Property name.
            @param value Float value.
        */
        inline void SetFloat(const std::string_view &name, float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("SetFloat"), 2).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(name), value);
        }

        /**
            @brief Sets a float property by ID.
            @param nameID Shader property ID.
            @param value Float value.
        */
        inline void SetFloat(int nameID, float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("SetFloat"), 2).cast<void>();
            method[(void *)this](nameID, value);
        }

        /**
            @brief Gets a named integer property.
            @param name Property name.
            @return Int value.
        */
        inline int GetInt(const std::string_view &name) const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("GetInt"), 1).cast<int>();
            return method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Sets a named integer property.
            @param name Property name.
            @param value Int value.
        */
        inline void SetInt(const std::string_view &name, int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("SetInt"), 2).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(name), value);
        }

        /**
            @brief Gets a named Vector4 property.
            @param name Property name.
            @return Vector4 value.
        */
        inline Structures::Unity::Vector4 GetVector(const std::string_view &name) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("GetVector"), 1).cast<Structures::Unity::Vector4>();
            return method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Sets a named Vector4 property.
            @param name Property name.
            @param value Vector4 value.
        */
        inline void SetVector(const std::string_view &name, Structures::Unity::Vector4 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("SetVector"), 2).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(name), value);
        }

        /**
            @brief Gets a named Matrix4x4 property.
            @param name Property name.
            @return Matrix4x4 value.
        */
        inline Structures::Unity::Matrix4x4 GetMatrix(const std::string_view &name) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("GetMatrix"), 1).cast<Structures::Unity::Matrix4x4>();
            return method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Sets a named Matrix4x4 property.
            @param name Property name.
            @param value Matrix4x4 value.
        */
        inline void SetMatrix(const std::string_view &name, Structures::Unity::Matrix4x4 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("SetMatrix"), 2).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(name), value);
        }

        /**
            @brief Enables a shader keyword on this material instance.
            @param keyword Name of keyword.
        */
        inline void EnableKeyword(const std::string_view &keyword) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("EnableKeyword"), 1).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(keyword));
        }

        /**
            @brief Disables a shader keyword on this material instance.
            @param keyword Name of keyword.
        */
        inline void DisableKeyword(const std::string_view &keyword) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("DisableKeyword"), 1).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(keyword));
        }

        /**
            @brief Checks whether a shader keyword is enabled on this material.
            @param keyword Name of keyword.
            @return True if keyword enabled.
        */
        inline bool IsKeywordEnabled(const std::string_view &keyword) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("IsKeywordEnabled"), 1).cast<bool>();
            return method[(void *)this](Structures::Mono::String::Create(keyword));
        }

        /**
            @brief Checks if material's shader has property with given name.
            @param name Property name.
            @return True if property exists.
        */
        inline bool HasProperty(const std::string_view &name) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("HasProperty"), 1).cast<bool>();
            return method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Checks if material's shader has property with given ID.
            @param nameID Shader property ID.
            @return True if property exists.
        */
        inline bool HasProperty(int nameID) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Material>().ToClass().GetMethod(BNM_OBFUSCATE("HasProperty"), 1).cast<bool>();
            return method[(void *)this](nameID);
        }
    };
}
