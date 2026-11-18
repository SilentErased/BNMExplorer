#pragma once

#include <vector>
#include <string_view>
#include "Component.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Matrix4x4.hpp"

namespace BNM::UnityEngine {
    /**
        @brief UnityEngine.Transform implementation.
        Position, rotation and scale of an object.
    */
    struct Transform : public Component {
        constexpr Transform() : Component() {}

        // --- Position & Coordinates ---

        /**
            @brief Gets the world space position of the Transform.
            @return Vector3 world coordinates.
        */
        inline Structures::Unity::Vector3 GetPosition() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_position"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the world space position of the Transform.
            @param value New world position as Vector3.
        */
        inline void SetPosition(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_position"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the position of the transform relative to the parent transform.
            @return Vector3 local coordinates.
        */
        inline Structures::Unity::Vector3 GetLocalPosition() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_localPosition"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the position of the transform relative to the parent transform.
            @param value New local position as Vector3.
        */
        inline void SetLocalPosition(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_localPosition"), 1).cast<void>();
            method[(void *)this](value);
        }

        // --- Rotation ---

        /**
            @brief Gets the world space rotation of the Transform stored as a Quaternion.
            @return Quaternion rotation.
        */
        inline Structures::Unity::Quaternion GetRotation() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_rotation"), 0).cast<Structures::Unity::Quaternion>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the world space rotation of the Transform using a Quaternion.
            @param value New rotation Quaternion.
        */
        inline void SetRotation(Structures::Unity::Quaternion value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_rotation"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the rotation of the transform relative to the transform rotation of the parent.
            @return Quaternion local rotation.
        */
        inline Structures::Unity::Quaternion GetLocalRotation() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_localRotation"), 0).cast<Structures::Unity::Quaternion>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the rotation of the transform relative to the parent's rotation.
            @param value New local rotation Quaternion.
        */
        inline void SetLocalRotation(Structures::Unity::Quaternion value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_localRotation"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the rotation as Euler angles in degrees in world space.
            @return Vector3 Euler angles (degrees).
        */
        inline Structures::Unity::Vector3 GetEulerAngles() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_eulerAngles"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the rotation as Euler angles in degrees in world space.
            @param value Euler angles in degrees.
        */
        inline void SetEulerAngles(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_eulerAngles"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the rotation as Euler angles in degrees relative to the parent transform's rotation.
            @return Vector3 local Euler angles.
        */
        inline Structures::Unity::Vector3 GetLocalEulerAngles() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_localEulerAngles"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the rotation as Euler angles in degrees relative to the parent transform's rotation.
            @param value Local Euler angles in degrees.
        */
        inline void SetLocalEulerAngles(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_localEulerAngles"), 1).cast<void>();
            method[(void *)this](value);
        }

        // --- Scale ---

        /**
            @brief Gets the scale of the transform relative to the parent.
            @return Vector3 local scale.
        */
        inline Structures::Unity::Vector3 GetLocalScale() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_localScale"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the scale of the transform relative to the parent.
            @param value New local scale Vector3.
        */
        inline void SetLocalScale(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_localScale"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the global scale of the object (read-only).
            @return Vector3 global lossy scale.
        */
        inline Structures::Unity::Vector3 GetLossyScale() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_lossyScale"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        // --- Direction Vectors ---

        /**
            @brief The blue axis of the transform in world space (normalized forward vector: Z axis).
            @return Vector3 forward direction.
        */
        inline Structures::Unity::Vector3 GetForward() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_forward"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the blue axis of the transform in world space.
            @param value New forward direction Vector3.
        */
        inline void SetForward(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_forward"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The green axis of the transform in world space (normalized up vector: Y axis).
            @return Vector3 up direction.
        */
        inline Structures::Unity::Vector3 GetUp() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_up"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the green axis of the transform in world space.
            @param value New up direction Vector3.
        */
        inline void SetUp(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_up"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The red axis of the transform in world space (normalized right vector: X axis).
            @return Vector3 right direction.
        */
        inline Structures::Unity::Vector3 GetRight() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_right"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the red axis of the transform in world space.
            @param value New right direction Vector3.
        */
        inline void SetRight(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_right"), 1).cast<void>();
            method[(void *)this](value);
        }

        // --- Hierarchy & Children ---

        /**
            @brief The parent of the transform.
            @return Transform pointer of parent, or nullptr if at root.
        */
        inline Transform *GetParent() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_parent"), 0).cast<Transform *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the parent of the transform.
            @param parent The parent Transform to use.
            @param worldPositionStays If true, the parent-relative position, scale and rotation are modified such that the object keeps the same world space position, rotation and scale as before.
        */
        inline void SetParent(Transform *parent, bool worldPositionStays = true) {
            if (!IsValid()) return;
            static auto method2 = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("SetParent"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](parent, worldPositionStays);
            } else {
                static auto method1 = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("set_parent"), 1).cast<void>();
                method1[(void *)this](parent);
            }
        }

        /**
            @brief Returns the topmost transform in the hierarchy.
            @return Root Transform pointer.
        */
        inline Transform *GetRoot() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_root"), 0).cast<Transform *>();
            return method[(void *)this]();
        }

        /**
            @brief The number of children the parent Transform has.
            @return Child count integer.
        */
        inline int GetChildCount() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_childCount"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Returns a transform child by index.
            @param index Child index (0 to childCount - 1).
            @return Child Transform pointer, or nullptr if out of range.
        */
        inline Transform *GetChild(int index) const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("GetChild"), 1).cast<Transform *>();
            return method[(void *)this](index);
        }

        /**
            @brief Returns all direct child transforms.
            @return std::vector of child Transform pointers.
        */
        inline std::vector<Transform *> GetChildren() const {
            std::vector<Transform *> children{};
            if (!IsValid()) return children;
            int count = GetChildCount();
            children.reserve(count);
            for (int i = 0; i < count; ++i) {
                auto child = GetChild(i);
                if (child) children.push_back(child);
            }
            return children;
        }

        /**
            @brief Finds a child by n and returns it. Supports path navigation (e.g. "Arm/Hand/Finger").
            @param n Name or path as Mono String pointer.
            @return Child Transform pointer, or nullptr if not found.
        */
        inline Transform *Find(Structures::Mono::String *n) const {
            if (!IsValid() || !n) return nullptr;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("Find"), 1).cast<Transform *>();
            return method[(void *)this](n);
        }

        /**
            @brief Finds a child by name or path using std::string_view.
            @param n Name or path string view.
            @return Child Transform pointer, or nullptr if not found.
        */
        inline Transform *Find(const std::string_view &n) const { return Find(CreateMonoString(n)); }

        /**
            @brief Unparents all children from this transform.
        */
        inline void DetachChildren() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("DetachChildren"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Move the transform to the start of the local transform list.
        */
        inline void SetAsFirstSibling() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("SetAsFirstSibling"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Move the transform to the end of the local transform list.
        */
        inline void SetAsLastSibling() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("SetAsLastSibling"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Gets the sibling index of this transform within its parent.
            @return Sibling index integer.
        */
        inline int GetSiblingIndex() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("GetSiblingIndex"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the sibling index of this transform.
            @param index New sibling index.
        */
        inline void SetSiblingIndex(int index) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("SetSiblingIndex"), 1).cast<void>();
            method[(void *)this](index);
        }

        /**
            @brief Is this transform a child of the specified parent?
            @param parent Parent Transform to test against.
            @return True if this transform is a child/descendant of parent.
        */
        inline bool IsChildOf(Transform *parent) const {
            if (!IsValid() || !parent) return false;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("IsChildOf"), 1).cast<bool>();
            return method[(void *)this](parent);
        }

        // --- LookAt, Rotate & Translate ---

        /**
            @brief Rotates the transform so the forward vector points at target's current position.
            @param target Target Transform to point towards.
            @param worldUp Vector specifying the upward direction.
        */
        inline void LookAt(Transform *target, Structures::Unity::Vector3 worldUp = {0.f, 1.f, 0.f}) {
            if (!IsValid() || !target) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("LookAt"), 2).cast<void>();
            method[(void *)this](target, worldUp);
        }

        /**
            @brief Rotates the transform so the forward vector points at worldPosition.
            @param worldPosition World coordinate to point towards.
            @param worldUp Vector specifying the upward direction.
        */
        inline void LookAt(Structures::Unity::Vector3 worldPosition, Structures::Unity::Vector3 worldUp = {0.f, 1.f, 0.f}) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("LookAt"), 2).cast<void>();
            method[(void *)this](worldPosition, worldUp);
        }

        /**
            @brief Applies a rotation of eulers angles around each axis.
            @param eulers Rotation angles in degrees.
            @param relativeTo Space relative to (0 = Space.Self, 1 = Space.World).
        */
        inline void Rotate(Structures::Unity::Vector3 eulers, int relativeTo = 0) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("Rotate"), 2).cast<void>();
            if (method.IsValid()) {
                method[(void *)this](eulers, relativeTo);
            } else {
                static auto method1 = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("Rotate"), 1).cast<void>();
                method1[(void *)this](eulers);
            }
        }

        /**
            @brief Applies a rotation around x, y, and z axes in degrees.
            @param xAngle Degrees to rotate around X axis.
            @param yAngle Degrees to rotate around Y axis.
            @param zAngle Degrees to rotate around Z axis.
            @param relativeTo Space relative to (0 = Space.Self, 1 = Space.World).
        */
        inline void Rotate(float xAngle, float yAngle, float zAngle, int relativeTo = 0) {
            Rotate(Structures::Unity::Vector3(xAngle, yAngle, zAngle), relativeTo);
        }

        /**
            @brief Rotates the transform around axis by angle degrees.
            @param axis Axis vector to rotate around.
            @param angle Degrees to rotate.
            @param relativeTo Space relative to (0 = Space.Self, 1 = Space.World).
        */
        inline void Rotate(Structures::Unity::Vector3 axis, float angle, int relativeTo = 0) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("Rotate"), 3).cast<void>();
            if (method.IsValid()) {
                method[(void *)this](axis, angle, relativeTo);
            } else {
                static auto method2 = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("Rotate"), 2).cast<void>();
                method2[(void *)this](axis, angle);
            }
        }

        /**
            @brief Moves the transform in the direction and distance of translation.
            @param translation Vector distance and direction to move.
            @param relativeTo Space relative to (0 = Space.Self, 1 = Space.World).
        */
        inline void Translate(Structures::Unity::Vector3 translation, int relativeTo = 0) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("Translate"), 2).cast<void>();
            if (method.IsValid()) {
                method[(void *)this](translation, relativeTo);
            } else {
                static auto method1 = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("Translate"), 1).cast<void>();
                method1[(void *)this](translation);
            }
        }

        /**
            @brief Moves the transform along x, y, and z axes.
            @param x Distance along X axis.
            @param y Distance along Y axis.
            @param z Distance along Z axis.
            @param relativeTo Space relative to (0 = Space.Self, 1 = Space.World).
        */
        inline void Translate(float x, float y, float z, int relativeTo = 0) {
            Translate(Structures::Unity::Vector3(x, y, z), relativeTo);
        }

        // --- Space Transformations ---

        /**
            @brief Transforms position from local space to world space.
            @param position Local position Vector3.
            @return World space position Vector3.
        */
        inline Structures::Unity::Vector3 TransformPoint(Structures::Unity::Vector3 position) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("TransformPoint"), 1).cast<Structures::Unity::Vector3>();
            return method[(void *)this](position);
        }

        /**
            @brief Transforms position from world space to local space.
            @param position World position Vector3.
            @return Local space position Vector3.
        */
        inline Structures::Unity::Vector3 InverseTransformPoint(Structures::Unity::Vector3 position) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("InverseTransformPoint"), 1).cast<Structures::Unity::Vector3>();
            return method[(void *)this](position);
        }

        /**
            @brief Transforms direction from local space to world space. (Not affected by scale).
            @param direction Local direction Vector3.
            @return World direction Vector3.
        */
        inline Structures::Unity::Vector3 TransformDirection(Structures::Unity::Vector3 direction) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("TransformDirection"), 1).cast<Structures::Unity::Vector3>();
            return method[(void *)this](direction);
        }

        /**
            @brief Transforms direction from world space to local space. (Not affected by scale).
            @param direction World direction Vector3.
            @return Local direction Vector3.
        */
        inline Structures::Unity::Vector3 InverseTransformDirection(Structures::Unity::Vector3 direction) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("InverseTransformDirection"), 1).cast<Structures::Unity::Vector3>();
            return method[(void *)this](direction);
        }

        /**
            @brief Transforms vector from local space to world space. (Affected by scale).
            @param vector Local vector Vector3.
            @return World vector Vector3.
        */
        inline Structures::Unity::Vector3 TransformVector(Structures::Unity::Vector3 vector) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("TransformVector"), 1).cast<Structures::Unity::Vector3>();
            return method[(void *)this](vector);
        }

        /**
            @brief Transforms vector from world space to local space. (Affected by scale).
            @param vector World vector Vector3.
            @return Local vector Vector3.
        */
        inline Structures::Unity::Vector3 InverseTransformVector(Structures::Unity::Vector3 vector) const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("InverseTransformVector"), 1).cast<Structures::Unity::Vector3>();
            return method[(void *)this](vector);
        }

        /**
            @brief Rotates the transform about axis passing through point in world coordinates by angle degrees.
            @param point World point to rotate around.
            @param axis Axis vector to rotate about.
            @param angle Degrees to rotate.
        */
        inline void RotateAround(Structures::Unity::Vector3 point, Structures::Unity::Vector3 axis, float angle) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("RotateAround"), 3).cast<void>();
            method[(void *)this](point, axis, angle);
        }

        /**
            @brief Matrix that transforms a point from local space into world space (Read Only).
            @return Matrix4x4 local to world transformation matrix.
        */
        inline Structures::Unity::Matrix4x4 GetLocalToWorldMatrix() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_localToWorldMatrix"), 0).cast<Structures::Unity::Matrix4x4>();
            return method[(void *)this]();
        }

        /**
            @brief Matrix that transforms a point from world space into local space (Read Only).
            @return Matrix4x4 world to local transformation matrix.
        */
        inline Structures::Unity::Matrix4x4 GetWorldToLocalMatrix() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Transform>().ToClass().GetMethod(BNM_OBFUSCATE("get_worldToLocalMatrix"), 0).cast<Structures::Unity::Matrix4x4>();
            return method[(void *)this]();
        }
    };
}
