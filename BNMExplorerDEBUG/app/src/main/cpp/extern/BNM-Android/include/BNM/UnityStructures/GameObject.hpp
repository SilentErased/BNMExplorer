#pragma once

#include <vector>
#include <string_view>
#include "Object.hpp"
#include "Component.hpp"
#include "Transform.hpp"

namespace BNM::UnityEngine {
    /**
        @brief UnityEngine.GameObject implementation.
        Base class for all entities in Unity Scenes.
    */
    struct GameObject : public Object {
        constexpr GameObject() : Object() {}

        /**
            @brief Creates a new GameObject instance in the current scene.
            @param name Optional name as a Mono String pointer.
            @return Newly created GameObject pointer.
        */
        static inline GameObject *Create(Structures::Mono::String *name = nullptr) {
            auto cls = BNM::Defaults::Get<GameObject>().ToClass();
            if (name) {
                return (GameObject *) cls.CreateNewObjectParameters(name);
            }
            return (GameObject *) cls.CreateNewObjectParameters();
        }

        /**
            @brief Creates a new GameObject instance with a given name.
            @param name Name as std::string_view.
            @return Newly created GameObject pointer.
        */
        static inline GameObject *Create(const std::string_view &name) {
            return Create(CreateMonoString(name));
        }

        /**
            @brief The Transform attached to this GameObject.
            @return Transform pointer, or nullptr if invalid.
        */
        inline Transform *GetTransform() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("get_transform"), 0).cast<Transform *>();
            return method[(void *)this]();
        }

        /**
            @brief Convenience alias for GetTransform().
            @return Transform pointer, or nullptr if invalid.
        */
        inline Transform *transform() const { return GetTransform(); }

        /**
            @brief Returns all direct child GameObjects.
            @return std::vector of child GameObject pointers.
        */
        inline std::vector<GameObject *> GetChildren() const {
            std::vector<GameObject *> children{};
            auto t = GetTransform();
            if (!t) return children;
            int count = t->GetChildCount();
            children.reserve(count);
            for (int i = 0; i < count; ++i) {
                auto childTransform = t->GetChild(i);
                if (childTransform) {
                    auto childGo = childTransform->GetGameObject();
                    if (childGo) children.push_back(childGo);
                }
            }
            return children;
        }

        /**
            @brief The layer the game object is in. (0 to 31).
            @return Layer index integer.
        */
        inline int GetLayer() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("get_layer"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the layer of the game object.
            @param layer Layer index integer (0 to 31).
        */
        inline void SetLayer(int layer) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("set_layer"), 1).cast<void>();
            method[(void *)this](layer);
        }

        /**
            @brief The local active state of this GameObject (ignoring parents).
            @return True if GameObject is locally active.
        */
        inline bool GetActiveSelf() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("get_activeSelf"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Defines whether the GameObject is active in the Scene (true if activeSelf and all parents are active).
            @return True if active in hierarchy.
        */
        inline bool GetActiveInHierarchy() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("get_activeInHierarchy"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Activates or deactivates the GameObject.
            @param value Set to true to activate, false to deactivate.
        */
        inline void SetActive(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("SetActive"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The tag of this game object.
            @return Tag as Mono String pointer.
        */
        inline Structures::Mono::String *GetTag() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("get_tag"), 0).cast<Structures::Mono::String *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the tag of this game object.
            @param tag New tag as Mono String pointer.
        */
        inline void SetTag(Structures::Mono::String *tag) {
            if (!IsValid() || !tag) return;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("set_tag"), 1).cast<void>();
            method[(void *)this](tag);
        }

        /**
            @brief Sets the tag of this game object using std::string_view.
            @param tag New tag string view.
        */
        inline void SetTag(const std::string_view &tag) { SetTag(CreateMonoString(tag)); }

        /**
            @brief Is this game object tagged with tag?
            @param tag Tag to compare.
            @return True if tags match.
        */
        inline bool CompareTag(Structures::Mono::String *tag) const {
            if (!IsValid() || !tag) return false;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("CompareTag"), 1).cast<bool>();
            return method[(void *)this](tag);
        }

        /**
            @brief Is this game object tagged with tag using string_view?
            @param tag Tag string view to compare.
            @return True if tags match.
        */
        inline bool CompareTag(const std::string_view &tag) const { return CompareTag(CreateMonoString(tag)); }

        // --- Generic GetComponent / AddComponent (3-Tier Support) ---

        /**
            @brief Returns the component of Type type if the game object has one attached.
            @param type Class/Type descriptor (CompileTimeClass, BNM::Class, Il2CppClass*, Il2CppType*, MonoType*).
            @return Component pointer, or nullptr if not attached.
        */
        inline Component *GetComponent(CompileTimeClass type) const {
            if (!IsValid()) return nullptr;
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return nullptr;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("GetComponent"), 1).cast<Component *>();
            return method[(void *)this](monoType);
        }

        /**
            @brief Type-safe template GetComponent.
            @tparam T Component pointer or value type (e.g. Rigidbody*, Collider*).
            @return Found component casted to type T, or nullptr.
        */
        template<typename T>
        inline T GetComponent() const {
            using CleanT = std::remove_pointer_t<T>;
            if constexpr (requires { CleanT::StaticClass(); }) {
                return (T) GetComponent(CleanT::StaticClass());
            } else {
                return (T) GetComponent(BNM::Defaults::Get<CleanT>().ToClass());
            }
        }

        /**
            @brief String-name helper GetComponent.
            @param name Class name of the component.
            @param namespaze Namespace of the class (defaults to "").
            @return Component pointer, or nullptr.
        */
        inline Component *GetComponent(const std::string_view &name, const std::string_view &namespaze = "") const {
            return GetComponent(BNM::Class(namespaze, name));
        }

        /**
            @brief Gets the component of the specified type, if it exists.
            Compatible across Unity versions (calls TryGetComponent on Unity 2019.2+, falls back to GetComponent on older versions).
            @param type Class/Type descriptor (CompileTimeClass, BNM::Class, Il2CppClass*, Il2CppType*, MonoType*).
            @param component Output component pointer.
            @return True if component is found and retrieved.
        */
        inline bool TryGetComponent(CompileTimeClass type, Component *&component) const {
            component = nullptr;
            if (!IsValid()) return false;
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return false;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("TryGetComponent"), 2).cast<bool>();
            if (method.IsValid()) {
                return method[(void *)this](monoType, &component);
            }
            component = GetComponent(type);
            return component != nullptr;
        }

        /**
            @brief Type-safe template TryGetComponent.
            @tparam T Component pointer type (e.g. Rigidbody*, Collider*).
            @param component Output component pointer of type T.
            @return True if component is found.
        */
        template<typename T>
        inline bool TryGetComponent(T &component) const {
            using CleanT = std::remove_pointer_t<T>;
            Component *comp = nullptr;
            bool result = false;
            if constexpr (requires { CleanT::StaticClass(); }) {
                result = TryGetComponent(CleanT::StaticClass(), comp);
            } else {
                result = TryGetComponent(BNM::Defaults::Get<CleanT>().ToClass(), comp);
            }
            component = (T) comp;
            return result;
        }

        /**
            @brief String-name helper TryGetComponent.
            @param name Class name of the component.
            @param component Output component pointer.
            @param namespaze Namespace of the class (defaults to "").
            @return True if component is found.
        */
        inline bool TryGetComponent(const std::string_view &name, Component *&component, const std::string_view &namespaze = "") const {
            return TryGetComponent(BNM::Class(namespaze, name), component);
        }

        /**
            @brief Returns the component of Type type in the GameObject or any of its children using depth-first search.
            @param type Class/Type descriptor.
            @param includeInactive Whether to include inactive children.
            @return Component pointer, or nullptr.
        */
        inline Component *GetComponentInChildren(CompileTimeClass type, bool includeInactive = false) const {
            if (!IsValid()) return nullptr;
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return nullptr;
            static auto method2 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("GetComponentInChildren"), 2).cast<Component *>();
            if (method2.IsValid()) return method2[(void *)this](monoType, includeInactive);
            static auto method1 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("GetComponentInChildren"), 1).cast<Component *>();
            return method1[(void *)this](monoType);
        }

        /**
            @brief Type-safe template GetComponentInChildren.
            @tparam T Target component pointer type.
            @param includeInactive Whether to search inactive children.
            @return Found component casted to type T, or nullptr.
        */
        template<typename T>
        inline T GetComponentInChildren(bool includeInactive = false) const {
            using CleanT = std::remove_pointer_t<T>;
            if constexpr (requires { CleanT::StaticClass(); }) {
                return (T) GetComponentInChildren(CleanT::StaticClass(), includeInactive);
            } else {
                return (T) GetComponentInChildren(BNM::Defaults::Get<CleanT>().ToClass(), includeInactive);
            }
        }

        /**
            @brief String-name helper GetComponentInChildren.
            @param name Class name to search.
            @param namespaze Namespace of the class (defaults to "").
            @param includeInactive Whether to search inactive children.
            @return Component pointer, or nullptr.
        */
        inline Component *GetComponentInChildren(const std::string_view &name, const std::string_view &namespaze = "", bool includeInactive = false) const {
            return GetComponentInChildren(BNM::Class(namespaze, name), includeInactive);
        }

        /**
            @brief Returns the component of Type type in the GameObject or any of its parents.
            @param type Class/Type descriptor.
            @param includeInactive Whether to search inactive parents.
            @return Component pointer, or nullptr.
        */
        inline Component *GetComponentInParent(CompileTimeClass type, bool includeInactive = false) const {
            if (!IsValid()) return nullptr;
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return nullptr;
            static auto method2 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("GetComponentInParent"), 2).cast<Component *>();
            if (method2.IsValid()) return method2[(void *)this](monoType, includeInactive);
            static auto method1 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("GetComponentInParent"), 1).cast<Component *>();
            return method1[(void *)this](monoType);
        }

        /**
            @brief Type-safe template GetComponentInParent.
            @tparam T Target component pointer type.
            @param includeInactive Whether to search inactive parents.
            @return Found component casted to type T, or nullptr.
        */
        template<typename T>
        inline T GetComponentInParent(bool includeInactive = false) const {
            using CleanT = std::remove_pointer_t<T>;
            if constexpr (requires { CleanT::StaticClass(); }) {
                return (T) GetComponentInParent(CleanT::StaticClass(), includeInactive);
            } else {
                return (T) GetComponentInParent(BNM::Defaults::Get<CleanT>().ToClass(), includeInactive);
            }
        }

        /**
            @brief String-name helper GetComponentInParent.
            @param name Class name to search.
            @param namespaze Namespace of the class (defaults to "").
            @param includeInactive Whether to search inactive parents.
            @return Component pointer, or nullptr.
        */
        inline Component *GetComponentInParent(const std::string_view &name, const std::string_view &namespaze = "", bool includeInactive = false) const {
            return GetComponentInParent(BNM::Class(namespaze, name), includeInactive);
        }

        /**
            @brief Returns all components of Type type in the GameObject.
            @param type Class/Type descriptor.
            @return Mono Array of Component pointers.
        */
        inline Structures::Mono::Array<Component *> *GetComponents(CompileTimeClass type) const {
            if (!IsValid()) return nullptr;
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return nullptr;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("GetComponents"), 1).cast<Structures::Mono::Array<Component *> *>();
            return method[(void *)this](monoType);
        }

        /**
            @brief Type-safe template GetComponents.
            @tparam T Target component pointer type.
            @return Typed Mono Array of components of type T.
        */
        template<typename T>
        inline Structures::Mono::Array<T> *GetComponents() const {
            using CleanT = std::remove_pointer_t<T>;
            if constexpr (requires { CleanT::StaticClass(); }) {
                return (Structures::Mono::Array<T> *) GetComponents(CleanT::StaticClass());
            } else {
                return (Structures::Mono::Array<T> *) GetComponents(BNM::Defaults::Get<CleanT>().ToClass());
            }
        }

        /**
            @brief String-name helper GetComponents.
            @param name Class name of components.
            @param namespaze Namespace of the class (defaults to "").
            @return Mono Array of Component pointers.
        */
        inline Structures::Mono::Array<Component *> *GetComponents(const std::string_view &name, const std::string_view &namespaze = "") const {
            return GetComponents(BNM::Class(namespaze, name));
        }

        /**
            @brief Returns all components of Type type in the GameObject or any of its children.
            @param type Class/Type descriptor.
            @param includeInactive Whether to search inactive children.
            @return Mono Array of Component pointers.
        */
        inline Structures::Mono::Array<Component *> *GetComponentsInChildren(CompileTimeClass type, bool includeInactive = false) const {
            if (!IsValid()) return nullptr;
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return nullptr;
            static auto method2 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("GetComponentsInChildren"), 2).cast<Structures::Mono::Array<Component *> *>();
            if (method2.IsValid()) return method2[(void *)this](monoType, includeInactive);
            static auto method1 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("GetComponentsInChildren"), 1).cast<Structures::Mono::Array<Component *> *>();
            return method1[(void *)this](monoType);
        }

        /**
            @brief Type-safe template GetComponentsInChildren.
            @tparam T Target component pointer type.
            @param includeInactive Whether to search inactive children.
            @return Typed Mono Array of components of type T.
        */
        template<typename T>
        inline Structures::Mono::Array<T> *GetComponentsInChildren(bool includeInactive = false) const {
            using CleanT = std::remove_pointer_t<T>;
            if constexpr (requires { CleanT::StaticClass(); }) {
                return (Structures::Mono::Array<T> *) GetComponentsInChildren(CleanT::StaticClass(), includeInactive);
            } else {
                return (Structures::Mono::Array<T> *) GetComponentsInChildren(BNM::Defaults::Get<CleanT>().ToClass(), includeInactive);
            }
        }

        /**
            @brief String-name helper GetComponentsInChildren.
            @param name Class name to search.
            @param namespaze Namespace of the class (defaults to "").
            @param includeInactive Whether to search inactive children.
            @return Mono Array of Component pointers.
        */
        inline Structures::Mono::Array<Component *> *GetComponentsInChildren(const std::string_view &name, const std::string_view &namespaze = "", bool includeInactive = false) const {
            return GetComponentsInChildren(BNM::Class(namespaze, name), includeInactive);
        }

        /**
            @brief Returns all components of Type type in the GameObject or any of its parents.
            @param type Class/Type descriptor.
            @param includeInactive Whether to search inactive parents.
            @return Mono Array of Component pointers.
        */
        inline Structures::Mono::Array<Component *> *GetComponentsInParent(CompileTimeClass type, bool includeInactive = false) const {
            if (!IsValid()) return nullptr;
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return nullptr;
            static auto method2 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("GetComponentsInParent"), 2).cast<Structures::Mono::Array<Component *> *>();
            if (method2.IsValid()) return method2[(void *)this](monoType, includeInactive);
            static auto method1 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("GetComponentsInParent"), 1).cast<Structures::Mono::Array<Component *> *>();
            return method1[(void *)this](monoType);
        }

        /**
            @brief Type-safe template GetComponentsInParent.
            @tparam T Target component pointer type.
            @param includeInactive Whether to search inactive parents.
            @return Typed Mono Array of components of type T.
        */
        template<typename T>
        inline Structures::Mono::Array<T> *GetComponentsInParent(bool includeInactive = false) const {
            using CleanT = std::remove_pointer_t<T>;
            if constexpr (requires { CleanT::StaticClass(); }) {
                return (Structures::Mono::Array<T> *) GetComponentsInParent(CleanT::StaticClass(), includeInactive);
            } else {
                return (Structures::Mono::Array<T> *) GetComponentsInParent(BNM::Defaults::Get<CleanT>().ToClass(), includeInactive);
            }
        }

        /**
            @brief String-name helper GetComponentsInParent.
            @param name Class name to search.
            @param namespaze Namespace of the class (defaults to "").
            @param includeInactive Whether to search inactive parents.
            @return Mono Array of Component pointers.
        */
        inline Structures::Mono::Array<Component *> *GetComponentsInParent(const std::string_view &name, const std::string_view &namespaze = "", bool includeInactive = false) const {
            return GetComponentsInParent(BNM::Class(namespaze, name), includeInactive);
        }

        /**
            @brief Adds a component of Type type to the game object.
            @param type Class/Type descriptor.
            @return Newly attached Component pointer.
        */
        inline Component *AddComponent(CompileTimeClass type) {
            if (!IsValid()) return nullptr;
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return nullptr;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("AddComponent"), 1).cast<Component *>();
            return method[(void *)this](monoType);
        }

        /**
            @brief Type-safe template AddComponent.
            @tparam T Component pointer type (e.g. BoxCollider*, AudioSource*).
            @return Newly attached Component casted to type T.
        */
        template<typename T>
        inline T AddComponent() {
            using CleanT = std::remove_pointer_t<T>;
            if constexpr (requires { CleanT::StaticClass(); }) {
                return (T) AddComponent(CleanT::StaticClass());
            } else {
                return (T) AddComponent(BNM::Defaults::Get<CleanT>().ToClass());
            }
        }

        /**
            @brief String-name helper AddComponent.
            @param name Class name of the component to add.
            @param namespaze Namespace of the class (defaults to "").
            @return Newly attached Component pointer.
        */
        inline Component *AddComponent(const std::string_view &name, const std::string_view &namespaze = "") {
            return AddComponent(BNM::Class(namespaze, name));
        }

        // --- SendMessage & BroadcastMessage ---

        /**
            @brief Calls the method named methodName on every MonoBehaviour in this game object.
            @param methodName Method name to invoke as a Mono String pointer.
            @param value Optional argument object.
        */
        inline void SendMessage(Structures::Mono::String *methodName, IL2CPP::Il2CppObject *value = nullptr) const {
            if (!IsValid() || !methodName) return;
            if (value) {
                static auto method2 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("SendMessage"), 2).cast<void>();
                method2[(void *)this](methodName, value);
            } else {
                static auto method1 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("SendMessage"), 1).cast<void>();
                method1[(void *)this](methodName);
            }
        }

        /**
            @brief Calls the method named methodName on every MonoBehaviour in this game object using std::string_view.
            @param methodName Method name to invoke.
            @param value Optional argument object.
        */
        inline void SendMessage(const std::string_view &methodName, IL2CPP::Il2CppObject *value = nullptr) const {
            SendMessage(CreateMonoString(methodName), value);
        }

        /**
            @brief Calls the method named methodName on every MonoBehaviour in this game object or any of its children.
            @param methodName Method name to invoke as a Mono String pointer.
            @param value Optional argument object.
        */
        inline void BroadcastMessage(Structures::Mono::String *methodName, IL2CPP::Il2CppObject *value = nullptr) const {
            if (!IsValid() || !methodName) return;
            if (value) {
                static auto method2 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("BroadcastMessage"), 2).cast<void>();
                method2[(void *)this](methodName, value);
            } else {
                static auto method1 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("BroadcastMessage"), 1).cast<void>();
                method1[(void *)this](methodName);
            }
        }

        /**
            @brief Calls the method named methodName on every MonoBehaviour in this game object or any of its children using std::string_view.
            @param methodName Method name to invoke.
            @param value Optional argument object.
        */
        inline void BroadcastMessage(const std::string_view &methodName, IL2CPP::Il2CppObject *value = nullptr) const {
            BroadcastMessage(CreateMonoString(methodName), value);
        }

        /**
            @brief Calls the method named methodName on every MonoBehaviour in this game object and on every ancestor of the behaviour.
            @param methodName Method name to invoke as a Mono String pointer.
            @param value Optional argument object.
        */
        inline void SendMessageUpwards(Structures::Mono::String *methodName, IL2CPP::Il2CppObject *value = nullptr) const {
            if (!IsValid() || !methodName) return;
            if (value) {
                static auto method2 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("SendMessageUpwards"), 2).cast<void>();
                method2[(void *)this](methodName, value);
            } else {
                static auto method1 = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("SendMessageUpwards"), 1).cast<void>();
                method1[(void *)this](methodName);
            }
        }

        /**
            @brief Calls the method named methodName on every MonoBehaviour in this game object and on every ancestor using std::string_view.
            @param methodName Method name to invoke.
            @param value Optional argument object.
        */
        inline void SendMessageUpwards(const std::string_view &methodName, IL2CPP::Il2CppObject *value = nullptr) const {
            SendMessageUpwards(CreateMonoString(methodName), value);
        }

        // --- Static Finder Methods ---

        /**
            @brief Finds a GameObject by name and returns it.
            @param name GameObject name as a Mono String pointer.
            @return GameObject pointer, or nullptr if not found.
        */
        static inline GameObject *Find(Structures::Mono::String *name) {
            if (!name) return nullptr;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("Find"), 1).cast<GameObject *>();
            return method(name);
        }

        /**
            @brief Finds a GameObject by name using std::string_view and returns it.
            @param name GameObject name string view.
            @return GameObject pointer, or nullptr if not found.
        */
        static inline GameObject *Find(const std::string_view &name) {
            return Find(CreateMonoString(name));
        }

        /**
            @brief Returns one active GameObject tagged tag. Returns null if no GameObject was found.
            @param tag Tag as a Mono String pointer.
            @return GameObject pointer, or nullptr if not found.
        */
        static inline GameObject *FindWithTag(Structures::Mono::String *tag) {
            if (!tag) return nullptr;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("FindWithTag"), 1).cast<GameObject *>();
            return method(tag);
        }

        /**
            @brief Returns one active GameObject tagged tag using std::string_view.
            @param tag Tag string view.
            @return GameObject pointer, or nullptr if not found.
        */
        static inline GameObject *FindWithTag(const std::string_view &tag) {
            return FindWithTag(CreateMonoString(tag));
        }

        /**
            @brief Returns an array of active GameObjects tagged tag. Returns empty array if no GameObject was found.
            @param tag Tag as a Mono String pointer.
            @return Mono Array of GameObject pointers.
        */
        static inline Structures::Mono::Array<GameObject *> *FindGameObjectsWithTag(Structures::Mono::String *tag) {
            if (!tag) return nullptr;
            static auto method = BNM::Defaults::Get<GameObject>().ToClass().GetMethod(BNM_OBFUSCATE("FindGameObjectsWithTag"), 1).cast<Structures::Mono::Array<GameObject *> *>();
            return method(tag);
        }

        /**
            @brief Returns an array of active GameObjects tagged tag using std::string_view.
            @param tag Tag string view.
            @return Mono Array of GameObject pointers.
        */
        static inline Structures::Mono::Array<GameObject *> *FindGameObjectsWithTag(const std::string_view &tag) {
            return FindGameObjectsWithTag(CreateMonoString(tag));
        }
    };
}
