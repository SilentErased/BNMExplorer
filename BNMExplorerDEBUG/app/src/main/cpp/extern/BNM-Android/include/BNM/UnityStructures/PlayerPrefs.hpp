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
        @brief Stores and accesses player preferences between game sessions.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct PlayerPrefs {
        PlayerPrefs() = delete;

        /**
            @brief Sets the integer value of the preference corresponding to key.
            @param key Preference key name.
            @param value Integer value to store.
        */
        static inline void SetInt(const std::string_view &key, int value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("SetInt"), 2).cast<void>();
            if (method.IsValid()) method(Structures::Mono::String::Create(key), value);
        }

        /**
            @brief Sets the integer value of the preference (Mono string key).
            @param key Preference key name.
            @param value Integer value to store.
        */
        static inline void SetInt(Structures::Mono::String *key, int value) {
            if (!key) return;
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("SetInt"), 2).cast<void>();
            if (method.IsValid()) method(key, value);
        }

        /**
            @brief Returns the value corresponding to key in the preference file if it exists.
            @param key Preference key name.
            @param defaultValue Value returned if key doesn't exist.
            @return Stored integer value or defaultValue.
        */
        static inline int GetInt(const std::string_view &key, int defaultValue = 0) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("GetInt"), 2).cast<int>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(key), defaultValue);
            static auto method1 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("GetInt"), 1).cast<int>();
            if (method1.IsValid()) return method1(Structures::Mono::String::Create(key));
            return defaultValue;
        }

        /**
            @brief Sets the float value of the preference corresponding to key.
            @param key Preference key name.
            @param value Float value to store.
        */
        static inline void SetFloat(const std::string_view &key, float value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("SetFloat"), 2).cast<void>();
            if (method.IsValid()) method(Structures::Mono::String::Create(key), value);
        }

        /**
            @brief Sets the float value of the preference (Mono string key).
            @param key Preference key name.
            @param value Float value to store.
        */
        static inline void SetFloat(Structures::Mono::String *key, float value) {
            if (!key) return;
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("SetFloat"), 2).cast<void>();
            if (method.IsValid()) method(key, value);
        }

        /**
            @brief Returns the float value corresponding to key in the preference file if it exists.
            @param key Preference key name.
            @param defaultValue Value returned if key doesn't exist.
            @return Stored float value or defaultValue.
        */
        static inline float GetFloat(const std::string_view &key, float defaultValue = 0.0f) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("GetFloat"), 2).cast<float>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(key), defaultValue);
            static auto method1 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("GetFloat"), 1).cast<float>();
            if (method1.IsValid()) return method1(Structures::Mono::String::Create(key));
            return defaultValue;
        }

        /**
            @brief Sets the string value of the preference corresponding to key.
            @param key Preference key name.
            @param value String value to store.
        */
        static inline void SetString(const std::string_view &key, const std::string_view &value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("SetString"), 2).cast<void>();
            if (method.IsValid()) method(Structures::Mono::String::Create(key), Structures::Mono::String::Create(value));
        }

        /**
            @brief Sets the string value of the preference (Mono string key and value).
            @param key Preference key name.
            @param value String value to store.
        */
        static inline void SetString(Structures::Mono::String *key, Structures::Mono::String *value) {
            if (!key || !value) return;
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("SetString"), 2).cast<void>();
            if (method.IsValid()) method(key, value);
        }

        /**
            @brief Returns the string value corresponding to key in the preference file if it exists.
            @param key Preference key name.
            @param defaultValue Value returned if key doesn't exist.
            @return Stored string value as Mono String.
        */
        static inline Structures::Mono::String *GetString(const std::string_view &key, const std::string_view &defaultValue = "") {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("GetString"), 2).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(key), Structures::Mono::String::Create(defaultValue));
            static auto method1 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("GetString"), 1).cast<Structures::Mono::String *>();
            if (method1.IsValid()) return method1(Structures::Mono::String::Create(key));
            return Structures::Mono::String::Create(defaultValue);
        }

        /**
            @brief Returns true if key exists in the preferences.
            @param key Preference key name.
            @return True if key exists.
        */
        static inline bool HasKey(const std::string_view &key) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("HasKey"), 1).cast<bool>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(key));
            return false;
        }

        /**
            @brief Removes key and its corresponding value from the preferences.
            @param key Preference key name to delete.
        */
        static inline void DeleteKey(const std::string_view &key) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("DeleteKey"), 1).cast<void>();
            if (method.IsValid()) method(Structures::Mono::String::Create(key));
        }

        /**
            @brief Removes all keys and values from the preferences. Use with caution.
        */
        static inline void DeleteAll() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("DeleteAll"), 0).cast<void>();
            if (method.IsValid()) method();
        }

        /**
            @brief Writes all modified preferences to disk.
        */
        static inline void Save() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("PlayerPrefs")).GetMethod(BNM_OBFUSCATE("Save"), 0).cast<void>();
            if (method.IsValid()) method();
        }
    };
}
