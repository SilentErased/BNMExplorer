#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"

namespace BNM::UnityEngine {

    /**
        @brief The interface to get time information from Unity.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Time {
        Time() = delete;

        /**
            @brief The interval in seconds from the last frame to the current one.
            @return Delta time in seconds.
        */
        static inline float GetDeltaTime() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_deltaTime"), 0).cast<float>();
            if (method.IsValid()) return method();
            return 0.0f;
        }

        /**
            @brief The interval in seconds at which physics and other fixed frame rate updates are performed.
            @return Fixed delta time in seconds.
        */
        static inline float GetFixedDeltaTime() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_fixedDeltaTime"), 0).cast<float>();
            if (method.IsValid()) return method();
            return 0.02f;
        }

        /**
            @brief Sets the interval in seconds at which physics and fixed frame rate updates are performed.
            @param value Fixed delta time in seconds.
        */
        static inline void SetFixedDeltaTime(float value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("set_fixedDeltaTime"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief The time at the beginning of this frame in seconds since the game started.
            @return Time in seconds.
        */
        static inline float GetTime() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_time"), 0).cast<float>();
            if (method.IsValid()) return method();
            return 0.0f;
        }

        /**
            @brief The time this frame has started in seconds since the last level has been loaded.
            @return Time since level load in seconds.
        */
        static inline float GetTimeSinceLevelLoad() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_timeSinceLevelLoad"), 0).cast<float>();
            if (method.IsValid()) return method();
            return 0.0f;
        }

        /**
            @brief The scale at which time passes. Can be used for slow motion or speedhack effects.
            @return Time scale multiplier (1.0 is normal speed).
        */
        static inline float GetTimeScale() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_timeScale"), 0).cast<float>();
            if (method.IsValid()) return method();
            return 1.0f;
        }

        /**
            @brief Sets the scale at which time passes. (Useful for speedhack).
            @param value Time scale multiplier (0.0 stops time, 1.0 is normal speed, 2.0 is 2x speed).
        */
        static inline void SetTimeScale(float value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("set_timeScale"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief The real time in seconds since the game started (unaffected by timeScale).
            @return Realtime since startup in seconds.
        */
        static inline float GetRealtimeSinceStartup() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_realtimeSinceStartup"), 0).cast<float>();
            if (method.IsValid()) return method();
            return 0.0f;
        }

        /**
            @brief The timeScale-independent interval in seconds from the last frame to the current one.
            @return Unscaled delta time in seconds.
        */
        static inline float GetUnscaledDeltaTime() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_unscaledDeltaTime"), 0).cast<float>();
            if (method.IsValid()) return method();
            return GetDeltaTime();
        }

        /**
            @brief The timeScale-independent time at the beginning of this frame in seconds.
            @return Unscaled time in seconds.
        */
        static inline float GetUnscaledTime() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_unscaledTime"), 0).cast<float>();
            if (method.IsValid()) return method();
            return GetTime();
        }

        /**
            @brief The total number of frames that have passed.
            @return Frame count integer.
        */
        static inline int GetFrameCount() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_frameCount"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief The maximum time a frame can take. Physics and other fixed frame rate updates will be performed only up to this limit.
            @return Maximum delta time in seconds.
        */
        static inline float GetMaximumDeltaTime() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_maximumDeltaTime"), 0).cast<float>();
            if (method.IsValid()) return method();
            return 0.3333333f;
        }

        /**
            @brief Sets the maximum time a frame can take.
            @param value Maximum delta time in seconds.
        */
        static inline void SetMaximumDeltaTime(float value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("set_maximumDeltaTime"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief A smoothed out Time.deltaTime.
            @return Smooth delta time in seconds.
        */
        static inline float GetSmoothDeltaTime() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_smoothDeltaTime"), 0).cast<float>();
            if (method.IsValid()) return method();
            return GetDeltaTime();
        }

        /**
            @brief Slows game playback time to allow a fixed frame rate when capturing screenshots/video.
            @return Capture framerate integer.
        */
        static inline int GetCaptureFramerate() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_captureFramerate"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Sets the frame capture rate.
            @param value Capture framerate integer.
        */
        static inline void SetCaptureFramerate(int value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("set_captureFramerate"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief Returns true if the current execution is inside a fixed time step callback (like FixedUpdate).
            @return True if in fixed time step.
        */
        static inline bool GetInFixedTimeStep() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Time")).GetMethod(BNM_OBFUSCATE("get_inFixedTimeStep"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return false;
        }
    };
}
