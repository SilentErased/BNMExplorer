#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Object.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Base class for all yield instructions in Unity coroutines.
    */
    struct YieldInstruction : public Object {};

    /**
        @brief Asynchronous operation coroutine object.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct AsyncOperation : public YieldInstruction {

        /**
            @brief Has the operation finished?
            @return True if operation is done.
        */
        inline bool GetIsDone() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<AsyncOperation>().ToClass().GetMethod(BNM_OBFUSCATE("get_isDone"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief What's the operation's progress? (0.0 to 1.0).
            @return Progress float.
        */
        inline float GetProgress() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<AsyncOperation>().ToClass().GetMethod(BNM_OBFUSCATE("get_progress"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Priority lets you tweak in which order async operation calls will be performed.
            @return Priority integer.
        */
        inline int GetPriority() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<AsyncOperation>().ToClass().GetMethod(BNM_OBFUSCATE("get_priority"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets priority for the async operation.
            @param value Priority integer.
        */
        inline void SetPriority(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AsyncOperation>().ToClass().GetMethod(BNM_OBFUSCATE("set_priority"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Allow Scenes to be activated as soon as it is ready.
            @return True if scene activation is allowed.
        */
        inline bool GetAllowSceneActivation() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<AsyncOperation>().ToClass().GetMethod(BNM_OBFUSCATE("get_allowSceneActivation"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Allow Scenes to be activated as soon as it is ready.
            @param value Set to false to prevent the scene from loading once it reaches 90% progress.
        */
        inline void SetAllowSceneActivation(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AsyncOperation>().ToClass().GetMethod(BNM_OBFUSCATE("set_allowSceneActivation"), 1).cast<void>();
            method[(void *)this](value);
        }
    };
}
