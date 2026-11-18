#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Texture.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Render textures are textures that can be rendered to (e.g. for camera rendering, off-screen buffers).
    */
    struct RenderTexture : public Texture {

        /**
            @brief Creates a new RenderTexture instance.
            @param width Width in pixels.
            @param height Height in pixels.
            @param depthBuffer Depth buffer bits (0, 16 or 24).
            @return Pointer to new RenderTexture instance.
        */
        static inline RenderTexture *Create(int width, int height, int depthBuffer = 24) {
            auto cls = BNM::Defaults::Get<RenderTexture>().ToClass();
            auto instance = (RenderTexture *) cls.CreateNewInstance();
            if (!instance) return nullptr;
            static auto ctor3 = cls.GetMethod(BNM_OBFUSCATE(".ctor"), 3).cast<void>();
            if (ctor3.IsValid()) {
                ctor3[(void *)instance](width, height, depthBuffer);
                return instance;
            }
            static auto ctor2 = cls.GetMethod(BNM_OBFUSCATE(".ctor"), 2).cast<void>();
            if (ctor2.IsValid()) {
                ctor2[(void *)instance](width, height);
                return instance;
            }
            return instance;
        }

        /**
            @brief Gets currently active RenderTexture.
            @return Active RenderTexture or nullptr if rendering to backbuffer.
        */
        static inline RenderTexture *GetActive() {
            static auto method = BNM::Defaults::Get<RenderTexture>().ToClass().GetMethod(BNM_OBFUSCATE("get_active"), 0).cast<RenderTexture *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Sets currently active RenderTexture.
            @param rt Target RenderTexture or nullptr to render to screen backbuffer.
        */
        static inline void SetActive(RenderTexture *rt) {
            static auto method = BNM::Defaults::Get<RenderTexture>().ToClass().GetMethod(BNM_OBFUSCATE("set_active"), 1).cast<void>();
            if (method.IsValid()) method(rt);
        }

        /**
            @brief Gets the precision of the render texture's depth buffer in bits (0, 16, 24, 32).
            @return Depth bits.
        */
        inline int GetDepth() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<RenderTexture>().ToClass().GetMethod(BNM_OBFUSCATE("get_depth"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the precision of the render texture's depth buffer in bits.
            @param value Depth bits (0, 16, 24, 32).
        */
        inline void SetDepth(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RenderTexture>().ToClass().GetMethod(BNM_OBFUSCATE("set_depth"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Allocate a temporary render texture from the GPU pool.
            @param width Width in pixels.
            @param height Height in pixels.
            @param depthBuffer Depth buffer bits (default 0).
            @return Temporary RenderTexture.
        */
        static inline RenderTexture *GetTemporary(int width, int height, int depthBuffer = 0) {
            static auto method3 = BNM::Defaults::Get<RenderTexture>().ToClass().GetMethod(BNM_OBFUSCATE("GetTemporary"), 3).cast<RenderTexture *>();
            if (method3.IsValid()) return method3(width, height, depthBuffer);
            static auto method2 = BNM::Defaults::Get<RenderTexture>().ToClass().GetMethod(BNM_OBFUSCATE("GetTemporary"), 2).cast<RenderTexture *>();
            if (method2.IsValid()) return method2(width, height);
            return nullptr;
        }

        /**
            @brief Release a temporary render texture allocated with GetTemporary.
            @param temp Temporary render texture to release.
        */
        static inline void ReleaseTemporary(RenderTexture *temp) {
            if (!temp) return;
            static auto method = BNM::Defaults::Get<RenderTexture>().ToClass().GetMethod(BNM_OBFUSCATE("ReleaseTemporary"), 1).cast<void>();
            if (method.IsValid()) method(temp);
        }

        /**
            @brief Actually creates the RenderTexture on hardware.
            @return True on success.
        */
        inline bool Create() {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<RenderTexture>().ToClass().GetMethod(BNM_OBFUSCATE("Create"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Releases the hardware resources used by the render texture.
        */
        inline void Release() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RenderTexture>().ToClass().GetMethod(BNM_OBFUSCATE("Release"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Is the render texture actually created on GPU?
            @return True if created.
        */
        inline bool IsCreated() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<RenderTexture>().ToClass().GetMethod(BNM_OBFUSCATE("IsCreated"), 0).cast<bool>();
            return method[(void *)this]();
        }
    };
}
