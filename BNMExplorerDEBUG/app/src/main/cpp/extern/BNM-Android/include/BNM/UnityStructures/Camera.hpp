#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Behaviour.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Ray.hpp"
#include "Rect.hpp"
#include "Color.hpp"
#include "RenderTexture.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Camera component used to render scenes to display or render textures.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Camera : public Behaviour {

        /**
            @brief The first enabled Camera component that is tagged "MainCamera".
            @return Main camera pointer or nullptr.
        */
        static inline Camera *GetMain() {
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_main"), 0).cast<Camera *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief The camera we are currently rendering with (for use in OnGUI/events).
            @return Current rendering camera or nullptr.
        */
        static inline Camera *GetCurrent() {
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_current"), 0).cast<Camera *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Returns all enabled cameras in the scene.
            @return Mono Array of Camera pointers.
        */
        static inline Structures::Mono::Array<Camera *> *GetAllCameras() {
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_allCameras"), 0).cast<Structures::Mono::Array<Camera *> *>();
            if (!method.IsValid()) return nullptr;
            return method();
        }

        /**
            @brief Returns the number of all enabled cameras.
            @return Camera count integer.
        */
        static inline int GetAllCamerasCount() {
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_allCamerasCount"), 0).cast<int>();
            if (!method.IsValid()) return 0;
            return method();
        }

        /**
            @brief Transforms position from world space into screen space.
            @param position Point in world coordinates.
            @return Vector3 screen coordinates (x, y in pixels, z is distance from camera).
        */
        inline Structures::Unity::Vector3 WorldToScreenPoint(Structures::Unity::Vector3 position) const {
            if (!IsValid()) return {};
            static auto method1 = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("WorldToScreenPoint"), 1).cast<Structures::Unity::Vector3>();
            if (method1.IsValid()) return method1[(void *)this](position);
            static auto method2 = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("WorldToScreenPoint"), 2).cast<Structures::Unity::Vector3>();
            if (method2.IsValid()) return method2[(void *)this](position, 2); // 2 = MonoOrStereoscopicEye.Mono
            return {};
        }

        /**
            @brief Transforms position from screen space into world space.
            @param position Screen point (x, y in pixels, z is distance from camera).
            @return Vector3 world coordinates.
        */
        inline Structures::Unity::Vector3 ScreenToWorldPoint(Structures::Unity::Vector3 position) const {
            if (!IsValid()) return {};
            static auto method1 = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("ScreenToWorldPoint"), 1).cast<Structures::Unity::Vector3>();
            if (method1.IsValid()) return method1[(void *)this](position);
            static auto method2 = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("ScreenToWorldPoint"), 2).cast<Structures::Unity::Vector3>();
            if (method2.IsValid()) return method2[(void *)this](position, 2);
            return {};
        }

        /**
            @brief Returns a ray going from camera through a screen point.
            @param position Screen point in pixel coordinates.
            @return Ray in world space.
        */
        inline Structures::Unity::Ray ScreenPointToRay(Structures::Unity::Vector3 position) const {
            if (!IsValid()) return {};
            static auto method1 = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("ScreenPointToRay"), 1).cast<Structures::Unity::Ray>();
            if (method1.IsValid()) return method1[(void *)this](position);
            static auto method2 = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("ScreenPointToRay"), 2).cast<Structures::Unity::Ray>();
            if (method2.IsValid()) return method2[(void *)this](position, 2);
            return {};
        }

        /**
            @brief Returns a ray going from camera through a 2D screen point.
            @param position Screen point (Vector2).
            @return Ray in world space.
        */
        inline Structures::Unity::Ray ScreenPointToRay(Structures::Unity::Vector2 position) const {
            return ScreenPointToRay(Structures::Unity::Vector3(position.x, position.y, 0.f));
        }

        /**
            @brief Transforms position from viewport space into world space.
            @param position Viewport point (0 to 1).
            @return Vector3 world point.
        */
        inline Structures::Unity::Vector3 ViewportToWorldPoint(Structures::Unity::Vector3 position) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("ViewportToWorldPoint"), 1).cast<Structures::Unity::Vector3>();
            if (!method.IsValid()) return {};
            return method[(void *)this](position);
        }

        /**
            @brief Transforms position from world space into viewport space.
            @param position Point in world coordinates.
            @return Vector3 viewport point (0 to 1 coordinates, z is distance).
        */
        inline Structures::Unity::Vector3 WorldToViewportPoint(Structures::Unity::Vector3 position) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("WorldToViewportPoint"), 1).cast<Structures::Unity::Vector3>();
            if (!method.IsValid()) return {};
            return method[(void *)this](position);
        }

        /**
            @brief Returns a ray going from camera through a viewport point.
            @param position Viewport point (0 to 1).
            @return Ray in world coordinates.
        */
        inline Structures::Unity::Ray ViewportPointToRay(Structures::Unity::Vector3 position) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("ViewportPointToRay"), 1).cast<Structures::Unity::Ray>();
            if (!method.IsValid()) return {};
            return method[(void *)this](position);
        }

        /**
            @brief Gets the field of view of the camera in degrees.
            @return FOV float in degrees.
        */
        inline float GetFieldOfView() const {
            if (!IsValid()) return 60.0f;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_fieldOfView"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the field of view of the camera in degrees.
            @param value FOV in degrees.
        */
        inline void SetFieldOfView(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_fieldOfView"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets near clipping plane distance.
            @return Near clip distance in meters.
        */
        inline float GetNearClipPlane() const {
            if (!IsValid()) return 0.3f;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_nearClipPlane"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets near clipping plane distance.
            @param value Near clip distance.
        */
        inline void SetNearClipPlane(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_nearClipPlane"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets far clipping plane distance.
            @return Far clip distance in meters.
        */
        inline float GetFarClipPlane() const {
            if (!IsValid()) return 1000.0f;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_farClipPlane"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets far clipping plane distance.
            @param value Far clip distance.
        */
        inline void SetFarClipPlane(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_farClipPlane"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Is the camera orthographic (true) or perspective (false)?
            @return True if orthographic.
        */
        inline bool GetOrthographic() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_orthographic"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether the camera is orthographic.
            @param value True for orthographic, false for perspective.
        */
        inline void SetOrthographic(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_orthographic"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Camera's half-size when in orthographic mode.
            @return Orthographic size float.
        */
        inline float GetOrthographicSize() const {
            if (!IsValid()) return 5.0f;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_orthographicSize"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets camera's half-size when in orthographic mode.
            @param value Orthographic size.
        */
        inline void SetOrthographicSize(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_orthographicSize"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Camera's depth in the camera rendering order.
            @return Depth float.
        */
        inline float GetDepth() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_depth"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets camera's depth in rendering order.
            @param value Depth float.
        */
        inline void SetDepth(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_depth"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief This is used to render parts of the Scene selectively by layer bitmask.
            @return LayerMask bitmask integer.
        */
        inline int GetCullingMask() const {
            if (!IsValid()) return -1;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_cullingMask"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the culling mask layer bitmask.
            @param value LayerMask bitmask integer.
        */
        inline void SetCullingMask(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_cullingMask"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Destination render texture.
            @return RenderTexture target or nullptr if rendering to screen.
        */
        inline RenderTexture *GetTargetTexture() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_targetTexture"), 0).cast<RenderTexture *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets destination render texture.
            @param target RenderTexture target or nullptr.
        */
        inline void SetTargetTexture(RenderTexture *target) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_targetTexture"), 1).cast<void>();
            method[(void *)this](target);
        }

        /**
            @brief How wide of a screen target is being rendered in pixels.
            @return Pixel width integer.
        */
        inline int GetPixelWidth() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_pixelWidth"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief How tall of a screen target is being rendered in pixels.
            @return Pixel height integer.
        */
        inline int GetPixelHeight() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_pixelHeight"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Where on the screen is the camera rendered in pixel coordinates.
            @return Rect pixel coordinates.
        */
        inline Structures::Unity::Rect GetPixelRect() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_pixelRect"), 0).cast<Structures::Unity::Rect>();
            return method[(void *)this]();
        }

        /**
            @brief Sets where on the screen the camera is rendered in pixel coordinates.
            @param value Rect pixel coordinates.
        */
        inline void SetPixelRect(Structures::Unity::Rect value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_pixelRect"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Where on the screen is the camera rendered in normalized viewport coordinates (0 to 1).
            @return Rect normalized viewport rectangle.
        */
        inline Structures::Unity::Rect GetRect() const {
            if (!IsValid()) return {0.f, 0.f, 1.f, 1.f};
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_rect"), 0).cast<Structures::Unity::Rect>();
            return method[(void *)this]();
        }

        /**
            @brief Sets camera viewport normalized rectangle.
            @param value Rect viewport (0 to 1).
        */
        inline void SetRect(Structures::Unity::Rect value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_rect"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The aspect ratio (width divided by height).
            @return Aspect ratio float.
        */
        inline float GetAspect() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_aspect"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets custom aspect ratio.
            @param value Aspect ratio float.
        */
        inline void SetAspect(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_aspect"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the background color used to clear the screen (CameraClearFlags.SolidColor).
            @return Background Color.
        */
        inline Structures::Unity::Color GetBackgroundColor() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("get_backgroundColor"), 0).cast<Structures::Unity::Color>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the background clear color.
            @param value Background Color.
        */
        inline void SetBackgroundColor(Structures::Unity::Color value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Camera>().ToClass().GetMethod(BNM_OBFUSCATE("set_backgroundColor"), 1).cast<void>();
            method[(void *)this](value);
        }
    };
}
