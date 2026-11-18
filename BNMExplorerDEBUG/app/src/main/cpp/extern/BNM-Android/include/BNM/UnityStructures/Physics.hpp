#pragma once

#include "../UserSettings/GlobalSettings.hpp"

#ifdef BNM_UNITY_PHYSICS

#include <string>
#include <string_view>
#include <limits>
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Component.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Ray.hpp"
#include "RaycastHit.hpp"

namespace BNM::UnityEngine {

    struct Rigidbody;
    struct Collider;
    struct BoxCollider;
    struct SphereCollider;
    struct CapsuleCollider;
    struct MeshCollider;

    /**
        @brief Base class for all colliders in 3D physics.
    */
    struct Collider : public Component {

        /**
            @brief The rigidbody the collider is attached to, or nullptr if none.
            @return Rigidbody pointer.
        */
        inline Rigidbody *GetAttachedRigidbody() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Collider>().ToClass().GetMethod(BNM_OBFUSCATE("get_attachedRigidbody"), 0).cast<Rigidbody *>();
            return method[(void *)this]();
        }

        /**
            @brief Is the collider configured as a trigger?
            @return True if trigger.
        */
        inline bool GetIsTrigger() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Collider>().ToClass().GetMethod(BNM_OBFUSCATE("get_isTrigger"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether the collider is a trigger.
            @param value True to set as trigger.
        */
        inline void SetIsTrigger(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Collider>().ToClass().GetMethod(BNM_OBFUSCATE("set_isTrigger"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Return the point on the collider that is closest to the given location.
            @note Unity Version Aware: Uses ClosestPoint on Unity 2017.1+, falls back to ClosestPointOnBounds on older.
            @param position Point in world coordinates.
            @return Closest world point.
        */
        inline Structures::Unity::Vector3 ClosestPoint(Structures::Unity::Vector3 position) const {
            if (!IsValid()) return {};
            static auto methodNew = BNM::Defaults::Get<Collider>().ToClass().GetMethod(BNM_OBFUSCATE("ClosestPoint"), 1).cast<Structures::Unity::Vector3>();
            if (methodNew.IsValid()) return methodNew[(void *)this](position);
            static auto methodOld = BNM::Defaults::Get<Collider>().ToClass().GetMethod(BNM_OBFUSCATE("ClosestPointOnBounds"), 1).cast<Structures::Unity::Vector3>();
            if (methodOld.IsValid()) return methodOld[(void *)this](position);
            return {};
        }

        /**
            @brief Casts a Ray that ignores all Colliders except this one.
            @param ray The starting point and direction of the ray.
            @param hitInfo Cast hit information output.
            @param maxDistance Maximum distance to check.
            @return True if ray intersects collider.
        */
        inline bool Raycast(Structures::Unity::Ray ray, Structures::Unity::RaycastHit &hitInfo, float maxDistance = std::numeric_limits<float>::infinity()) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Collider>().ToClass().GetMethod(BNM_OBFUSCATE("Raycast"), 3).cast<bool>();
            if (!method.IsValid()) return false;
            return method[(void *)this](ray, &hitInfo, maxDistance);
        }
    };

    /**
        @brief A box-shaped primitive collider.
    */
    struct BoxCollider : public Collider {

        /**
            @brief Gets the center of the box in the object's local space.
            @return Vector3 center.
        */
        inline Structures::Unity::Vector3 GetCenter() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<BoxCollider>().ToClass().GetMethod(BNM_OBFUSCATE("get_center"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the center of the box in local space.
            @param value Vector3 center.
        */
        inline void SetCenter(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<BoxCollider>().ToClass().GetMethod(BNM_OBFUSCATE("set_center"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the size of the box measured in the object's local space.
            @return Vector3 dimensions.
        */
        inline Structures::Unity::Vector3 GetSize() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<BoxCollider>().ToClass().GetMethod(BNM_OBFUSCATE("get_size"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the size of the box in local space.
            @param value Vector3 dimensions.
        */
        inline void SetSize(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<BoxCollider>().ToClass().GetMethod(BNM_OBFUSCATE("set_size"), 1).cast<void>();
            method[(void *)this](value);
        }
    };

    /**
        @brief A sphere-shaped primitive collider.
    */
    struct SphereCollider : public Collider {

        /**
            @brief Gets the center of the sphere in the object's local space.
            @return Vector3 center.
        */
        inline Structures::Unity::Vector3 GetCenter() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<SphereCollider>().ToClass().GetMethod(BNM_OBFUSCATE("get_center"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the center of the sphere in local space.
            @param value Vector3 center.
        */
        inline void SetCenter(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SphereCollider>().ToClass().GetMethod(BNM_OBFUSCATE("set_center"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the radius of the sphere measured in local space.
            @return Radius float.
        */
        inline float GetRadius() const {
            if (!IsValid()) return 0.5f;
            static auto method = BNM::Defaults::Get<SphereCollider>().ToClass().GetMethod(BNM_OBFUSCATE("get_radius"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the radius of the sphere in local space.
            @param value Radius float.
        */
        inline void SetRadius(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<SphereCollider>().ToClass().GetMethod(BNM_OBFUSCATE("set_radius"), 1).cast<void>();
            method[(void *)this](value);
        }
    };

    /**
        @brief A capsule-shaped primitive collider.
    */
    struct CapsuleCollider : public Collider {

        /**
            @brief Gets the center of the capsule in the object's local space.
            @return Vector3 center.
        */
        inline Structures::Unity::Vector3 GetCenter() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<CapsuleCollider>().ToClass().GetMethod(BNM_OBFUSCATE("get_center"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the center of the capsule in local space.
            @param value Vector3 center.
        */
        inline void SetCenter(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<CapsuleCollider>().ToClass().GetMethod(BNM_OBFUSCATE("set_center"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the radius of the capsule's hemispherical ends.
            @return Radius float.
        */
        inline float GetRadius() const {
            if (!IsValid()) return 0.5f;
            static auto method = BNM::Defaults::Get<CapsuleCollider>().ToClass().GetMethod(BNM_OBFUSCATE("get_radius"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the radius of the capsule.
            @param value Radius float.
        */
        inline void SetRadius(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<CapsuleCollider>().ToClass().GetMethod(BNM_OBFUSCATE("set_radius"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the height of the capsule.
            @return Height float.
        */
        inline float GetHeight() const {
            if (!IsValid()) return 2.0f;
            static auto method = BNM::Defaults::Get<CapsuleCollider>().ToClass().GetMethod(BNM_OBFUSCATE("get_height"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the height of the capsule.
            @param value Height float.
        */
        inline void SetHeight(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<CapsuleCollider>().ToClass().GetMethod(BNM_OBFUSCATE("set_height"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the directional alignment axis of the capsule (0 = X, 1 = Y, 2 = Z).
            @return Direction axis index.
        */
        inline int GetDirection() const {
            if (!IsValid()) return 1;
            static auto method = BNM::Defaults::Get<CapsuleCollider>().ToClass().GetMethod(BNM_OBFUSCATE("get_direction"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the directional alignment axis of the capsule (0 = X, 1 = Y, 2 = Z).
            @param value Direction axis index.
        */
        inline void SetDirection(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<CapsuleCollider>().ToClass().GetMethod(BNM_OBFUSCATE("set_direction"), 1).cast<void>();
            method[(void *)this](value);
        }
    };

    /**
        @brief A mesh-based collider for complex geometry.
    */
    struct MeshCollider : public Collider {

        /**
            @brief Use a convex mesh for the collider (required for dynamic rigidbodies).
            @return True if convex.
        */
        inline bool GetConvex() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<MeshCollider>().ToClass().GetMethod(BNM_OBFUSCATE("get_convex"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether the mesh collider is convex.
            @param value True to make convex.
        */
        inline void SetConvex(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<MeshCollider>().ToClass().GetMethod(BNM_OBFUSCATE("set_convex"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The mesh object used for collision detection.
            @return Mesh Object pointer.
        */
        inline Object *GetSharedMesh() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<MeshCollider>().ToClass().GetMethod(BNM_OBFUSCATE("get_sharedMesh"), 0).cast<Object *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the mesh object used for collision detection.
            @param mesh Mesh Object pointer.
        */
        inline void SetSharedMesh(Object *mesh) {
            if (!IsValid() || !mesh) return;
            static auto method = BNM::Defaults::Get<MeshCollider>().ToClass().GetMethod(BNM_OBFUSCATE("set_sharedMesh"), 1).cast<void>();
            method[(void *)this](mesh);
        }
    };

    /**
        @brief Control of an object's position through physics simulation.
    */
    struct Rigidbody : public Component {

        /**
            @brief The velocity vector of the rigidbody. It represents the rate of change of Rigidbody position.
            @return Vector3 velocity.
        */
        inline Structures::Unity::Vector3 GetVelocity() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_velocity"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the velocity vector of the rigidbody.
            @param value New velocity.
        */
        inline void SetVelocity(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_velocity"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The angular velocity vector of the rigidbody measured in radians per second.
            @return Vector3 angular velocity.
        */
        inline Structures::Unity::Vector3 GetAngularVelocity() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_angularVelocity"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets angular velocity.
            @param value Angular velocity in radians per second.
        */
        inline void SetAngularVelocity(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_angularVelocity"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The mass of the rigidbody.
            @return Mass in kg.
        */
        inline float GetMass() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_mass"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the mass of the rigidbody.
            @param value Mass in kg.
        */
        inline void SetMass(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_mass"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The drag of the object (air resistance).
            @return Linear drag coefficient.
        */
        inline float GetDrag() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_drag"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets linear drag.
            @param value Drag coefficient.
        */
        inline void SetDrag(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_drag"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The angular drag of the object (rotational resistance).
            @return Angular drag coefficient.
        */
        inline float GetAngularDrag() const {
            if (!IsValid()) return 0.05f;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_angularDrag"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets angular drag.
            @param value Angular drag coefficient.
        */
        inline void SetAngularDrag(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_angularDrag"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Controls whether gravity affects this rigidbody.
            @return True if gravity is enabled.
        */
        inline bool GetUseGravity() const {
            if (!IsValid()) return true;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_useGravity"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether gravity affects this rigidbody.
            @param value True to enable gravity.
        */
        inline void SetUseGravity(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_useGravity"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Controls whether physics affects the rigidbody (kinematic bodies are moved via script).
            @return True if kinematic.
        */
        inline bool GetIsKinematic() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_isKinematic"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether the rigidbody is kinematic.
            @param value True for kinematic.
        */
        inline void SetIsKinematic(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_isKinematic"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Controls whether physics will change the rotation of the object.
            @return True if rotation is frozen.
        */
        inline bool GetFreezeRotation() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_freezeRotation"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether rotation is frozen.
            @param value True to freeze rotation.
        */
        inline void SetFreezeRotation(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_freezeRotation"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets RigidbodyConstraints bitmask (FreezePositionX=2, FreezePositionY=4, FreezePositionZ=8, FreezeRotationX=16, dll).
            @return Constraints integer bitmask.
        */
        inline int GetConstraints() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_constraints"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets RigidbodyConstraints bitmask.
            @param value Constraints integer bitmask.
        */
        inline void SetConstraints(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_constraints"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The position of the rigidbody in world space.
            @return Vector3 position.
        */
        inline Structures::Unity::Vector3 GetPosition() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_position"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the position of the rigidbody.
            @param value Vector3 world position.
        */
        inline void SetPosition(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_position"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The rotation of the rigidbody in world space.
            @return Quaternion rotation.
        */
        inline Structures::Unity::Quaternion GetRotation() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("get_rotation"), 0).cast<Structures::Unity::Quaternion>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the rotation of the rigidbody.
            @param value Quaternion rotation.
        */
        inline void SetRotation(Structures::Unity::Quaternion value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("set_rotation"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Adds a force to the Rigidbody.
            @param force Force vector in world coordinates.
            @param mode ForceMode enum (0 = Force, 1 = Acceleration, 2 = Impulse, 5 = VelocityChange).
        */
        inline void AddForce(Structures::Unity::Vector3 force, int mode = 0) {
            if (!IsValid()) return;
            static auto method2 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddForce"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](force, mode);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddForce"), 1).cast<void>();
            if (method1.IsValid()) method1[(void *)this](force);
        }

        /**
            @brief Adds a force to the Rigidbody relative to its coordinate system.
            @param force Relative force vector.
            @param mode ForceMode enum.
        */
        inline void AddRelativeForce(Structures::Unity::Vector3 force, int mode = 0) {
            if (!IsValid()) return;
            static auto method2 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddRelativeForce"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](force, mode);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddRelativeForce"), 1).cast<void>();
            if (method1.IsValid()) method1[(void *)this](force);
        }

        /**
            @brief Adds a torque to the Rigidbody.
            @param torque Torque vector.
            @param mode ForceMode enum.
        */
        inline void AddTorque(Structures::Unity::Vector3 torque, int mode = 0) {
            if (!IsValid()) return;
            static auto method2 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddTorque"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](torque, mode);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddTorque"), 1).cast<void>();
            if (method1.IsValid()) method1[(void *)this](torque);
        }

        /**
            @brief Adds a torque to the Rigidbody relative to its coordinate system.
            @param torque Relative torque vector.
            @param mode ForceMode enum.
        */
        inline void AddRelativeTorque(Structures::Unity::Vector3 torque, int mode = 0) {
            if (!IsValid()) return;
            static auto method2 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddRelativeTorque"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](torque, mode);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddRelativeTorque"), 1).cast<void>();
            if (method1.IsValid()) method1[(void *)this](torque);
        }

        /**
            @brief Applies force at position. As a result this will apply a torque and force on the object.
            @param force Force vector.
            @param position World position to apply force at.
            @param mode ForceMode enum.
        */
        inline void AddForceAtPosition(Structures::Unity::Vector3 force, Structures::Unity::Vector3 position, int mode = 0) {
            if (!IsValid()) return;
            static auto method3 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddForceAtPosition"), 3).cast<void>();
            if (method3.IsValid()) {
                method3[(void *)this](force, position, mode);
                return;
            }
            static auto method2 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddForceAtPosition"), 2).cast<void>();
            if (method2.IsValid()) method2[(void *)this](force, position);
        }

        /**
            @brief Applies a force to a Rigidbody that simulates explosion effects.
            @param explosionForce The strength of the explosion.
            @param explosionPosition Center of the explosion sphere.
            @param explosionRadius Radius of the explosion sphere.
            @param upwardsModifier Adjustment to the apparent position of the explosion.
            @param mode ForceMode enum.
        */
        inline void AddExplosionForce(float explosionForce, Structures::Unity::Vector3 explosionPosition, float explosionRadius, float upwardsModifier = 0.0f, int mode = 0) {
            if (!IsValid()) return;
            static auto method5 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddExplosionForce"), 5).cast<void>();
            if (method5.IsValid()) {
                method5[(void *)this](explosionForce, explosionPosition, explosionRadius, upwardsModifier, mode);
                return;
            }
            static auto method4 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddExplosionForce"), 4).cast<void>();
            if (method4.IsValid()) {
                method4[(void *)this](explosionForce, explosionPosition, explosionRadius, upwardsModifier);
                return;
            }
            static auto method3 = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("AddExplosionForce"), 3).cast<void>();
            if (method3.IsValid()) method3[(void *)this](explosionForce, explosionPosition, explosionRadius);
        }

        /**
            @brief Moves the kinematic rigidbody towards position.
            @param position Target position.
        */
        inline void MovePosition(Structures::Unity::Vector3 position) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("MovePosition"), 1).cast<void>();
            method[(void *)this](position);
        }

        /**
            @brief Rotates the rigidbody to rotation.
            @param rotation Target rotation.
        */
        inline void MoveRotation(Structures::Unity::Quaternion rotation) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("MoveRotation"), 1).cast<void>();
            method[(void *)this](rotation);
        }

        /**
            @brief Forces a rigidbody to sleep at least one frame.
        */
        inline void Sleep() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("Sleep"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Is the rigidbody sleeping?
            @return True if sleeping.
        */
        inline bool IsSleeping() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("IsSleeping"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Dislodges the rigidbody from the sleeping state.
        */
        inline void WakeUp() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody>().ToClass().GetMethod(BNM_OBFUSCATE("WakeUp"), 0).cast<void>();
            method[(void *)this]();
        }
    };

    /**
        @brief Global 3D Physics engine interface and raycasting utilities.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Physics {
        Physics() = delete;

        /**
            @brief Gets the default gravity vector applied to all rigidbodies in the scene.
            @return Vector3 gravity.
        */
        static inline Structures::Unity::Vector3 GetGravity() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("get_gravity"), 0).cast<Structures::Unity::Vector3>();
            if (!method.IsValid()) return {0.f, -9.81f, 0.f};
            return method();
        }

        /**
            @brief Sets global gravity vector.
            @param value Gravity acceleration vector.
        */
        static inline void SetGravity(Structures::Unity::Vector3 value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("set_gravity"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief Casts a ray against all colliders in the Scene.
            @param ray The starting point and direction of the ray.
            @param maxDistance The max distance the ray should check for collisions.
            @param layerMask A Layer mask that is used to selectively ignore Colliders.
            @return True if ray intersects any collider.
        */
        static inline bool Raycast(Structures::Unity::Ray ray, float maxDistance = std::numeric_limits<float>::infinity(), int layerMask = -5) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("Raycast"), 3).cast<bool>();
            if (method.IsValid()) return method(ray, maxDistance, layerMask);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("Raycast"), 2).cast<bool>();
            if (method2.IsValid()) return method2(ray, maxDistance);
            static auto method1 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("Raycast"), 1).cast<bool>();
            if (method1.IsValid()) return method1(ray);
            return false;
        }

        /**
            @brief Casts a ray against colliders and returns detailed hit information.
            @param ray The starting point and direction of the ray.
            @param hitInfo Hit information output.
            @param maxDistance Max distance.
            @param layerMask Layer mask.
            @return True if ray intersects collider.
        */
        static inline bool Raycast(Structures::Unity::Ray ray, Structures::Unity::RaycastHit &hitInfo, float maxDistance = std::numeric_limits<float>::infinity(), int layerMask = -5) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("Raycast"), 4).cast<bool>();
            if (method.IsValid()) return method(ray, &hitInfo, maxDistance, layerMask);
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("Raycast"), 3).cast<bool>();
            if (method3.IsValid()) return method3(ray, &hitInfo, maxDistance);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("Raycast"), 2).cast<bool>();
            if (method2.IsValid()) return method2(ray, &hitInfo);
            return false;
        }

        /**
            @brief Casts a ray from origin in direction.
            @param origin Start point.
            @param direction Direction vector.
            @param maxDistance Max distance.
            @param layerMask Layer mask.
            @return True if ray intersects collider.
        */
        static inline bool Raycast(Structures::Unity::Vector3 origin, Structures::Unity::Vector3 direction, float maxDistance = std::numeric_limits<float>::infinity(), int layerMask = -5) {
            return Raycast(Structures::Unity::Ray(origin, direction), maxDistance, layerMask);
        }

        /**
            @brief Casts a ray from origin in direction, returning detailed hit info.
            @param origin Start point.
            @param direction Direction vector.
            @param hitInfo Hit information output.
            @param maxDistance Max distance.
            @param layerMask Layer mask.
            @return True if ray intersects collider.
        */
        static inline bool Raycast(Structures::Unity::Vector3 origin, Structures::Unity::Vector3 direction, Structures::Unity::RaycastHit &hitInfo, float maxDistance = std::numeric_limits<float>::infinity(), int layerMask = -5) {
            return Raycast(Structures::Unity::Ray(origin, direction), hitInfo, maxDistance, layerMask);
        }

        /**
            @brief Casts a ray through the Scene and returns all hits.
            @param ray The ray to cast.
            @param maxDistance Max distance.
            @param layerMask Layer mask.
            @return Mono Array of RaycastHit structures.
        */
        static inline Structures::Mono::Array<Structures::Unity::RaycastHit> *RaycastAll(Structures::Unity::Ray ray, float maxDistance = std::numeric_limits<float>::infinity(), int layerMask = -5) {
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("RaycastAll"), 3).cast<Structures::Mono::Array<Structures::Unity::RaycastHit> *>();
            if (method3.IsValid()) return method3(ray, maxDistance, layerMask);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("RaycastAll"), 2).cast<Structures::Mono::Array<Structures::Unity::RaycastHit> *>();
            if (method2.IsValid()) return method2(ray, maxDistance);
            static auto method1 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("RaycastAll"), 1).cast<Structures::Mono::Array<Structures::Unity::RaycastHit> *>();
            if (method1.IsValid()) return method1(ray);
            return nullptr;
        }

        /**
            @brief Casts a ray without allocating GC memory.
            @param ray The ray to cast.
            @param results Preallocated array to store results into.
            @param maxDistance Max distance.
            @param layerMask Layer mask.
            @return Number of hits stored.
        */
        static inline int RaycastNonAlloc(Structures::Unity::Ray ray, Structures::Mono::Array<Structures::Unity::RaycastHit> *results, float maxDistance = std::numeric_limits<float>::infinity(), int layerMask = -5) {
            if (!results) return 0;
            static auto method4 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("RaycastNonAlloc"), 4).cast<int>();
            if (method4.IsValid()) return method4(ray, results, maxDistance, layerMask);
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("RaycastNonAlloc"), 3).cast<int>();
            if (method3.IsValid()) return method3(ray, results, maxDistance);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("RaycastNonAlloc"), 2).cast<int>();
            if (method2.IsValid()) return method2(ray, results);
            return 0;
        }

        /**
            @brief Returns true if there is any collider intersecting the line between start and end.
            @param start Start point.
            @param end End point.
            @param layerMask Layer mask.
            @return True if intersecting.
        */
        static inline bool Linecast(Structures::Unity::Vector3 start, Structures::Unity::Vector3 end, int layerMask = -5) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("Linecast"), 3).cast<bool>();
            if (method.IsValid()) return method(start, end, layerMask);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("Linecast"), 2).cast<bool>();
            if (method2.IsValid()) return method2(start, end);
            return false;
        }

        /**
            @brief Returns true if there is any collider intersecting the line between start and end, and writes hitInfo.
            @param start Start point.
            @param end End point.
            @param hitInfo Output RaycastHit information.
            @param layerMask Layer mask.
            @return True if intersecting.
        */
        static inline bool Linecast(Structures::Unity::Vector3 start, Structures::Unity::Vector3 end, Structures::Unity::RaycastHit &hitInfo, int layerMask = -5) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("Linecast"), 4).cast<bool>();
            if (method.IsValid()) return method(start, end, &hitInfo, layerMask);
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("Linecast"), 3).cast<bool>();
            if (method3.IsValid()) return method3(start, end, &hitInfo);
            return false;
        }

        /**
            @brief Returns all colliders touching or inside the sphere.
            @param position Center of the sphere.
            @param radius Radius of the sphere.
            @param layerMask Layer mask.
            @return Mono Array of Collider pointers.
        */
        static inline Structures::Mono::Array<Collider *> *OverlapSphere(Structures::Unity::Vector3 position, float radius, int layerMask = -5) {
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("OverlapSphere"), 3).cast<Structures::Mono::Array<Collider *> *>();
            if (method3.IsValid()) return method3(position, radius, layerMask);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("OverlapSphere"), 2).cast<Structures::Mono::Array<Collider *> *>();
            if (method2.IsValid()) return method2(position, radius);
            return nullptr;
        }

        /**
            @brief Returns all colliders touching or inside the given box.
            @param center Center of the box.
            @param halfExtents Half size of the box in each dimension.
            @param orientation Rotation of the box.
            @param layerMask Layer mask.
            @return Mono Array of Collider pointers.
        */
        static inline Structures::Mono::Array<Collider *> *OverlapBox(Structures::Unity::Vector3 center, Structures::Unity::Vector3 halfExtents, Structures::Unity::Quaternion orientation = Structures::Unity::Quaternion(), int layerMask = -5) {
            static auto method4 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("OverlapBox"), 4).cast<Structures::Mono::Array<Collider *> *>();
            if (method4.IsValid()) return method4(center, halfExtents, orientation, layerMask);
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("OverlapBox"), 3).cast<Structures::Mono::Array<Collider *> *>();
            if (method3.IsValid()) return method3(center, halfExtents, orientation);
            return nullptr;
        }

        /**
            @brief Check if there are any colliders touching or inside a sphere.
            @param position Center of sphere.
            @param radius Radius of sphere.
            @param layerMask Layer mask.
            @return True if any colliders overlap.
        */
        static inline bool CheckSphere(Structures::Unity::Vector3 position, float radius, int layerMask = -5) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("CheckSphere"), 3).cast<bool>();
            if (method.IsValid()) return method(position, radius, layerMask);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("CheckSphere"), 2).cast<bool>();
            if (method2.IsValid()) return method2(position, radius);
            return false;
        }

        /**
            @brief Check if any colliders overlap the given box.
            @param center Center of box.
            @param halfExtents Half size.
            @param orientation Rotation of box.
            @param layerMask Layer mask.
            @return True if overlapping.
        */
        static inline bool CheckBox(Structures::Unity::Vector3 center, Structures::Unity::Vector3 halfExtents, Structures::Unity::Quaternion orientation = Structures::Unity::Quaternion(), int layerMask = -5) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("CheckBox"), 4).cast<bool>();
            if (method.IsValid()) return method(center, halfExtents, orientation, layerMask);
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("CheckBox"), 3).cast<bool>();
            if (method3.IsValid()) return method3(center, halfExtents, orientation);
            return false;
        }

        /**
            @brief Makes the collision detection system ignore all collisions between collider1 and collider2.
            @param collider1 First collider.
            @param collider2 Second collider.
            @param ignore True to ignore collisions, false to enable.
        */
        static inline void IgnoreCollision(Collider *collider1, Collider *collider2, bool ignore = true) {
            if (!collider1 || !collider2) return;
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("IgnoreCollision"), 3).cast<void>();
            if (method.IsValid()) method(collider1, collider2, ignore);
        }

        /**
            @brief Makes the collision detection system ignore collisions between layer1 and layer2.
            @param layer1 First layer index.
            @param layer2 Second layer index.
            @param ignore True to ignore collisions.
        */
        static inline void IgnoreLayerCollision(int layer1, int layer2, bool ignore = true) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("IgnoreLayerCollision"), 3).cast<void>();
            if (method.IsValid()) method(layer1, layer2, ignore);
        }

        /**
            @brief Returns true if collision detection between layer1 and layer2 is ignored.
            @param layer1 First layer index.
            @param layer2 Second layer index.
            @return True if ignored.
        */
        static inline bool GetIgnoreLayerCollision(int layer1, int layer2) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("GetIgnoreLayerCollision"), 2).cast<bool>();
            if (!method.IsValid()) return false;
            return method(layer1, layer2);
        }

        /**
            @brief Syncs Transform position and rotation changes with the physics engine immediately.
            @note Unity Version Aware: Available in Unity 2017.2+, safe no-op on older Unity versions.
        */
        static inline void SyncTransforms() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics")).GetMethod(BNM_OBFUSCATE("SyncTransforms"), 0).cast<void>();
            if (method.IsValid()) method();
        }
    };
}

#endif
