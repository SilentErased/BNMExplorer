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
        @brief Base class for Texture handling in Unity.
    */
    struct Texture : public Object {
        /**
            @brief Gets the width of the texture in pixels.
            @return Width in pixels.
        */
        inline int GetWidth() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("get_width"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the width of the texture.
            @param value Width in pixels.
        */
        inline void SetWidth(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("set_width"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the height of the texture in pixels.
            @return Height in pixels.
        */
        inline int GetHeight() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("get_height"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the height of the texture.
            @param value Height in pixels.
        */
        inline void SetHeight(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("set_height"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets filtering mode of the Texture (0 = Point, 1 = Bilinear, 2 = Trilinear).
            @return FilterMode integer.
        */
        inline int GetFilterMode() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("get_filterMode"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Gets dimension of the texture (e.g. 2D, 3D, Cube, etc.).
            @return TextureDimension integer.
        */
        inline int GetDimension() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("get_dimension"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets filtering mode of the Texture.
            @param value FilterMode (0 = Point, 1 = Bilinear, 2 = Trilinear).
        */
        inline void SetFilterMode(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("set_filterMode"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets texture wrap mode (0 = Repeat, 1 = Clamp, 2 = Mirror, 3 = MirrorOnce).
            @return TextureWrapMode integer.
        */
        inline int GetWrapMode() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("get_wrapMode"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets texture wrap mode.
            @param value TextureWrapMode integer.
        */
        inline void SetWrapMode(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("set_wrapMode"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets texture mipmap bias.
            @return Mipmap bias float.
        */
        inline float GetMipMapBias() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("get_mipMapBias"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets texture mipmap bias.
            @param value Mipmap bias float.
        */
        inline void SetMipMapBias(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("set_mipMapBias"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets anisotropic filtering level.
            @return Aniso level integer.
        */
        inline int GetAnisoLevel() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("get_anisoLevel"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets anisotropic filtering level.
            @param value Aniso level integer.
        */
        inline void SetAnisoLevel(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("set_anisoLevel"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Retrieve native underlying graphics API texture pointer (IntPtr).
            @return Native pointer handle.
        */
        inline void *GetNativeTexturePtr() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Texture>().ToClass().GetMethod(BNM_OBFUSCATE("GetNativeTexturePtr"), 0).cast<void *>();
            return method[(void *)this]();
        }
    };
}
