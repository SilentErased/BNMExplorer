#pragma once

#include "Component.hpp"

namespace BNM::UnityEngine {
    /**
        @brief UnityEngine.Behaviour implementation.
        Behaviours are Components that can be enabled or disabled.
    */
    struct Behaviour : public Component {
        constexpr Behaviour() : Component() {}

        /**
            @brief Enabled Behaviours are Updated, disabled Behaviours are not.
            @return True if the Behaviour is enabled.
        */
        inline bool GetEnabled() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Behaviour>().ToClass().GetMethod(BNM_OBFUSCATE("get_enabled"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Enables or disables the Behaviour.
            @param value Set to true to enable, false to disable.
        */
        inline void SetEnabled(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Behaviour>().ToClass().GetMethod(BNM_OBFUSCATE("set_enabled"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Has the Behaviour had active and enabled called? True if GameObject is active in hierarchy and Behaviour is enabled.
            @return True if both active and enabled.
        */
        inline bool GetIsActiveAndEnabled() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Behaviour>().ToClass().GetMethod(BNM_OBFUSCATE("get_isActiveAndEnabled"), 0).cast<bool>();
            return method[(void *)this]();
        }
    };
}
