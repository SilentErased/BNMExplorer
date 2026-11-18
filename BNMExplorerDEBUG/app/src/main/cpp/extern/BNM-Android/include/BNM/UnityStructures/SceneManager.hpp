#pragma once

#include <vector>
#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "AsyncOperation.hpp"
#include "GameObject.hpp"

namespace BNM::UnityEngine::SceneManagement {

    /**
        @brief Run-time data structure for Unity Scene.
    */
    struct Scene {
        int m_Handle{0};

        inline int GetHandle() const { return m_Handle; }

        /**
            @brief Returns the name of the scene.
            @return Scene name Mono String.
        */
        inline Structures::Mono::String *GetName() const {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("Scene")).GetMethod(BNM_OBFUSCATE("get_name"), 0).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method[(void *)this]();
            return nullptr;
        }

        /**
            @brief Returns the relative path of the scene.
            @return Path Mono String.
        */
        inline Structures::Mono::String *GetPath() const {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("Scene")).GetMethod(BNM_OBFUSCATE("get_path"), 0).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method[(void *)this]();
            return nullptr;
        }

        /**
            @brief Returns the index of the scene in the Build Settings.
            @return Build index integer.
        */
        inline int GetBuildIndex() const {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("Scene")).GetMethod(BNM_OBFUSCATE("get_buildIndex"), 0).cast<int>();
            if (method.IsValid()) return method[(void *)this]();
            return -1;
        }

        /**
            @brief Returns true if the scene is loaded.
            @return True if loaded.
        */
        inline bool GetIsLoaded() const {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("Scene")).GetMethod(BNM_OBFUSCATE("get_isLoaded"), 0).cast<bool>();
            if (method.IsValid()) return method[(void *)this]();
            return false;
        }

        /**
            @brief Returns true if this is a valid scene handle.
            @return True if valid.
        */
        inline bool GetIsValid() const {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("Scene")).GetMethod(BNM_OBFUSCATE("IsValid"), 0).cast<bool>();
            if (method.IsValid()) return method[(void *)this]();
            return m_Handle != 0;
        }

        /**
            @brief Alias for GetIsValid().
            @return True if valid.
        */
        inline bool IsValid() const { return GetIsValid(); }

        /**
            @brief Returns true if the scene is modified.
            @return True if dirty.
        */
        inline bool GetIsDirty() const {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("Scene")).GetMethod(BNM_OBFUSCATE("get_isDirty"), 0).cast<bool>();
            if (method.IsValid()) return method[(void *)this]();
            return false;
        }

        /**
            @brief Returns the number of root GameObjects in the scene.
            @return Root count integer.
        */
        inline int GetRootCount() const {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("Scene")).GetMethod(BNM_OBFUSCATE("get_rootCount"), 0).cast<int>();
            if (method.IsValid()) return method[(void *)this]();
            return 0;
        }

        /**
            @brief Returns all the root GameObjects in the Scene.
            @return Mono Array of GameObject pointers.
        */
        inline Structures::Mono::Array<GameObject *> *GetRootGameObjects() const {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("Scene")).GetMethod(BNM_OBFUSCATE("GetRootGameObjects"), 0).cast<Structures::Mono::Array<GameObject *> *>();
            if (method.IsValid()) return method[(void *)this]();
            return nullptr;
        }
    };

    /**
        @brief Scene management at run-time.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct SceneManager {
        SceneManager() = delete;

        /**
            @brief Gets the currently active Scene.
            @return Active Scene struct.
        */
        static inline Scene GetActiveScene() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("GetActiveScene"), 0).cast<Scene>();
            if (method.IsValid()) return method();
            return {};
        }

        /**
            @brief Set the scene to be active.
            @param scene Scene to make active.
            @return True if successful.
        */
        static inline bool SetActiveScene(Scene scene) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("SetActiveScene"), 1).cast<bool>();
            if (method.IsValid()) return method(scene);
            return false;
        }

        /**
            @brief Get the Scene at index in the SceneManager's list of loaded Scenes.
            @param index Index of the loaded scene.
            @return Scene struct.
        */
        static inline Scene GetSceneAt(int index) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("GetSceneAt"), 1).cast<Scene>();
            if (method.IsValid()) return method(index);
            return {};
        }

        /**
            @brief Searches through the Scenes loaded for a Scene with the given name.
            @param name Name of scene to search for.
            @return Scene struct.
        */
        static inline Scene GetSceneByName(const std::string_view &name) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("GetSceneByName"), 1).cast<Scene>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(name));
            return {};
        }

        /**
            @brief Searches through the scenes loaded for a Scene with the given path.
            @param scenePath Path of scene.
            @return Scene struct.
        */
        static inline Scene GetSceneByPath(const std::string_view &scenePath) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("GetSceneByPath"), 1).cast<Scene>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(scenePath));
            return {};
        }

        /**
            @brief Create an empty new Scene at runtime with the given name.
            @param sceneName Name of new scene.
            @return Newly created Scene.
        */
        static inline Scene CreateScene(const std::string_view &sceneName) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("CreateScene"), 1).cast<Scene>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(sceneName));
            return {};
        }

        /**
            @brief Merges the source Scene into the destination Scene.
            @param sourceScene Source Scene.
            @param destinationScene Destination Scene.
        */
        static inline void MergeScenes(Scene sourceScene, Scene destinationScene) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("MergeScenes"), 2).cast<void>();
            if (method.IsValid()) method(sourceScene, destinationScene);
        }

        /**
            @brief Move a GameObject from its current Scene to a new Scene.
            @param go GameObject to move.
            @param scene Target Scene.
        */
        static inline void MoveGameObjectToScene(GameObject *go, Scene scene) {
            if (!go) return;
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("MoveGameObjectToScene"), 2).cast<void>();
            if (method.IsValid()) method((void *)go, scene);
        }

        /**
            @brief Get a Scene struct from a build index.
            @param buildIndex Build index of the scene.
            @return Scene struct.
        */
        static inline Scene GetSceneByBuildIndex(int buildIndex) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("GetSceneByBuildIndex"), 1).cast<Scene>();
            if (method.IsValid()) return method(buildIndex);
            return {};
        }

        /**
            @brief The total number of currently loaded Scenes.
            @return Scene count integer.
        */
        static inline int GetSceneCount() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("get_sceneCount"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 1;
        }

        /**
            @brief Returns all root GameObjects across all active loaded scenes, and optionally DontDestroyOnLoad.
            @param includeDontDestroyOnLoad Whether to include roots from the DontDestroyOnLoad scene.
            @return std::vector of GameObject pointers.
        */
        static inline std::vector<GameObject *> GetAllRootGameObjects(bool includeDontDestroyOnLoad = true) {
            std::vector<GameObject *> allRoots{};
            int count = GetSceneCount();
            for (int i = 0; i < count; ++i) {
                auto scene = GetSceneAt(i);
                if (!scene.IsValid() || !scene.GetIsLoaded()) continue;
                auto rootsArr = scene.GetRootGameObjects();
                if (rootsArr) {
                    for (IL2CPP::il2cpp_array_size_t r = 0; r < rootsArr->capacity; ++r) {
                        auto go = rootsArr->At(r);
                        if (go && go->IsValid()) allRoots.push_back(go);
                    }
                }
            }

            if (includeDontDestroyOnLoad) {
                auto tempGo = GameObject::Create();
                if (tempGo && tempGo->IsValid()) {
                    Object::DontDestroyOnLoad((Object *)tempGo);
                    static auto getSceneMethod = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("get_scene"), 0).cast<Scene>();
                    if (getSceneMethod.IsValid()) {
                        auto ddolScene = getSceneMethod[(void *)tempGo]();
                        if (ddolScene.IsValid()) {
                            auto ddolRoots = ddolScene.GetRootGameObjects();
                            if (ddolRoots) {
                                for (IL2CPP::il2cpp_array_size_t r = 0; r < ddolRoots->capacity; ++r) {
                                    auto go = ddolRoots->At(r);
                                    if (go && go->IsValid() && (void *)go != (void *)tempGo) {
                                        allRoots.push_back(go);
                                    }
                                }
                            }
                        }
                    }
                    Object::Destroy((Object *)tempGo);
                }
            }

            return allRoots;
        }

        /**
            @brief Number of Scenes in Build Settings.
            @return Scene count in build settings integer.
        */
        static inline int GetSceneCountInBuildSettings() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("get_sceneCountInBuildSettings"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Loads the Scene by its name or index in Build Settings.
            @param sceneBuildIndex Index of the scene in Build Settings.
            @param mode LoadSceneMode (0: Single, 1: Additive).
        */
        static inline void LoadScene(int sceneBuildIndex, int mode = 0) {
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("LoadScene"), 2).cast<void>();
            if (method2.IsValid()) {
                method2(sceneBuildIndex, mode);
                return;
            }
            static auto method1 = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("LoadScene"), 1).cast<void>();
            if (method1.IsValid()) method1(sceneBuildIndex);
        }

        /**
            @brief Loads the Scene by its name.
            @param sceneName Name of the scene to load.
            @param mode LoadSceneMode (0: Single, 1: Additive).
        */
        static inline void LoadScene(const std::string_view &sceneName, int mode = 0) {
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("LoadScene"), 2).cast<void>();
            if (method2.IsValid()) {
                method2(Structures::Mono::String::Create(sceneName), mode);
                return;
            }
            static auto method1 = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("LoadScene"), 1).cast<void>();
            if (method1.IsValid()) method1(Structures::Mono::String::Create(sceneName));
        }

        /**
            @brief Loads the Scene asynchronously in the background by its build index.
            @param sceneBuildIndex Index of the scene in Build Settings.
            @param mode LoadSceneMode (0: Single, 1: Additive).
            @return AsyncOperation pointer.
        */
        static inline AsyncOperation *LoadSceneAsync(int sceneBuildIndex, int mode = 0) {
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("LoadSceneAsync"), 2).cast<AsyncOperation *>();
            if (method2.IsValid()) return method2(sceneBuildIndex, mode);
            static auto method1 = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("LoadSceneAsync"), 1).cast<AsyncOperation *>();
            if (method1.IsValid()) return method1(sceneBuildIndex);
            return nullptr;
        }

        /**
            @brief Loads the Scene asynchronously in the background by its name.
            @param sceneName Name of the scene to load.
            @param mode LoadSceneMode (0: Single, 1: Additive).
            @return AsyncOperation pointer.
        */
        static inline AsyncOperation *LoadSceneAsync(const std::string_view &sceneName, int mode = 0) {
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("LoadSceneAsync"), 2).cast<AsyncOperation *>();
            if (method2.IsValid()) return method2(Structures::Mono::String::Create(sceneName), mode);
            static auto method1 = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("LoadSceneAsync"), 1).cast<AsyncOperation *>();
            if (method1.IsValid()) return method1(Structures::Mono::String::Create(sceneName));
            return nullptr;
        }

        /**
            @brief Destroys all GameObjects associated with the given Scene and removes the Scene from the SceneManager.
            @param sceneBuildIndex Index of the scene in Build Settings.
            @return AsyncOperation pointer.
        */
        static inline AsyncOperation *UnloadSceneAsync(int sceneBuildIndex) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("UnloadSceneAsync"), 1).cast<AsyncOperation *>();
            if (method.IsValid()) return method(sceneBuildIndex);
            return nullptr;
        }

        /**
            @brief Destroys all GameObjects associated with the given Scene by name.
            @param sceneName Name of the scene to unload.
            @return AsyncOperation pointer.
        */
        static inline AsyncOperation *UnloadSceneAsync(const std::string_view &sceneName) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("UnloadSceneAsync"), 1).cast<AsyncOperation *>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(sceneName));
            return nullptr;
        }

        /**
            @brief Destroys all GameObjects associated with the given Scene.
            @param scene Scene to unload.
            @return AsyncOperation pointer.
        */
        static inline AsyncOperation *UnloadSceneAsync(Scene scene) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("SceneManager")).GetMethod(BNM_OBFUSCATE("UnloadSceneAsync"), {BNM::Class(BNM_OBFUSCATE("UnityEngine.SceneManagement"), BNM_OBFUSCATE("Scene"))}).cast<AsyncOperation *>();
            if (method.IsValid()) return method(scene);
            return nullptr;
        }
    };
}

namespace BNM::UnityEngine {
    using Scene = SceneManagement::Scene;
    using SceneManager = SceneManagement::SceneManager;
}
