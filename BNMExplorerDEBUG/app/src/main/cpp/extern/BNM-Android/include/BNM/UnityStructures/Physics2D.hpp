#pragma once

#include "../UserSettings/GlobalSettings.hpp"

#ifdef BNM_UNITY_PHYSICS2D

#include <string>
#include <string_view>
#include <limits>
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Behaviour.hpp"
#include "Vector2.hpp"
#include "RaycastHit2D.hpp"

namespace BNM::UnityEngine {

    struct Rigidbody2D;
    struct Collider2D;
    struct BoxCollider2D;
    struct CircleCollider2D;

    /**
        @brief Base class for 2D colliders.
    */
    struct Collider2D : public Behaviour {

        /**
            @brief The Rigidbody2D attached to the Collider2D (or nullptr if none).
            @return Rigidbody2D pointer.
        */
        inline Rigidbody2D *GetAttachedRigidbody() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Collider2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_attachedRigidbody"), 0).cast<Rigidbody2D *>();
            return method[(void *)this]();
        }

        /**
            @brief Whether this collider is a trigger.
            @return True if trigger.
        */
        inline bool GetIsTrigger() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Collider2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_isTrigger"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether this collider is a trigger.
            @param value True for trigger.
        */
        inline void SetIsTrigger(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Collider2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_isTrigger"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The density of the collider used to calculate mass (when attached to dynamic Rigidbody2D).
            @return Density float.
        */
        inline float GetDensity() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Collider2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_density"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets collider density.
            @param value Density float.
        */
        inline void SetDensity(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Collider2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_density"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The local offset of the collider geometry.
            @return Vector2 offset.
        */
        inline Structures::Unity::Vector2 GetOffset() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Collider2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_offset"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets local offset of the collider geometry.
            @param value Vector2 offset.
        */
        inline void SetOffset(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Collider2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_offset"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Check if a collider overlaps a point in space.
            @param point World coordinates point.
            @return True if point is inside collider.
        */
        inline bool OverlapPoint(Structures::Unity::Vector2 point) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Collider2D>().ToClass().GetMethod(BNM_OBFUSCATE("OverlapPoint"), 1).cast<bool>();
            return method[(void *)this](point);
        }
    };

    /**
        @brief A 2D box-shaped collider.
    */
    struct BoxCollider2D : public Collider2D {

        /**
            @brief Gets the width and height of the rectangle.
            @return Vector2 size.
        */
        inline Structures::Unity::Vector2 GetSize() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<BoxCollider2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_size"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the width and height of the rectangle.
            @param value Vector2 size.
        */
        inline void SetSize(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<BoxCollider2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_size"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the radius of the corner edges.
            @return Edge radius float.
        */
        inline float GetEdgeRadius() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<BoxCollider2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_edgeRadius"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets corner edge radius.
            @param value Edge radius float.
        */
        inline void SetEdgeRadius(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<BoxCollider2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_edgeRadius"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Controls whether the collider automatically resizes with SpriteRenderer tiling.
            @return True if auto tiling enabled.
        */
        inline bool GetAutoTiling() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<BoxCollider2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_autoTiling"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets auto tiling.
            @param value True to enable auto tiling.
        */
        inline void SetAutoTiling(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<BoxCollider2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_autoTiling"), 1).cast<void>();
            method[(void *)this](value);
        }
    };

    /**
        @brief A 2D circle-shaped collider.
    */
    struct CircleCollider2D : public Collider2D {

        /**
            @brief Gets radius of the circle collider.
            @return Radius float.
        */
        inline float GetRadius() const {
            if (!IsValid()) return 0.5f;
            static auto method = BNM::Defaults::Get<CircleCollider2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_radius"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets radius of the circle collider.
            @param value Radius float.
        */
        inline void SetRadius(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<CircleCollider2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_radius"), 1).cast<void>();
            method[(void *)this](value);
        }
    };

    /**
        @brief Control of an object's 2D position through physics simulation.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Rigidbody2D : public Behaviour {

        /**
            @brief The physical position of the Rigidbody2D in world space.
            @return Vector2 position.
        */
        inline Structures::Unity::Vector2 GetPosition() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_position"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets position of the Rigidbody2D.
            @param value Vector2 position.
        */
        inline void SetPosition(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_position"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The physical rotation of the Rigidbody2D in degrees.
            @return Rotation angle in degrees.
        */
        inline float GetRotation() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_rotation"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets rotation angle in degrees.
            @param value Rotation angle.
        */
        inline void SetRotation(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_rotation"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Linear velocity of the Rigidbody2D in units per second.
            @return Vector2 velocity.
        */
        inline Structures::Unity::Vector2 GetVelocity() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_velocity"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets linear velocity.
            @param value Vector2 velocity.
        */
        inline void SetVelocity(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_velocity"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Angular velocity in degrees per second.
            @return Angular velocity float.
        */
        inline float GetAngularVelocity() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_angularVelocity"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets angular velocity.
            @param value Angular velocity in degrees per second.
        */
        inline void SetAngularVelocity(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_angularVelocity"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Mass of the Rigidbody2D in kg.
            @return Mass float.
        */
        inline float GetMass() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_mass"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets mass in kg.
            @param value Mass float.
        */
        inline void SetMass(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_mass"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The degree to which this object is affected by gravity.
            @return Gravity scale multiplier.
        */
        inline float GetGravityScale() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_gravityScale"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets gravity scale multiplier.
            @param value Gravity scale.
        */
        inline void SetGravityScale(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_gravityScale"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets RigidbodyType2D (0 = Dynamic, 1 = Kinematic, 2 = Static).
            @note Unity Version Aware: Falls back to isKinematic on Unity 5.6.
            @return RigidbodyType2D integer value.
        */
        inline int GetBodyType() const {
            if (!IsValid()) return 0;
            static auto methodType = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_bodyType"), 0).cast<int>();
            if (methodType.IsValid()) return methodType[(void *)this]();
            static auto methodKinematic = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_isKinematic"), 0).cast<bool>();
            if (methodKinematic.IsValid()) return methodKinematic[(void *)this]() ? 1 : 0;
            return 0;
        }

        /**
            @brief Sets RigidbodyType2D (0 = Dynamic, 1 = Kinematic, 2 = Static).
            @param value RigidbodyType2D integer.
        */
        inline void SetBodyType(int value) {
            if (!IsValid()) return;
            static auto methodType = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_bodyType"), 1).cast<void>();
            if (methodType.IsValid()) {
                methodType[(void *)this](value);
                return;
            }
            static auto methodKinematic = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_isKinematic"), 1).cast<void>();
            if (methodKinematic.IsValid()) methodKinematic[(void *)this](value == 1);
        }

        /**
            @brief Should rotation be frozen?
            @return True if rotation frozen.
        */
        inline bool GetFreezeRotation() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_freezeRotation"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether rotation is frozen.
            @param value True to freeze.
        */
        inline void SetFreezeRotation(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_freezeRotation"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Indicates whether the Rigidbody2D should be simulated or not by the physics system.
            @return True if simulated.
        */
        inline bool GetSimulated() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("get_simulated"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether the Rigidbody2D is simulated.
            @param value True to simulate.
        */
        inline void SetSimulated(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("set_simulated"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Apply a force to the Rigidbody2D.
            @param force Force vector in world coordinates.
            @param mode ForceMode2D enum (0 = Force, 1 = Impulse).
        */
        inline void AddForce(Structures::Unity::Vector2 force, int mode = 0) {
            if (!IsValid()) return;
            static auto method2 = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("AddForce"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](force, mode);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("AddForce"), 1).cast<void>();
            if (method1.IsValid()) method1[(void *)this](force);
        }

        /**
            @brief Apply a force relative to Rigidbody2D coordinate system.
            @param force Relative force vector.
            @param mode ForceMode2D enum.
        */
        inline void AddRelativeForce(Structures::Unity::Vector2 force, int mode = 0) {
            if (!IsValid()) return;
            static auto method2 = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("AddRelativeForce"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](force, mode);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("AddRelativeForce"), 1).cast<void>();
            if (method1.IsValid()) method1[(void *)this](force);
        }

        /**
            @brief Apply a torque at the rigidbody's center of mass.
            @param torque Torque float.
            @param mode ForceMode2D enum.
        */
        inline void AddTorque(float torque, int mode = 0) {
            if (!IsValid()) return;
            static auto method2 = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("AddTorque"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](torque, mode);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("AddTorque"), 1).cast<void>();
            if (method1.IsValid()) method1[(void *)this](torque);
        }

        /**
            @brief Apply a force at a given position in space.
            @param force Force vector.
            @param position Position vector.
            @param mode ForceMode2D enum.
        */
        inline void AddForceAtPosition(Structures::Unity::Vector2 force, Structures::Unity::Vector2 position, int mode = 0) {
            if (!IsValid()) return;
            static auto method3 = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("AddForceAtPosition"), 3).cast<void>();
            if (method3.IsValid()) {
                method3[(void *)this](force, position, mode);
                return;
            }
            static auto method2 = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("AddForceAtPosition"), 2).cast<void>();
            if (method2.IsValid()) method2[(void *)this](force, position);
        }

        /**
            @brief Moves the rigidbody to position.
            @param position Target position.
        */
        inline void MovePosition(Structures::Unity::Vector2 position) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("MovePosition"), 1).cast<void>();
            method[(void *)this](position);
        }

        /**
            @brief Rotates the rigidbody to angle.
            @param angle Target angle in degrees.
        */
        inline void MoveRotation(float angle) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("MoveRotation"), 1).cast<void>();
            method[(void *)this](angle);
        }

        /**
            @brief Forces the Rigidbody2D to sleep.
        */
        inline void Sleep() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("Sleep"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Is the Rigidbody2D sleeping?
            @return True if sleeping.
        */
        inline bool IsSleeping() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("IsSleeping"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Wakes the Rigidbody2D from sleeping.
        */
        inline void WakeUp() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Rigidbody2D>().ToClass().GetMethod(BNM_OBFUSCATE("WakeUp"), 0).cast<void>();
            method[(void *)this]();
        }
    };

    /**
        @brief Global 2D Physics engine interface.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Physics2D {
        Physics2D() = delete;

        /**
            @brief Gets global 2D gravity vector.
            @return Vector2 gravity.
        */
        static inline Structures::Unity::Vector2 GetGravity() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("get_gravity"), 0).cast<Structures::Unity::Vector2>();
            if (!method.IsValid()) return {0.f, -9.81f};
            return method();
        }

        /**
            @brief Sets global 2D gravity vector.
            @param value Vector2 gravity.
        */
        static inline void SetGravity(Structures::Unity::Vector2 value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("set_gravity"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief Casts a ray against Colliders in the Scene.
            @param origin Point in world space where the ray starts.
            @param direction Vector2 representing direction of ray.
            @param distance Maximum distance ray should check.
            @param layerMask Layer mask.
            @return RaycastHit2D structure.
        */
        static inline Structures::Unity::RaycastHit2D Raycast(Structures::Unity::Vector2 origin, Structures::Unity::Vector2 direction, float distance = std::numeric_limits<float>::infinity(), int layerMask = -5) {
            static auto method4 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("Raycast"), 4).cast<Structures::Unity::RaycastHit2D>();
            if (method4.IsValid()) return method4(origin, direction, distance, layerMask);
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("Raycast"), 3).cast<Structures::Unity::RaycastHit2D>();
            if (method3.IsValid()) return method3(origin, direction, distance);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("Raycast"), 2).cast<Structures::Unity::RaycastHit2D>();
            if (method2.IsValid()) return method2(origin, direction);
            return {};
        }

        /**
            @brief Casts a ray through the Scene and returns all hits.
            @param origin Point in world space.
            @param direction Direction vector.
            @param distance Max distance.
            @param layerMask Layer mask.
            @return Mono Array of RaycastHit2D.
        */
        static inline Structures::Mono::Array<Structures::Unity::RaycastHit2D> *RaycastAll(Structures::Unity::Vector2 origin, Structures::Unity::Vector2 direction, float distance = std::numeric_limits<float>::infinity(), int layerMask = -5) {
            static auto method4 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("RaycastAll"), 4).cast<Structures::Mono::Array<Structures::Unity::RaycastHit2D> *>();
            if (method4.IsValid()) return method4(origin, direction, distance, layerMask);
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("RaycastAll"), 3).cast<Structures::Mono::Array<Structures::Unity::RaycastHit2D> *>();
            if (method3.IsValid()) return method3(origin, direction, distance);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("RaycastAll"), 2).cast<Structures::Mono::Array<Structures::Unity::RaycastHit2D> *>();
            if (method2.IsValid()) return method2(origin, direction);
            return nullptr;
        }

        /**
            @brief Checks if a collider falls within a circular area.
            @param point Center of the circle.
            @param radius Radius of circle.
            @param layerMask Layer mask.
            @return Collider2D pointer or nullptr.
        */
        static inline Collider2D *OverlapCircle(Structures::Unity::Vector2 point, float radius, int layerMask = -5) {
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("OverlapCircle"), 3).cast<Collider2D *>();
            if (method3.IsValid()) return method3(point, radius, layerMask);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("OverlapCircle"), 2).cast<Collider2D *>();
            if (method2.IsValid()) return method2(point, radius);
            return nullptr;
        }

        /**
            @brief Checks if a collider falls within a box area.
            @param point Center of box.
            @param size Width and height.
            @param angle Rotation angle.
            @param layerMask Layer mask.
            @return Collider2D pointer or nullptr.
        */
        static inline Collider2D *OverlapBox(Structures::Unity::Vector2 point, Structures::Unity::Vector2 size, float angle, int layerMask = -5) {
            static auto method4 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("OverlapBox"), 4).cast<Collider2D *>();
            if (method4.IsValid()) return method4(point, size, angle, layerMask);
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("OverlapBox"), 3).cast<Collider2D *>();
            if (method3.IsValid()) return method3(point, size, angle);
            return nullptr;
        }

        /**
            @brief Checks if a collider overlaps a point in space.
            @param point Point in world space.
            @param layerMask Layer mask.
            @return Collider2D pointer or nullptr.
        */
        static inline Collider2D *OverlapPoint(Structures::Unity::Vector2 point, int layerMask = -5) {
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("OverlapPoint"), 2).cast<Collider2D *>();
            if (method2.IsValid()) return method2(point, layerMask);
            static auto method1 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("OverlapPoint"), 1).cast<Collider2D *>();
            if (method1.IsValid()) return method1(point);
            return nullptr;
        }

        /**
            @brief Casts a line segment against colliders in the Scene.
            @param start Start point.
            @param end End point.
            @param layerMask Layer mask.
            @return RaycastHit2D structure.
        */
        static inline Structures::Unity::RaycastHit2D Linecast(Structures::Unity::Vector2 start, Structures::Unity::Vector2 end, int layerMask = -5) {
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("Linecast"), 3).cast<Structures::Unity::RaycastHit2D>();
            if (method3.IsValid()) return method3(start, end, layerMask);
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("Linecast"), 2).cast<Structures::Unity::RaycastHit2D>();
            if (method2.IsValid()) return method2(start, end);
            return {};
        }

        /**
            @brief Returns whether collisions between layer1 and layer2 are ignored.
            @param layer1 First layer index.
            @param layer2 Second layer index.
            @return True if collisions are ignored.
        */
        static inline bool GetIgnoreLayerCollision(int layer1, int layer2) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("GetIgnoreLayerCollision"), 2).cast<bool>();
            if (method.IsValid()) return method(layer1, layer2);
            return false;
        }

        /**
            @brief Makes collision detection ignore all collisions between collider1 and collider2.
            @param collider1 First 2D collider.
            @param collider2 Second 2D collider.
            @param ignore True to ignore collisions.
        */
        static inline void IgnoreCollision(Collider2D *collider1, Collider2D *collider2, bool ignore = true) {
            if (!collider1 || !collider2) return;
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("IgnoreCollision"), 3).cast<void>();
            if (method.IsValid()) method(collider1, collider2, ignore);
        }

        /**
            @brief Makes collision detection ignore collisions between layer1 and layer2.
            @param layer1 First layer index.
            @param layer2 Second layer index.
            @param ignore True to ignore collisions.
        */
        static inline void IgnoreLayerCollision(int layer1, int layer2, bool ignore = true) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("IgnoreLayerCollision"), 3).cast<void>();
            if (method.IsValid()) method(layer1, layer2, ignore);
        }

        /**
            @brief Syncs Transform position and rotation changes with the 2D physics engine.
            @note Unity Version Aware: Available in Unity 2017.2+, safe no-op on older Unity versions.
        */
        static inline void SyncTransforms() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Physics2D")).GetMethod(BNM_OBFUSCATE("SyncTransforms"), 0).cast<void>();
            if (method.IsValid()) method();
        }
    };
}

#endif
