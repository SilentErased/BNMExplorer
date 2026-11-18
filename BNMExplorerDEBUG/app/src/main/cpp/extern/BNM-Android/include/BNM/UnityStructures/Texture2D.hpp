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
#include "Color.hpp"
#include "Rect.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Class for 2D texture handling, pixel reading/writing, and image encoding.
    */
    struct Texture2D : public Texture {

        /**
            @brief Creates a new Texture2D instance.
            @param width Width of texture in pixels.
            @param height Height of texture in pixels.
            @param textureFormat TextureFormat enum integer (default RGBA32 = 4 or ARGB32 = 5).
            @param mipChain Whether to generate mipmaps.
            @return Pointer to new Texture2D instance.
        */
        static inline Texture2D *Create(int width, int height, int textureFormat = 4, bool mipChain = false) {
            auto cls = BNM::Defaults::Get<Texture2D>().ToClass();
            auto instance = (Texture2D *) cls.CreateNewInstance();
            if (!instance) return nullptr;
            static auto ctor4 = cls.GetMethod(BNM_OBFUSCATE(".ctor"), 4).cast<void>();
            if (ctor4.IsValid()) {
                ctor4[(void *)instance](width, height, textureFormat, mipChain);
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
            @brief Gets a small white texture (built-in).
            @return Pointer to white texture.
        */
        static inline Texture2D *GetWhiteTexture() {
            static auto method = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_whiteTexture"), 0).cast<Texture2D *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Gets a small black texture (built-in).
            @return Pointer to black texture.
        */
        static inline Texture2D *GetBlackTexture() {
            static auto method = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_blackTexture"), 0).cast<Texture2D *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Gets a small red texture (built-in).
            @return Pointer to red texture.
        */
        static inline Texture2D *GetRedTexture() {
            static auto method = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_redTexture"), 0).cast<Texture2D *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Returns pixel color at coordinates (x, y).
            @param x Pixel X coordinate.
            @param y Pixel Y coordinate.
            @return Color of pixel.
        */
        inline Structures::Unity::Color GetPixel(int x, int y) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("GetPixel"), 2).cast<Structures::Unity::Color>();
            return method[(void *)this](x, y);
        }

        /**
            @brief Sets pixel color at coordinates (x, y). Call Apply() to upload to GPU.
            @param x Pixel X coordinate.
            @param y Pixel Y coordinate.
            @param color New color.
        */
        inline void SetPixel(int x, int y, Structures::Unity::Color color) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("SetPixel"), 3).cast<void>();
            method[(void *)this](x, y, color);
        }

        /**
            @brief Returns filtered pixel color at normalized coordinates (u, v).
            @param u Normalized U coordinate (0.0 to 1.0).
            @param v Normalized V coordinate (0.0 to 1.0).
            @return Bilinear interpolated color.
        */
        inline Structures::Unity::Color GetPixelBilinear(float u, float v) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("GetPixelBilinear"), 2).cast<Structures::Unity::Color>();
            return method[(void *)this](u, v);
        }

        /**
            @brief Get a block of pixel colors.
            @return Mono Array of Color structures.
        */
        inline Structures::Mono::Array<Structures::Unity::Color> *GetPixels() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("GetPixels"), 0).cast<Structures::Mono::Array<Structures::Unity::Color> *>();
            return method[(void *)this]();
        }

        /**
            @brief Set a block of pixel colors. Call Apply() to upload to GPU.
            @param colors Array of colors.
        */
        inline void SetPixels(Structures::Mono::Array<Structures::Unity::Color> *colors) {
            if (!IsValid() || !colors) return;
            static auto method = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("SetPixels"), 1).cast<void>();
            method[(void *)this](colors);
        }

        /**
            @brief Actually apply all previous SetPixel and SetPixels changes to GPU.
            @param updateMipmaps When true, recalculates mipmaps.
            @param makeNoLongerReadable When true, marks texture as non-readable to save memory.
        */
        inline void Apply(bool updateMipmaps = true, bool makeNoLongerReadable = false) {
            if (!IsValid()) return;
            static auto method2 = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("Apply"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](updateMipmaps, makeNoLongerReadable);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("Apply"), 1).cast<void>();
            if (method1.IsValid()) {
                method1[(void *)this](updateMipmaps);
                return;
            }
            static auto method0 = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("Apply"), 0).cast<void>();
            if (method0.IsValid()) method0[(void *)this]();
        }

        /**
            @brief Resizes the texture.
            @param width New width.
            @param height New height.
            @return True on success.
        */
        inline bool Resize(int width, int height) {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("Resize"), 2).cast<bool>();
            return method[(void *)this](width, height);
        }

        /**
            @brief Read pixels from screen into the saved texture data.
            @param source Rectangular area of screen to read.
            @param destX Horizontal pixel position in texture to write to.
            @param destY Vertical pixel position in texture to write to.
            @param recalculateMipMaps Whether to recalculate mipmaps.
        */
        inline void ReadPixels(Structures::Unity::Rect source, int destX, int destY, bool recalculateMipMaps = true) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("ReadPixels"), 4).cast<void>();
            method[(void *)this](source, destX, destY, recalculateMipMaps);
        }

        /**
            @brief Encodes this texture into PNG format bytes.
            @note Unity Version Aware: Checks ImageConversion module / Texture2D method.
            @return Mono byte array containing PNG data.
        */
        inline Structures::Mono::Array<uint8_t> *EncodeToPNG() const {
            if (!IsValid()) return nullptr;
            static auto methodDirect = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("EncodeToPNG"), 0).cast<Structures::Mono::Array<uint8_t> *>();
            if (methodDirect.IsValid()) return methodDirect[(void *)this]();
            static auto methodExt = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("ImageConversion")).GetMethod(BNM_OBFUSCATE("EncodeToPNG"), 1).cast<Structures::Mono::Array<uint8_t> *>();
            if (methodExt.IsValid()) return methodExt((void *)this);
            return nullptr;
        }

        /**
            @brief Encodes this texture into JPG format bytes.
            @param quality JPG compression quality (1 to 100).
            @return Mono byte array containing JPG data.
        */
        inline Structures::Mono::Array<uint8_t> *EncodeToJPG(int quality = 75) const {
            if (!IsValid()) return nullptr;
            static auto methodDirect = BNM::Defaults::Get<Texture2D>().ToClass().GetMethod(BNM_OBFUSCATE("EncodeToJPG"), 1).cast<Structures::Mono::Array<uint8_t> *>();
            if (methodDirect.IsValid()) return methodDirect[(void *)this](quality);
            static auto methodExt = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("ImageConversion")).GetMethod(BNM_OBFUSCATE("EncodeToJPG"), 2).cast<Structures::Mono::Array<uint8_t> *>();
            if (methodExt.IsValid()) return methodExt((void *)this, quality);
            return nullptr;
        }
    };
}
