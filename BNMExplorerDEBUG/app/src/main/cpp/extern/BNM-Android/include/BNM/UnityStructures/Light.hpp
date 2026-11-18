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
#include "Color.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Script interface for light components.
    */
    struct Light : public Behaviour {

        /**
            @brief Gets the color of the light.
            @return Color structure.
        */
        inline Structures::Unity::Color GetColor() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("get_color"), 0).cast<Structures::Unity::Color>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the color of the light.
            @param value New light color.
        */
        inline void SetColor(Structures::Unity::Color value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("set_color"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the brightness of the light.
            @return Light intensity multiplier.
        */
        inline float GetIntensity() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("get_intensity"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the brightness of the light.
            @param value Light intensity.
        */
        inline void SetIntensity(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("set_intensity"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the multiplier that defines the strength of the bounce light.
            @return Bounce intensity multiplier.
        */
        inline float GetBounceIntensity() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("get_bounceIntensity"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the multiplier that defines the strength of the bounce light.
            @param value Bounce intensity.
        */
        inline void SetBounceIntensity(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("set_bounceIntensity"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the range of the light in world units (Point/Spot light).
            @return Range in meters.
        */
        inline float GetRange() const {
            if (!IsValid()) return 10.0f;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("get_range"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the range of the light in world units.
            @param value Range in meters.
        */
        inline void SetRange(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("set_range"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the type of the light (0 = Spot, 1 = Directional, 2 = Point, 3 = Area).
            @return LightType enum integer.
        */
        inline int GetType() const {
            if (!IsValid()) return 1;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("get_type"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the type of the light.
            @param value LightType enum integer (0 = Spot, 1 = Directional, 2 = Point, 3 = Area).
        */
        inline void SetType(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("set_type"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets shadow casting options for the light (0 = None, 1 = Hard, 2 = Soft).
            @return LightShadows enum integer.
        */
        inline int GetShadows() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("get_shadows"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets shadow casting options for the light.
            @param value LightShadows enum integer (0 = None, 1 = Hard, 2 = Soft).
        */
        inline void SetShadows(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("set_shadows"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the angle of the light's spotlight cone in degrees.
            @return Spot angle degrees.
        */
        inline float GetSpotAngle() const {
            if (!IsValid()) return 30.0f;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("get_spotAngle"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the angle of the light's spotlight cone in degrees.
            @param value Spot angle degrees.
        */
        inline void SetSpotAngle(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Light>().ToClass().GetMethod(BNM_OBFUSCATE("set_spotAngle"), 1).cast<void>();
            method[(void *)this](value);
        }
    };
}
