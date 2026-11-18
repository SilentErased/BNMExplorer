#pragma once

#include <type_traits>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"

namespace BNM {
    /**
        @brief Namespace that holds Unity math and helper structs.

        It contains: Vector2, Vector3, Vector4, Color, Color32, Ray, RaycastHit, RaycastHit2D, Quaternion, Matrix3x3, Matrix4x4, Bounds.
    */
    namespace Structures::Unity {
        struct Vector2;
        struct Vector3;
        struct Vector4;
        struct Color;
        struct Color32;
        struct Ray;
        struct RaycastHit;
        struct RaycastHit2D;
        struct Quaternion;
        struct Matrix3x3;
        struct Matrix4x4;
        struct Bounds;
    }
    namespace Structures::Mono {
        struct String;
        struct decimal;
    }
    namespace UnityEngine {
        struct Object;
        struct Component;
        struct Transform;
        struct GameObject;
        struct Behaviour;
        struct MonoBehaviour;
        struct ScriptableObject;
        struct Application;
        struct Camera;
        struct Light;
        struct Shader;
        struct Material;
        struct Texture;
        struct Texture2D;
        struct RenderTexture;

        // Core Utilities, Input & System
        struct Time;
        struct Screen;
        struct Touch;
        struct Input;
        struct SystemInfo;
        struct PlayerPrefs;

        // Scene Management & Assets
        struct YieldInstruction;
        struct AsyncOperation;
        struct Resources;
        namespace SceneManagement {
            struct Scene;
            struct SceneManager;
        }

#ifdef BNM_UNITY_RENDERERS
        struct Renderer;
        struct Mesh;
        struct MeshRenderer;
        struct SkinnedMeshRenderer;
        struct SpriteRenderer;
#endif

#ifdef BNM_UNITY_AUDIO
        struct AudioClip;
        struct AudioSource;
        struct AudioListener;
#endif

#ifdef BNM_UNITY_ANIMATION
        struct AnimationClip;
        struct Animation;
        struct Animator;
#endif

#ifdef BNM_UNITY_PHYSICS
        struct Collider;
        struct BoxCollider;
        struct SphereCollider;
        struct CapsuleCollider;
        struct MeshCollider;
        struct Rigidbody;
        struct Physics;
#endif

#ifdef BNM_UNITY_PHYSICS2D
        struct Collider2D;
        struct BoxCollider2D;
        struct CircleCollider2D;
        struct Rigidbody2D;
        struct Physics2D;
#endif

#ifdef BNM_UNITY_UI
        namespace UI {
            struct RectTransform;
            struct Canvas;
            struct CanvasScaler;
            struct UIBehaviour;
            struct Graphic;
            struct GraphicRaycaster;
            struct MaskableGraphic;
            struct Text;
            struct Image;
            struct Selectable;
            struct Button;
            struct Slider;
            struct Toggle;
            struct InputField;
            struct EventSystem;
        }
#endif

#ifdef BNM_UNITY_TEXTMESHPRO
        namespace TMPro {
            struct TMP_Text;
            struct TextMeshPro;
            struct TextMeshProUGUI;
            struct TMP_InputField;
        }
#endif
    }
    struct CompileTimeClass;
    struct Class;
}

/**
    @brief Namespace that holds C# primitives.

    It contains: byte, sbyte, ushort, uint, ulong, nint, nuint, decimal.
*/
namespace BNM::Types {
    typedef uint8_t byte;
    typedef int8_t sbyte;
    typedef unsigned short ushort;
    typedef unsigned int uint;
    typedef unsigned long ulong;
    enum nint : intptr_t {};
    enum nuint : uintptr_t {};
    typedef BNM::Structures::Mono::decimal decimal;
}

// NOLINTBEGIN
/**
    @brief Namespace that helps to get references to primitives and common used C# and Unity types.
*/
namespace BNM::Defaults {
    /// @cond
    namespace Internal {
        typedef IL2CPP::Il2CppClass *ClassType;
        extern ClassType Void, Boolean, Byte, SByte, Int16, UInt16, Int32, UInt32, IntPtr, UIntPtr, Int64, UInt64, Single, Double, Decimal, String, Object;
        extern ClassType Vector2, Vector3, Vector4, Color, Color32, Ray, Quaternion, Matrix3x3, Matrix4x4, RaycastHit, RaycastHit2D, Bounds;
        extern ClassType UnityObject, Component, Transform, GameObject, Behaviour, MonoBehaviour, ScriptableObject;
        extern ClassType Application, Camera, Light, Shader, Material, Texture, Texture2D, RenderTexture;
        extern ClassType Time, Screen, Touch, Input, SystemInfo, PlayerPrefs;
        extern ClassType YieldInstruction, AsyncOperation, Resources, Scene, SceneManager;
#ifdef BNM_UNITY_RENDERERS
        extern ClassType Renderer, Mesh, MeshRenderer, SkinnedMeshRenderer, SpriteRenderer;
#endif
#ifdef BNM_UNITY_AUDIO
        extern ClassType AudioClip, AudioSource, AudioListener;
#endif
#ifdef BNM_UNITY_ANIMATION
        extern ClassType AnimationClip, Animation, Animator;
#endif
#ifdef BNM_UNITY_PHYSICS
        extern ClassType Collider, BoxCollider, SphereCollider, CapsuleCollider, MeshCollider, Rigidbody, Physics;
#endif
#ifdef BNM_UNITY_PHYSICS2D
        extern ClassType Collider2D, BoxCollider2D, CircleCollider2D, Rigidbody2D, Physics2D;
#endif
#ifdef BNM_UNITY_UI
        extern ClassType RectTransform, Canvas, CanvasScaler, UIBehaviour, Graphic, GraphicRaycaster, MaskableGraphic, Text, UIImage, Selectable, Button, Slider, Toggle, InputField, EventSystem;
#endif
#ifdef BNM_UNITY_TEXTMESHPRO
        extern ClassType TMP_Text, TextMeshPro, TextMeshProUGUI, TMP_InputField;
#endif
    }
    /// @endcond

    struct DefaultTypeRef {
        Internal::ClassType *_reference{};

        inline bool IsValid() const { return _reference != nullptr; }
        inline explicit operator bool() const { return IsValid(); }
        operator BNM::CompileTimeClass() const;
        operator BNM::Class() const;
        inline operator IL2CPP::Il2CppClass *() const { return _reference ? *_reference : nullptr; }
        BNM::Class ToClass() const;
    };

    /**
        @brief Returns the reference to a class.
        @tparam T Type
        @return Reference to a class
    */
    template<typename T>
    constexpr DefaultTypeRef Get() {
        using namespace Types;
        using namespace Structures::Unity;
        using CleanT = std::remove_pointer_t<std::remove_cvref_t<T>>;

        if constexpr (std::is_same_v<T, void>)
            return {&Internal::Void};
        else if constexpr (std::is_same_v<T, bool>)
            return {&Internal::Boolean};
        else if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, byte>)
            return {&Internal::Byte};
        else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, sbyte>)
            return {&Internal::SByte};
        else if constexpr (std::is_same_v<T, short>)
            return {&Internal::Int16};
        else if constexpr (std::is_same_v<T, ushort>)
            return {&Internal::UInt16};
        else if constexpr (std::is_same_v<T, int>)
            return {&Internal::Int32};
        else if constexpr (std::is_same_v<T, uint>)
            return {&Internal::UInt32};
        else if constexpr (std::is_same_v<T, nint>)
            return {&Internal::IntPtr};
        else if constexpr (std::is_same_v<T, nuint>)
            return {&Internal::UIntPtr};
        else if constexpr (std::is_same_v<T, long>)
            return {&Internal::Int64};
        else if constexpr (std::is_same_v<T, unsigned long>)
            return {&Internal::UInt64};
        else if constexpr (std::is_same_v<T, float>)
            return {&Internal::Single};
        else if constexpr (std::is_same_v<T, double>)
            return {&Internal::Double};
        else if constexpr (std::is_same_v<T, decimal>)
            return {&Internal::Decimal};
        else if constexpr (std::is_same_v<T, BNM::IL2CPP::Il2CppString *> || std::is_same_v<T, Structures::Mono::String *>)
            return {&Internal::String};

        // Unity math / value types
        else if constexpr (std::is_same_v<CleanT, Vector2>)
            return {&Internal::Vector2};
        else if constexpr (std::is_same_v<CleanT, Vector3>)
            return {&Internal::Vector3};
        else if constexpr (std::is_same_v<CleanT, Vector4>)
            return {&Internal::Vector4};
        else if constexpr (std::is_same_v<CleanT, Color>)
            return {&Internal::Color};
        else if constexpr (std::is_same_v<CleanT, Color32>)
            return {&Internal::Color32};
        else if constexpr (std::is_same_v<CleanT, Ray>)
            return {&Internal::Ray};
        else if constexpr (std::is_same_v<CleanT, Quaternion>)
            return {&Internal::Quaternion};
        else if constexpr (std::is_same_v<CleanT, Matrix3x3>)
            return {&Internal::Matrix3x3};
        else if constexpr (std::is_same_v<CleanT, Matrix4x4>)
            return {&Internal::Matrix4x4};
        else if constexpr (std::is_same_v<CleanT, RaycastHit>)
            return {&Internal::RaycastHit};
        else if constexpr (std::is_same_v<CleanT, RaycastHit2D>)
            return {&Internal::RaycastHit2D};
        else if constexpr (std::is_same_v<CleanT, Bounds>)
            return {&Internal::Bounds};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Touch>)
            return {&Internal::Touch};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::SceneManagement::Scene>)
            return {&Internal::Scene};

        // Unity Classes (Order: most-derived to base / leaf-to-base)
#ifdef BNM_UNITY_TEXTMESHPRO
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::TMPro::TextMeshPro>)
            return {&Internal::TextMeshPro};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::TMPro::TextMeshProUGUI>)
            return {&Internal::TextMeshProUGUI};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::TMPro::TMP_Text>)
            return {&Internal::TMP_Text};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::TMPro::TMP_InputField>)
            return {&Internal::TMP_InputField};
#endif

#ifdef BNM_UNITY_UI
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::Button>)
            return {&Internal::Button};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::Slider>)
            return {&Internal::Slider};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::Toggle>)
            return {&Internal::Toggle};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::InputField>)
            return {&Internal::InputField};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::Selectable>)
            return {&Internal::Selectable};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::Text>)
            return {&Internal::Text};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::Image>)
            return {&Internal::UIImage};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::MaskableGraphic>)
            return {&Internal::MaskableGraphic};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::GraphicRaycaster>)
            return {&Internal::GraphicRaycaster};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::Graphic>)
            return {&Internal::Graphic};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::CanvasScaler>)
            return {&Internal::CanvasScaler};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::Canvas>)
            return {&Internal::Canvas};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::EventSystem>)
            return {&Internal::EventSystem};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::UIBehaviour>)
            return {&Internal::UIBehaviour};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::UI::RectTransform>)
            return {&Internal::RectTransform};
#endif

#ifdef BNM_UNITY_RENDERERS
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::SkinnedMeshRenderer>)
            return {&Internal::SkinnedMeshRenderer};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::MeshRenderer>)
            return {&Internal::MeshRenderer};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::SpriteRenderer>)
            return {&Internal::SpriteRenderer};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Renderer>)
            return {&Internal::Renderer};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Mesh>)
            return {&Internal::Mesh};
#endif

#ifdef BNM_UNITY_AUDIO
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::AudioSource>)
            return {&Internal::AudioSource};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::AudioListener>)
            return {&Internal::AudioListener};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::AudioClip>)
            return {&Internal::AudioClip};
#endif

#ifdef BNM_UNITY_ANIMATION
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Animator>)
            return {&Internal::Animator};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Animation>)
            return {&Internal::Animation};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::AnimationClip>)
            return {&Internal::AnimationClip};
#endif

#ifdef BNM_UNITY_PHYSICS
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::BoxCollider>)
            return {&Internal::BoxCollider};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::SphereCollider>)
            return {&Internal::SphereCollider};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::CapsuleCollider>)
            return {&Internal::CapsuleCollider};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::MeshCollider>)
            return {&Internal::MeshCollider};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Collider>)
            return {&Internal::Collider};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Rigidbody>)
            return {&Internal::Rigidbody};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Physics>)
            return {&Internal::Physics};
#endif

#ifdef BNM_UNITY_PHYSICS2D
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::BoxCollider2D>)
            return {&Internal::BoxCollider2D};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::CircleCollider2D>)
            return {&Internal::CircleCollider2D};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Collider2D>)
            return {&Internal::Collider2D};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Rigidbody2D>)
            return {&Internal::Rigidbody2D};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Physics2D>)
            return {&Internal::Physics2D};
#endif

        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Camera>)
            return {&Internal::Camera};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Light>)
            return {&Internal::Light};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Texture2D>)
            return {&Internal::Texture2D};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::RenderTexture>)
            return {&Internal::RenderTexture};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Texture>)
            return {&Internal::Texture};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Material>)
            return {&Internal::Material};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Shader>)
            return {&Internal::Shader};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Application>)
            return {&Internal::Application};

        // Core Utilities & Input
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Time>)
            return {&Internal::Time};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Screen>)
            return {&Internal::Screen};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Input>)
            return {&Internal::Input};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::SystemInfo>)
            return {&Internal::SystemInfo};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::PlayerPrefs>)
            return {&Internal::PlayerPrefs};

        // Scene Management & Assets
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::AsyncOperation>)
            return {&Internal::AsyncOperation};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::YieldInstruction>)
            return {&Internal::YieldInstruction};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Resources>)
            return {&Internal::Resources};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::SceneManagement::SceneManager>)
            return {&Internal::SceneManager};

        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::MonoBehaviour>)
            return {&Internal::MonoBehaviour};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Behaviour>)
            return {&Internal::Behaviour};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Transform>)
            return {&Internal::Transform};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Component>)
            return {&Internal::Component};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::GameObject>)
            return {&Internal::GameObject};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::ScriptableObject>)
            return {&Internal::ScriptableObject};
        else if constexpr (std::is_same_v<CleanT, BNM::UnityEngine::Object>)
            return {&Internal::UnityObject};

        // Fallbacks
        else if constexpr (std::is_same_v<CleanT, BNM::IL2CPP::Il2CppObject>)
            return {&Internal::Object};
        else if constexpr (std::is_pointer_v<T>)
            return {&Internal::Object};
        return {};
    }

    /**
        @brief Simple method for boxing values.
        @param value Value that needs to be packed
        @tparam T Value type

        Method is based on BNM::Defaults::Get so only types supported by it will be boxed. Others will result null.

        @return Boxed value if type is supported, otherwise null.
    */
    template<typename T>
    inline IL2CPP::Il2CppObject *Box(T value) {
        if constexpr (std::is_pointer_v<T>) {
            return (IL2CPP::Il2CppObject *) value;
        } else if constexpr (std::is_base_of_v<IL2CPP::Il2CppObject, std::remove_cvref_t<T>>) {
            return (IL2CPP::Il2CppObject *) &value;
        } else {
            return BNM::Defaults::Get<T>().ToClass().BoxObject(value);
        }
    }

    /**
        @brief Simple method for unboxing values from Il2CppObject.
        @param obj Boxed object to unpack
        @tparam T Value type
        @return Unboxed value if valid, default constructed T otherwise.
    */
    template<typename T>
    inline T Unbox(IL2CPP::Il2CppObject *obj) {
        if (!obj) return T{};
        if constexpr (std::is_pointer_v<T>) {
            return (T) obj;
        } else if constexpr (std::is_base_of_v<IL2CPP::Il2CppObject, std::remove_cvref_t<T>>) {
            return *(T *) obj;
        } else {
            return *(T *) ((char *) obj + sizeof(IL2CPP::Il2CppObject));
        }
    }
}
// NOLINTEND
