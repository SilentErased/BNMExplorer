#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"

#ifdef BNM_UNITY_RENDERERS

#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Component.hpp"
#include "Material.hpp"
#include "Bounds.hpp"

namespace BNM::UnityEngine {

    /**
        @brief General functionality for all renderers.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Renderer : public Component {

        /**
            @brief Returns the first instantiated Material assigned to the renderer.
            @return Instantiated Material pointer.
        */
        inline Material *GetMaterial() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_material"), 0).cast<Material *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the Material assigned to the renderer.
            @param material Material pointer.
        */
        inline void SetMaterial(Material *material) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_material"), 1).cast<void>();
            method[(void *)this]((void *)material);
        }

        /**
            @brief Returns all the instantiated materials of this object.
            @return Mono Array of Material pointers.
        */
        inline Structures::Mono::Array<Material *> *GetMaterials() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_materials"), 0).cast<Structures::Mono::Array<Material *> *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets all materials of this renderer.
            @param materials Array of Material pointers.
        */
        inline void SetMaterials(Structures::Mono::Array<Material *> *materials) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_materials"), 1).cast<void>();
            method[(void *)this]((void *)materials);
        }

        /**
            @brief The shared material of this object. Modifying it changes material for all objects using it.
            @return Shared Material pointer.
        */
        inline Material *GetSharedMaterial() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_sharedMaterial"), 0).cast<Material *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the shared material of this object.
            @param material Shared Material pointer.
        */
        inline void SetSharedMaterial(Material *material) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_sharedMaterial"), 1).cast<void>();
            method[(void *)this]((void *)material);
        }

        /**
            @brief All the shared materials of this object.
            @return Mono Array of shared Material pointers.
        */
        inline Structures::Mono::Array<Material *> *GetSharedMaterials() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_sharedMaterials"), 0).cast<Structures::Mono::Array<Material *> *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets all shared materials of this renderer.
            @param materials Array of shared Material pointers.
        */
        inline void SetSharedMaterials(Structures::Mono::Array<Material *> *materials) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_sharedMaterials"), 1).cast<void>();
            method[(void *)this]((void *)materials);
        }

        /**
            @brief The bounding box of the renderer in world space.
            @return Bounds structure.
        */
        inline Structures::Unity::Bounds GetBounds() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_bounds"), 0).cast<Structures::Unity::Bounds>();
            return method[(void *)this]();
        }

        /**
            @brief Makes the rendered 3D object visible if enabled.
            @return True if enabled.
        */
        inline bool GetEnabled() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_enabled"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Controls visibility of the renderer.
            @param value True to enable.
        */
        inline void SetEnabled(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_enabled"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Is this renderer visible in any camera?
            @return True if visible.
        */
        inline bool GetIsVisible() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_isVisible"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Does this object cast shadows? (0: Off, 1: On, 2: TwoSided, 3: ShadowsOnly).
            @return ShadowCastingMode integer.
        */
        inline int GetShadowCastingMode() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_shadowCastingMode"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets shadow casting mode.
            @param value ShadowCastingMode integer.
        */
        inline void SetShadowCastingMode(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_shadowCastingMode"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Does this object receive shadows?
            @return True if receives shadows.
        */
        inline bool GetReceiveShadows() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_receiveShadows"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether this object receives shadows.
            @param value True to receive shadows.
        */
        inline void SetReceiveShadows(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_receiveShadows"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Name of the Renderer's sorting layer.
            @return Sorting layer name Mono String.
        */
        inline Structures::Mono::String *GetSortingLayerName() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_sortingLayerName"), 0).cast<Structures::Mono::String *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the Renderer's sorting layer name.
            @param name Sorting layer name.
        */
        inline void SetSortingLayerName(const std::string_view &name) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_sortingLayerName"), 1).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Unique ID of the Renderer's sorting layer.
            @return Sorting layer ID integer.
        */
        inline int GetSortingLayerID() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_sortingLayerID"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets unique ID of the Renderer's sorting layer.
            @param value Sorting layer ID integer.
        */
        inline void SetSortingLayerID(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_sortingLayerID"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Renderer's order within a sorting layer.
            @return Sorting order integer.
        */
        inline int GetSortingOrder() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_sortingOrder"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets Renderer's order within a sorting layer.
            @param value Sorting order integer.
        */
        inline void SetSortingOrder(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Renderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_sortingOrder"), 1).cast<void>();
            method[(void *)this](value);
        }
    };
}

#endif
