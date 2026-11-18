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
#include "Renderer.hpp"
#include "Mesh.hpp"
#include "Transform.hpp"

namespace BNM::UnityEngine {

    /**
        @brief The Skinned Mesh Renderer is used to render bone-animated, deformed character models.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+). Crucial for character Bone extraction (ESP / Aimbot).
    */
    struct SkinnedMeshRenderer : public Renderer {

        /**
            @brief The bones used to skin the mesh. Crucial for joint tracking, ESP skeleton rendering, and aimbot bone targeting.
            @return Mono Array of Transform pointers representing character skeleton bones.
        */
        inline Structures::Mono::Array<Transform *> *GetBones() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_bones"), 0).cast<Structures::Mono::Array<Transform *> *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the bones used to skin the mesh.
            @param bones Array of Transform bone pointers.
        */
        inline void SetBones(Structures::Mono::Array<Transform *> *bones) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_bones"), 1).cast<void>();
            method[(void *)this]((void *)bones);
        }

        /**
            @brief The Transform that is the root of the bone hierarchy.
            @return Root bone Transform pointer.
        */
        inline Transform *GetRootBone() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_rootBone"), 0).cast<Transform *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the Transform that is the root of the bone hierarchy.
            @param rootBone Root bone Transform pointer.
        */
        inline void SetRootBone(Transform *rootBone) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_rootBone"), 1).cast<void>();
            method[(void *)this]((void *)rootBone);
        }

        /**
            @brief The mesh used for skinning.
            @return Shared Mesh pointer.
        */
        inline Mesh *GetSharedMesh() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_sharedMesh"), 0).cast<Mesh *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the mesh used for skinning.
            @param mesh Shared Mesh pointer.
        */
        inline void SetSharedMesh(Mesh *mesh) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_sharedMesh"), 1).cast<void>();
            method[(void *)this]((void *)mesh);
        }

        /**
            @brief The maximum number of bones that can affect a single vertex (0: Auto, 1: 1 Bone, 2: 2 Bones, 4: 4 Bones).
            @return SkinQuality integer.
        */
        inline int GetQuality() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_quality"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the maximum number of bones affecting a single vertex.
            @param value SkinQuality integer.
        */
        inline void SetQuality(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_quality"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief If enabled, the Skinned Mesh will be updated when offscreen.
            @return True if updating when offscreen.
        */
        inline bool GetUpdateWhenOffscreen() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_updateWhenOffscreen"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether the Skinned Mesh is updated when offscreen.
            @param value True to update when offscreen.
        */
        inline void SetUpdateWhenOffscreen(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_updateWhenOffscreen"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Returns the weight of a BlendShape on this Renderer.
            @param index BlendShape index.
            @return BlendShape weight float (0 to 100).
        */
        inline float GetBlendShapeWeight(int index) const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("GetBlendShapeWeight"), 1).cast<float>();
            return method[(void *)this](index);
        }

        /**
            @brief Sets the weight of a BlendShape on this Renderer.
            @param index BlendShape index.
            @param value BlendShape weight float (0 to 100).
        */
        inline void SetBlendShapeWeight(int index, float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("SetBlendShapeWeight"), 2).cast<void>();
            method[(void *)this](index, value);
        }

        /**
            @brief Creates a snapshot of SkinnedMeshRenderer and stores it in mesh.
            @param mesh Target Mesh to receive deformed geometry.
        */
        inline void BakeMesh(Mesh *mesh) {
            if (!IsValid() || !mesh) return;
            static auto method = BNM::Defaults::Get<SkinnedMeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("BakeMesh"), 1).cast<void>();
            method[(void *)this]((void *)mesh);
        }
    };
}

#endif
