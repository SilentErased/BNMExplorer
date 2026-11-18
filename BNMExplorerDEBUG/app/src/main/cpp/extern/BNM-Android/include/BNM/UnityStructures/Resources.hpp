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
#include "AsyncOperation.hpp"

namespace BNM::UnityEngine {

    /**
        @brief The Resources class allows you to find and access Objects including assets.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Resources {
        Resources() = delete;

        /**
            @brief Loads an asset stored at path in a Resources folder.
            @param path Relative path to the asset in Resources folder.
            @return Object pointer, or nullptr if not found.
        */
        static inline Object *Load(const std::string_view &path) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Resources")).GetMethod(BNM_OBFUSCATE("Load"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<Object *>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(path));
            return nullptr;
        }

        /**
            @brief Loads an asset stored at path of a specific Type.
            @param path Relative path to the asset in Resources folder.
            @param type Type of the object to load.
            @return Object pointer, or nullptr if not found.
        */
        static inline Object *Load(const std::string_view &path, CompileTimeClass type) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Resources")).GetMethod(BNM_OBFUSCATE("Load"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass(), BNM::Defaults::Get<IL2CPP::Il2CppType *>().ToClass()}).cast<Object *>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(path), type.ToIl2CppType());
            return nullptr;
        }

        /**
            @brief Loads an asset stored at path and automatically casts to type T*.
            @tparam T Target Unity Object type.
            @param path Relative path to the asset.
            @return Pointer to T, or nullptr if not found.
        */
        template<typename T>
        static inline auto Load(const std::string_view &path) {
            using CleanT = std::remove_pointer_t<T>;
            return (CleanT *) Load(path, BNM::Defaults::Get<CleanT>());
        }

        /**
            @brief Loads all assets in a folder or file at path in a Resources folder.
            @param path Path to the folder or asset file.
            @return Mono Array of Object pointers.
        */
        static inline Structures::Mono::Array<Object *> *LoadAll(const std::string_view &path) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Resources")).GetMethod(BNM_OBFUSCATE("LoadAll"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<Structures::Mono::Array<Object *> *>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(path));
            return nullptr;
        }

        /**
            @brief Loads all assets of a specific type in a folder or file at path.
            @param path Path to the folder or asset file.
            @param type Type filter.
            @return Mono Array of Object pointers.
        */
        static inline Structures::Mono::Array<Object *> *LoadAll(const std::string_view &path, CompileTimeClass type) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Resources")).GetMethod(BNM_OBFUSCATE("LoadAll"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass(), BNM::Defaults::Get<IL2CPP::Il2CppType *>().ToClass()}).cast<Structures::Mono::Array<Object *> *>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(path), type.ToIl2CppType());
            return nullptr;
        }

        /**
            @brief Loads all assets of type T* in a folder or file at path.
            @tparam T Target Unity Object type.
            @param path Path to the folder or asset file.
            @return Mono Array of T pointers.
        */
        template<typename T>
        static inline auto LoadAll(const std::string_view &path) {
            using CleanT = std::remove_pointer_t<T>;
            return (Structures::Mono::Array<CleanT *> *) LoadAll(path, BNM::Defaults::Get<CleanT>());
        }

        /**
            @brief Asynchronously loads an asset stored at path in a Resources folder.
            @param path Path to asset.
            @return AsyncOperation pointer (ResourceRequest).
        */
        static inline AsyncOperation *LoadAsync(const std::string_view &path) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Resources")).GetMethod(BNM_OBFUSCATE("LoadAsync"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<AsyncOperation *>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(path));
            return nullptr;
        }

        /**
            @brief Asynchronously loads an asset of a specific Type.
            @param path Path to asset.
            @param type Type of asset.
            @return AsyncOperation pointer (ResourceRequest).
        */
        static inline AsyncOperation *LoadAsync(const std::string_view &path, CompileTimeClass type) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Resources")).GetMethod(BNM_OBFUSCATE("LoadAsync"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass(), BNM::Defaults::Get<IL2CPP::Il2CppType *>().ToClass()}).cast<AsyncOperation *>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(path), type.ToIl2CppType());
            return nullptr;
        }

        /**
            @brief Asynchronously loads an asset of type T.
            @tparam T Target Unity Object type.
            @param path Path to asset.
            @return AsyncOperation pointer.
        */
        template<typename T>
        static inline auto LoadAsync(const std::string_view &path) {
            using CleanT = std::remove_pointer_t<T>;
            return LoadAsync(path, BNM::Defaults::Get<CleanT>());
        }

        /**
            @brief Unloads assets that are not used.
            @return AsyncOperation pointer.
        */
        static inline AsyncOperation *UnloadUnusedAssets() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Resources")).GetMethod(BNM_OBFUSCATE("UnloadUnusedAssets"), 0).cast<AsyncOperation *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief Unloads assetToUnload from memory.
            @param assetToUnload Object to unload.
        */
        static inline void UnloadAsset(Object *assetToUnload) {
            if (!assetToUnload) return;
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Resources")).GetMethod(BNM_OBFUSCATE("UnloadAsset"), 1).cast<void>();
            if (method.IsValid()) method((void *)assetToUnload);
        }

        /**
            @brief Returns a list of all Objects of Type type.
            @param type Object type to find.
            @return Mono Array of Object pointers.
        */
        static inline Structures::Mono::Array<Object *> *FindObjectsOfTypeAll(CompileTimeClass type) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Resources")).GetMethod(BNM_OBFUSCATE("FindObjectsOfTypeAll"), {BNM::Defaults::Get<IL2CPP::Il2CppType *>().ToClass()}).cast<Structures::Mono::Array<Object *> *>();
            if (method.IsValid()) return method(type.ToIl2CppType());
            return nullptr;
        }

        /**
            @brief Returns a list of all Objects of type T.
            @tparam T Target Unity Object type.
            @return Mono Array of T pointers.
        */
        template<typename T>
        static inline Structures::Mono::Array<T *> *FindObjectsOfTypeAll() {
            return (Structures::Mono::Array<T *> *) FindObjectsOfTypeAll(BNM::Defaults::Get<T>());
        }
    };
}
