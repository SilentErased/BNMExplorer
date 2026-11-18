#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"

namespace BNM::UnityEngine {
    struct GameObject;
    struct Component;
    struct Transform;
    struct Behaviour;
    struct MonoBehaviour;
    struct ScriptableObject;

    /**
        @brief Bitmask flags controlling Object destruction, inspector visibility and editing/saving in scenes.
    */
    enum class HideFlags : int {
        None = 0,
        HideInHierarchy = 1,
        HideInInspector = 2,
        DontSaveInEditor = 4,
        NotEditable = 8,
        DontSaveInBuild = 16,
        DontUnloadUnusedAsset = 32,
        DontSave = 52,
        HideAndDontSave = 61
    };

    /**
        @brief UnityEngine.Object implementation.
        Base class for all objects Unity can reference (GameObjects, Components, ScriptableObjects, Assets).
    */
    struct Object : public BNM::IL2CPP::Il2CppObject {
        constexpr Object() : BNM::IL2CPP::Il2CppObject({}) {}
        BNM_INT_PTR m_CachedPtr = 0;

        /**
            @brief Should the object be hidden, saved with the Scene or modifiable by the user?
            @return HideFlags bitmask.
        */
        inline HideFlags GetHideFlags() const {
            if (!IsValid()) return HideFlags::None;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("get_hideFlags"), 0).cast<HideFlags>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the HideFlags of the object.
            @param flags HideFlags bitmask.
        */
        inline void SetHideFlags(HideFlags flags) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("set_hideFlags"), 1).cast<void>();
            method[(void *)this](flags);
        }

        /**
            @brief Check if the Unity object is valid, non-null, and alive in the Unity engine.
            @attention Always use this method to check for null on Unity objects. Checking pointer alone is not sufficient because destroyed objects may still reside in IL2CPP memory while m_CachedPtr is null.
            @return True if object is valid and its native Unity representation is alive.
        */
        [[nodiscard]] inline bool IsValid() const __attribute__((always_inline)) {
            return CheckForNull(this) && m_CachedPtr;
        }

        /**
            @brief Alias for IsValid().
            @return True if object is valid and alive.
        */
        [[nodiscard]] inline bool Alive() const __attribute__((always_inline)) { return IsValid(); }

        /**
            @brief Explicit boolean conversion operator for validity checking.
            @return True if object is valid and alive.
        */
        inline explicit operator bool() const __attribute__((always_inline)) { return IsValid(); }

        /**
            @brief Check if current object represents the same underlying Unity object as another.
            @param object Comparison pointer.
            @return True if both objects are identical or both are dead/null.
        */
        inline bool Same(const void *object) const { return Same((const Object *)object); }

        /**
            @brief Check if current object represents the same underlying Unity object as another Object.
            @param object Comparison Object pointer.
            @return True if both objects are identical or both are dead/null.
        */
        inline bool Same(const Object *object) const {
            if (!object) return !Alive();
            return (!Alive() && !object->Alive()) || (Alive() && object->Alive() && m_CachedPtr == object->m_CachedPtr);
        }

        /**
            @brief Equality operator for Object pointer comparison.
            @param other Object to compare with.
            @return True if both objects are the same.
        */
        inline bool operator==(const Object *other) const { return Same(other); }

        /**
            @brief Inequality operator for Object pointer comparison.
            @param other Object to compare with.
            @return True if objects are different.
        */
        inline bool operator!=(const Object *other) const { return !Same(other); }

        /**
            @brief Equality operator for Object reference comparison.
            @param other Object reference to compare with.
            @return True if both objects are the same.
        */
        inline bool operator==(const Object &other) const { return Same(&other); }

        /**
            @brief Inequality operator for Object reference comparison.
            @param other Object reference to compare with.
            @return True if objects are different.
        */
        inline bool operator!=(const Object &other) const { return !Same(&other); }

        /**
            @brief Gets the name of the object.
            @return Mono String containing the object's name, or nullptr if object is invalid.
        */
        inline Structures::Mono::String *GetName() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("get_name"), 0).cast<Structures::Mono::String *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the name of the object.
            @param name New name as a Mono String pointer.
        */
        inline void SetName(Structures::Mono::String *name) {
            if (!IsValid() || !name) return;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("set_name"), 1).cast<void>();
            method[(void *)this](name);
        }

        /**
            @brief Sets the name of the object using a C++ string view.
            @param name New name as std::string_view.
        */
        inline void SetName(const std::string_view &name) {
            SetName(CreateMonoString(name));
        }

        /**
            @brief Returns the unique instance ID of the object.
            @return Integer instance ID, or 0 if object is invalid.
        */
        inline int GetInstanceID() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("GetInstanceID"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Returns the name of the object formatted as a string.
            @return Mono String representation of the object, or nullptr if invalid.
        */
        inline Structures::Mono::String *ToString() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("ToString"), 0).cast<Structures::Mono::String *>();
            return method[(void *)this]();
        }

        /**
            @brief Removes a GameObject, Component or asset from the scene.
            @param obj The Object to destroy.
            @param t Optional delay in seconds before destroying the object.
        */
        static inline void Destroy(Object *obj, float t = 0.0f) {
            if (!obj || !obj->IsValid()) return;
            if (t > 0.0f) {
                static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("Destroy"), 2).cast<void>();
                method(obj, t);
            } else {
                static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("Destroy"), 1).cast<void>();
                method(obj);
            }
        }

        /**
            @brief Destroys the object immediately without waiting for the end of the frame.
            @param obj The Object to be destroyed.
            @param allowDestroyingAssets Set to true to allow destroying asset files.
        */
        static inline void DestroyImmediate(Object *obj, bool allowDestroyingAssets = false) {
            if (!obj || !obj->IsValid()) return;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("DestroyImmediate"), 2).cast<void>();
            if (method.IsValid()) {
                method(obj, allowDestroyingAssets);
            } else {
                static auto method1 = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("DestroyImmediate"), 1).cast<void>();
                method1(obj);
            }
        }

        /**
            @brief Prevents the target Object from being destroyed automatically when loading a new Scene.
            @param target The Object to preserve across Scene loads.
        */
        static inline void DontDestroyOnLoad(Object *target) {
            if (!target || !target->IsValid()) return;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("DontDestroyOnLoad"), 1).cast<void>();
            method(target);
        }

        /**
            @brief Clones the object original and returns the cloned instance.
            @param original An existing object to duplicate.
            @return The instantiated clone, or nullptr on failure.
        */
        static inline Object *Instantiate(Object *original) {
            if (!original || !original->IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("Instantiate"), 1).cast<Object *>();
            return method(original);
        }

        /**
            @brief Clones the object original at the specified world position and rotation.
            @param original An existing object to duplicate.
            @param position World position for the new cloned object.
            @param rotation World rotation for the new cloned object.
            @return The instantiated clone, or nullptr on failure.
        */
        static inline Object *Instantiate(Object *original, Structures::Unity::Vector3 position, Structures::Unity::Quaternion rotation) {
            if (!original || !original->IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("Instantiate"), 3).cast<Object *>();
            return method(original, position, rotation);
        }

        /**
            @brief Clones the object original at specified position and rotation, parented to the given Transform.
            @param original An existing object to duplicate.
            @param position Position for the new cloned object.
            @param rotation Rotation for the new cloned object.
            @param parent Parent Transform to attach the cloned object to.
            @return The instantiated clone, or nullptr on failure.
        */
        static inline Object *Instantiate(Object *original, Structures::Unity::Vector3 position, Structures::Unity::Quaternion rotation, Transform *parent) {
            if (!original || !original->IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("Instantiate"), 4).cast<Object *>();
            return method(original, position, rotation, parent);
        }

        /**
            @brief Clones the object original and parents it to the specified Transform.
            @param original An existing object to duplicate.
            @param parent Parent Transform to attach the cloned object to.
            @param instantiateInWorldSpace When true, keeps original world transform; otherwise sets local to parent.
            @return The instantiated clone, or nullptr on failure.
        */
        static inline Object *Instantiate(Object *original, Transform *parent, bool instantiateInWorldSpace = false) {
            if (!original || !original->IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("Instantiate"), 3).cast<Object *>();
            return method(original, parent, instantiateInWorldSpace);
        }

        /**
            @brief Type-safe template Instantiate. Automatically casts the result to the desired pointer type.
            @tparam T Object pointer type (e.g. GameObject*, Transform*, custom Component*).
            @param original Object instance to duplicate.
            @return Casted cloned object.
        */
        template<typename T>
        static inline T Instantiate(T original) {
            return (T) Instantiate((Object *) original);
        }

        /**
            @brief Returns the first active loaded object of the specified Type.
            @param type Class/Type descriptor (CompileTimeClass, BNM::Class, Il2CppClass*, Il2CppType*, MonoType*).
            @param includeInactive Whether to include inactive objects (Unity 2020.1+).
            @return First matching loaded object, or nullptr if none found.
        */
        static inline Object *FindObjectOfType(CompileTimeClass type, bool includeInactive = false) {
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return nullptr;
            if (includeInactive) {
                static auto method2 = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("FindObjectOfType"), 2).cast<Object *>();
                if (method2.IsValid()) return method2(monoType, true);
            }
            static auto method1 = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("FindObjectOfType"), 1).cast<Object *>();
            return method1(monoType);
        }

        /**
            @brief Type-safe template FindObjectOfType.
            @tparam T Target class pointer or value type (e.g. Camera*, Transform*, PlayerController*).
            @param includeInactive Whether to include inactive objects.
            @return First matching loaded object casted to type T, or nullptr if none found.
        */
        template<typename T>
        static inline T FindObjectOfType(bool includeInactive = false) {
            using CleanT = std::remove_pointer_t<T>;
            if constexpr (requires { CleanT::StaticClass(); }) {
                return (T) FindObjectOfType(CleanT::StaticClass(), includeInactive);
            } else {
                return (T) FindObjectOfType(BNM::Defaults::Get<CleanT>().ToClass(), includeInactive);
            }
        }

        /**
            @brief String-name helper FindObjectOfType.
            @param name Class name to look for.
            @param namespaze Namespace of the class (defaults to "").
            @param includeInactive Whether to include inactive objects.
            @return First matching loaded object, or nullptr if none found.
        */
        static inline Object *FindObjectOfType(const std::string_view &name, const std::string_view &namespaze = "", bool includeInactive = false) {
            return FindObjectOfType(BNM::Class(namespaze, name), includeInactive);
        }

        /**
            @brief Gets an array of all loaded objects of the specified Type.
            @param type Class/Type descriptor (CompileTimeClass, BNM::Class, Il2CppClass*, Il2CppType*, MonoType*).
            @param includeInactive Whether to include inactive objects.
            @return Mono Array of Objects found in memory.
        */
        static inline Structures::Mono::Array<Object *> *FindObjectsOfType(CompileTimeClass type, bool includeInactive = false) {
            auto monoType = type.ToClass().GetMonoType();
            if (!monoType) return nullptr;
            if (includeInactive) {
                static auto method2 = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("FindObjectsOfType"), 2).cast<Structures::Mono::Array<Object *> *>();
                if (method2.IsValid()) return method2(monoType, true);
            }
            static auto method1 = BNM::Defaults::Get<Object>().ToClass().GetMethod(BNM_OBFUSCATE("FindObjectsOfType"), 1).cast<Structures::Mono::Array<Object *> *>();
            return method1(monoType);
        }

        /**
            @brief Type-safe template FindObjectsOfType.
            @tparam T Target class pointer or value type (e.g. Camera*, Enemy*).
            @param includeInactive Whether to include inactive objects.
            @return Typed Mono Array of objects of type T.
        */
        template<typename T>
        static inline Structures::Mono::Array<T> *FindObjectsOfType(bool includeInactive = false) {
            using CleanT = std::remove_pointer_t<T>;
            if constexpr (requires { CleanT::StaticClass(); }) {
                return (Structures::Mono::Array<T> *) FindObjectsOfType(CleanT::StaticClass(), includeInactive);
            } else {
                return (Structures::Mono::Array<T> *) FindObjectsOfType(BNM::Defaults::Get<CleanT>().ToClass(), includeInactive);
            }
        }

        /**
            @brief String-name helper FindObjectsOfType.
            @param name Class name to look for.
            @param namespaze Namespace of the class (defaults to "").
            @param includeInactive Whether to include inactive objects.
            @return Mono Array of Objects matching the class name.
        */
        static inline Structures::Mono::Array<Object *> *FindObjectsOfType(const std::string_view &name, const std::string_view &namespaze = "", bool includeInactive = false) {
            return FindObjectsOfType(BNM::Class(namespaze, name), includeInactive);
        }
    };

    /**
        @brief Global helper alias for checking if a Unity Object is valid and alive.
        @tparam T Pointer type convertible to UnityEngine::Object*.
        @param o Unity Object pointer to check.
        @return True if object is alive and valid.
    */
    template <typename T>
    inline bool IsUnityObjectAlive(T o) {
        return ((UnityEngine::Object *)o)->Alive();
    }

    /**
        @brief Global helper alias for checking if two Unity Objects refer to the same native instance.
        @tparam T1 First Object pointer type.
        @tparam T2 Second Object pointer type.
        @param o1 First Unity Object.
        @param o2 Second Unity Object.
        @return True if both objects represent the same native instance.
    */
    template <typename T1, typename T2>
    inline bool IsSameUnityObject(T1 o1, T2 o2) {
        auto obj1 = (const UnityEngine::Object *)o1;
        auto obj2 = (const UnityEngine::Object *)o2;
        return obj1->Same(obj2);
    }
}
