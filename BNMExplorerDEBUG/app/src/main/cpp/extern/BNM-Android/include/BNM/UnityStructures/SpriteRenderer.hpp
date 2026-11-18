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
#include "Color.hpp"
#include "Vector2.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Renders a Sprite for 2D graphics.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct SpriteRenderer : public Renderer {

        /**
            @brief The Sprite to render.
            @return Sprite Object pointer.
        */
        inline Object *GetSprite() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_sprite"), 0).cast<Object *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the Sprite to render.
            @param sprite Sprite Object pointer.
        */
        inline void SetSprite(Object *sprite) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_sprite"), 1).cast<void>();
            method[(void *)this]((void *)sprite);
        }

        /**
            @brief Rendering color of the Sprite.
            @return Color structure.
        */
        inline Structures::Unity::Color GetColor() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_color"), 0).cast<Structures::Unity::Color>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the rendering color of the Sprite.
            @param color Color structure.
        */
        inline void SetColor(Structures::Unity::Color color) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_color"), 1).cast<void>();
            method[(void *)this](color);
        }

        /**
            @brief Flips the sprite on the X axis.
            @return True if flipped horizontally.
        */
        inline bool GetFlipX() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_flipX"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets horizontal flip of the sprite.
            @param value True to flip horizontally.
        */
        inline void SetFlipX(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_flipX"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Flips the sprite on the Y axis.
            @return True if flipped vertically.
        */
        inline bool GetFlipY() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_flipY"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets vertical flip of the sprite.
            @param value True to flip vertically.
        */
        inline void SetFlipY(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_flipY"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The current draw mode for the Sprite (0: Simple, 1: Sliced, 2: Tiled).
            @return SpriteDrawMode integer.
        */
        inline int GetDrawMode() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_drawMode"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets draw mode for the Sprite.
            @param value SpriteDrawMode integer.
        */
        inline void SetDrawMode(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_drawMode"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Size of the Sprite in world coordinates when drawMode is Sliced or Tiled.
            @return Vector2 size.
        */
        inline Structures::Unity::Vector2 GetSize() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_size"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets size of the Sprite when drawMode is Sliced or Tiled.
            @param size Vector2 size.
        */
        inline void SetSize(Structures::Unity::Vector2 size) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SpriteRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_size"), 1).cast<void>();
            method[(void *)this](size);
        }
    };
}

#endif
