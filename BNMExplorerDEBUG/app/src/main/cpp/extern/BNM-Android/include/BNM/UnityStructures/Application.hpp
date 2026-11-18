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
        @brief Provides access to application run-time data and utility functions.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Application {
        Application() = delete;

        /**
            @brief Gets the path to the game data folder on the target device.
            @return Mono String containing the data path.
        */
        static inline Structures::Mono::String *GetDataPath() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_dataPath"), 0).cast<Structures::Mono::String *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Gets the path to a persistent data directory where data can be saved across runs.
            @return Mono String containing the persistent data path.
        */
        static inline Structures::Mono::String *GetPersistentDataPath() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_persistentDataPath"), 0).cast<Structures::Mono::String *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Gets the path to the StreamingAssets folder.
            @return Mono String containing the streaming assets path.
        */
        static inline Structures::Mono::String *GetStreamingAssetsPath() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_streamingAssetsPath"), 0).cast<Structures::Mono::String *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Gets the path to a temporary data / cache folder.
            @return Mono String containing temporary cache path.
        */
        static inline Structures::Mono::String *GetTemporaryCachePath() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_temporaryCachePath"), 0).cast<Structures::Mono::String *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Gets the version of the Unity runtime used to build the game.
            @return Mono String of the Unity engine version (e.g. "2021.3.16f1").
        */
        static inline Structures::Mono::String *GetUnityVersion() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_unityVersion"), 0).cast<Structures::Mono::String *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Gets the application version / bundle version.
            @return Mono String of the application version.
        */
        static inline Structures::Mono::String *GetVersion() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_version"), 0).cast<Structures::Mono::String *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Gets the unique application identifier (e.g. "com.Company.Game").
            @note Unity Version Aware: Uses get_identifier on Unity 2017+, falls back to get_bundleIdentifier on Unity 5.6.
            @return Mono String containing the bundle identifier.
        */
        static inline Structures::Mono::String *GetIdentifier() {
            static auto methodNew = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_identifier"), 0).cast<Structures::Mono::String *>();
            if (methodNew.IsValid()) return methodNew();
            static auto methodOld = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_bundleIdentifier"), 0).cast<Structures::Mono::String *>();
            if (methodOld.IsValid()) return methodOld();
            return nullptr;
        }

        /**
            @brief Gets the target frame rate of the application.
            @return Target frames per second (-1 means default / unlimited).
        */
        static inline int GetTargetFrameRate() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_targetFrameRate"), 0).cast<int>();
            if (!method.IsValid()) return -1;
            return method();
        }

        /**
            @brief Sets the target frame rate of the application.
            @param value Desired frame rate (-1 for platform default).
        */
        static inline void SetTargetFrameRate(int value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("set_targetFrameRate"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief Returns the language the user's operating system is running in.
            @return SystemLanguage enum integer value.
        */
        static inline int GetSystemLanguage() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_systemLanguage"), 0).cast<int>();
            if (!method.IsValid()) return 0;
            return method();
        }

        /**
            @brief Returns the platform the application is running on (e.g. Android = 11, iPhonePlayer = 8).
            @return RuntimePlatform enum integer value.
        */
        static inline int GetPlatform() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_platform"), 0).cast<int>();
            if (!method.IsValid()) return 0;
            return method();
        }

        /**
            @brief Returns whether the game is currently playing (in editor or player).
            @return True if playing.
        */
        static inline bool GetIsPlaying() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_isPlaying"), 0).cast<bool>();
            if (!method.IsValid()) return false;
            return method();
        }

        /**
            @brief Returns whether the game is running inside the Unity Editor.
            @return True if inside editor.
        */
        static inline bool GetIsEditor() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_isEditor"), 0).cast<bool>();
            if (!method.IsValid()) return false;
            return method();
        }

        /**
            @brief Returns whether the application currently has OS window/input focus.
            @return True if application is focused.
        */
        static inline bool GetIsFocused() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_isFocused"), 0).cast<bool>();
            if (!method.IsValid()) return false;
            return method();
        }

        /**
            @brief Returns whether the application runs in the background when it loses focus.
            @return True if running in background.
        */
        static inline bool GetRunInBackground() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("get_runInBackground"), 0).cast<bool>();
            if (!method.IsValid()) return false;
            return method();
        }

        /**
            @brief Sets whether the application should continue running when it loses focus.
            @param value True to run in background.
        */
        static inline void SetRunInBackground(bool value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("set_runInBackground"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief Quits the application.
            @note Unity Version Aware: Calls Quit(int) on Unity 2019+, falls back to Quit() on Unity 5.6-2018.
            @param exitCode Process exit code.
        */
        static inline void Quit(int exitCode = 0) {
            static auto methodExit = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("Quit"), 1).cast<void>();
            if (methodExit.IsValid()) {
                methodExit(exitCode);
                return;
            }
            static auto methodVoid = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("Quit"), 0).cast<void>();
            if (methodVoid.IsValid()) methodVoid();
        }

        /**
            @brief Opens the specified URL in the default browser or associated application.
            @param url Target URL to open.
        */
        static inline void OpenURL(const std::string_view &url) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("OpenURL"), 1).cast<void>();
            if (method.IsValid()) method(Structures::Mono::String::Create(url));
        }

        /**
            @brief Opens the specified URL (Mono string overload).
            @param url Target URL as Mono string.
        */
        static inline void OpenURL(Structures::Mono::String *url) {
            if (!url) return;
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Application")).GetMethod(BNM_OBFUSCATE("OpenURL"), 1).cast<void>();
            if (method.IsValid()) method(url);
        }
    };
}
