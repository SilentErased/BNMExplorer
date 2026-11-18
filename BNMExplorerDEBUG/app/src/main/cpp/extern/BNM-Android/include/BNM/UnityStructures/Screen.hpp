#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Rect.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Provides access to display and screen properties.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Screen {
        Screen() = delete;

        /**
            @brief The current width of the screen window in pixels.
            @return Screen width in pixels.
        */
        static inline int GetWidth() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("get_width"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief The current height of the screen window in pixels.
            @return Screen height in pixels.
        */
        static inline int GetHeight() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("get_height"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief The current DPI of the screen.
            @return Dots per inch float.
        */
        static inline float GetDpi() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("get_dpi"), 0).cast<float>();
            if (method.IsValid()) return method();
            return 0.0f;
        }

        /**
            @brief The current screen orientation.
            @return Orientation enum integer (0 = Unknown, 1 = Portrait, 2 = PortraitUpsideDown, 3 = LandscapeLeft, 4 = LandscapeRight, 5 = AutoRotation).
        */
        static inline int GetOrientation() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("get_orientation"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Sets the screen orientation.
            @param value ScreenOrientation integer enum.
        */
        static inline void SetOrientation(int value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("set_orientation"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief A power saving setting, allowing the screen to dim some time after the last active user interaction.
            @return Sleep timeout integer (-1 = NeverSleep, -2 = SystemSetting).
        */
        static inline int GetSleepTimeout() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("get_sleepTimeout"), 0).cast<int>();
            if (method.IsValid()) return method();
            return -2;
        }

        /**
            @brief Sets the sleep timeout value.
            @param value Sleep timeout (-1 = NeverSleep, -2 = SystemSetting).
        */
        static inline void SetSleepTimeout(int value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("set_sleepTimeout"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief The current screen brightness (mobile only).
            @return Brightness float (0.0 to 1.0).
        */
        static inline float GetBrightness() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("get_brightness"), 0).cast<float>();
            if (method.IsValid()) return method();
            return 1.0f;
        }

        /**
            @brief Sets the screen brightness (mobile only).
            @param value Brightness float (0.0 to 1.0).
        */
        static inline void SetBrightness(float value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("set_brightness"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief Is the game running in full-screen mode?
            @return True if fullscreen.
        */
        static inline bool GetFullScreen() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("get_fullScreen"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return true;
        }

        /**
            @brief Sets whether the game is running in full-screen mode.
            @param value True for fullscreen.
        */
        static inline void SetFullScreen(bool value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("set_fullScreen"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief Returns the safe area of the screen in pixels (Unity 2017.2+).
            @return Safe area Rect. On Unity < 2017.2, returns full screen Rect (0, 0, width, height).
        */
        static inline Structures::Unity::Rect GetSafeArea() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("get_safeArea"), 0).cast<Structures::Unity::Rect>();
            if (method.IsValid()) return method();
            return Structures::Unity::Rect(0.0f, 0.0f, (float)GetWidth(), (float)GetHeight());
        }

        /**
            @brief Returns an array of Rects for cutout areas on the display. (Unity 2019.1+).
            @return Mono Array of Rects, or nullptr on older Unity.
        */
        static inline Structures::Mono::Array<Structures::Unity::Rect> *GetCutouts() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("get_cutouts"), 0).cast<Structures::Mono::Array<Structures::Unity::Rect> *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief Switches the screen resolution.
            @param width Resolution width.
            @param height Resolution height.
            @param fullscreen Fullscreen flag.
            @param preferredRefreshRate Refresh rate in Hz (default 0).
        */
        static inline void SetResolution(int width, int height, bool fullscreen, int preferredRefreshRate = 0) {
            static auto method4 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("SetResolution"), 4).cast<void>();
            if (method4.IsValid()) {
                method4(width, height, fullscreen, preferredRefreshRate);
                return;
            }
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Screen")).GetMethod(BNM_OBFUSCATE("SetResolution"), 3).cast<void>();
            if (method3.IsValid()) method3(width, height, fullscreen);
        }
    };
}
