#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"

#ifdef BNM_UNITY_ANIMATION

#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Object.hpp"
#include "Behaviour.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Stores keyframe based animations.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct AnimationClip : public Object {

        /**
            @brief Animation length in seconds.
            @return Length in seconds float.
        */
        inline float GetLength() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<AnimationClip>().ToClass().GetMethod(BNM_OBFUSCATE("get_length"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Frame rate at which keyframes are sampled.
            @return Frame rate float.
        */
        inline float GetFrameRate() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<AnimationClip>().ToClass().GetMethod(BNM_OBFUSCATE("get_frameRate"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets frame rate at which keyframes are sampled.
            @param value Frame rate float.
        */
        inline void SetFrameRate(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AnimationClip>().ToClass().GetMethod(BNM_OBFUSCATE("set_frameRate"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Sets the default wrap mode used in the animation state (0: Once, 1: Loop, 2: PingPong, 4: ClampForever).
            @return WrapMode integer.
        */
        inline int GetWrapMode() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<AnimationClip>().ToClass().GetMethod(BNM_OBFUSCATE("get_wrapMode"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets wrap mode for this animation clip.
            @param value WrapMode integer.
        */
        inline void SetWrapMode(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AnimationClip>().ToClass().GetMethod(BNM_OBFUSCATE("set_wrapMode"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Set to true if the AnimationClip will be used with the Legacy Animation component.
            @return True if legacy animation.
        */
        inline bool GetLegacy() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<AnimationClip>().ToClass().GetMethod(BNM_OBFUSCATE("get_legacy"), 0).cast<bool>();
            if (method.IsValid()) return method[(void *)this]();
            return false;
        }

        /**
            @brief Sets whether the AnimationClip is used with Legacy Animation.
            @param value True for legacy.
        */
        inline void SetLegacy(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AnimationClip>().ToClass().GetMethod(BNM_OBFUSCATE("set_legacy"), 1).cast<void>();
            if (method.IsValid()) method[(void *)this](value);
        }

        /**
            @brief Returns true if the animation contains curve that loops.
            @return True if looping.
        */
        inline bool GetIsLooping() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<AnimationClip>().ToClass().GetMethod(BNM_OBFUSCATE("get_isLooping"), 0).cast<bool>();
            if (method.IsValid()) return method[(void *)this]();
            return GetWrapMode() == 1 || GetWrapMode() == 2;
        }
    };

    /**
        @brief The Animation component is used to play back legacy animations.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Animation : public Behaviour {

        /**
            @brief Should the default animation clip automatically start playing on startup?
            @return True if playing automatically.
        */
        inline bool GetPlayAutomatically() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("get_playAutomatically"), 0).cast<bool>();
            if (method.IsValid()) return method[(void *)this]();
            return false;
        }

        /**
            @brief Sets whether the default animation starts playing automatically.
            @param value True to play automatically.
        */
        inline void SetPlayAutomatically(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("set_playAutomatically"), 1).cast<void>();
            if (method.IsValid()) method[(void *)this](value);
        }

        /**
            @brief Plays the default animation.
            @return True if started playing.
        */
        inline bool Play() {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("Play"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Plays animation named name.
            @param name Name of animation to play.
            @return True if started playing.
        */
        inline bool Play(const std::string_view &name) {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("Play"), 1).cast<bool>();
            return method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Plays animation named name with playMode (0: StopSameLayer, 4: StopAll).
            @param name Animation name.
            @param playMode PlayMode integer.
            @return True if started playing.
        */
        inline bool Play(const std::string_view &name, int playMode) {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("Play"), 2).cast<bool>();
            if (method.IsValid()) return method[(void *)this](Structures::Mono::String::Create(name), playMode);
            return Play(name);
        }

        /**
            @brief Fades the animation named name in over a period of time and fades other animations out.
            @param name Name of animation.
            @param fadeLength Length of cross fade in seconds.
            @param playMode PlayMode integer.
        */
        inline void CrossFade(const std::string_view &name, float fadeLength = 0.3f, int playMode = 0) {
            if (!IsValid()) return;
            static auto method3 = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("CrossFade"), 3).cast<void>();
            if (method3.IsValid()) {
                method3[(void *)this](Structures::Mono::String::Create(name), fadeLength, playMode);
                return;
            }
            static auto method2 = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("CrossFade"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](Structures::Mono::String::Create(name), fadeLength);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("CrossFade"), 1).cast<void>();
            if (method1.IsValid()) method1[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Blends the animation named name towards targetWeight over the next time seconds.
            @param name Animation name.
            @param targetWeight Target weight (0.0 to 1.0).
            @param fadeLength Fade duration in seconds.
        */
        inline void Blend(const std::string_view &name, float targetWeight = 1.0f, float fadeLength = 0.3f) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("Blend"), 3).cast<void>();
            if (method.IsValid()) method[(void *)this](Structures::Mono::String::Create(name), targetWeight, fadeLength);
        }

        /**
            @brief Stops all playing animations that were started with this Animation.
        */
        inline void Stop() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("Stop"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Stops an animation named name.
            @param name Animation name to stop.
        */
        inline void Stop(const std::string_view &name) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("Stop"), 1).cast<void>();
            if (method.IsValid()) method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Is the animation named name playing?
            @param name Animation name to check.
            @return True if playing.
        */
        inline bool IsPlaying(const std::string_view &name) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("IsPlaying"), 1).cast<bool>();
            if (method.IsValid()) return method[(void *)this](Structures::Mono::String::Create(name));
            return false;
        }

        /**
            @brief Are there any animations playing?
            @return True if any animation is playing.
        */
        inline bool GetIsPlaying() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("get_isPlaying"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Are there any animations playing? (Alias for GetIsPlaying)
            @return True if any animation is playing.
        */
        inline bool IsPlaying() const {
            return GetIsPlaying();
        }

        /**
            @brief Get the AnimationClip named name.
            @param name Clip name.
            @return AnimationClip pointer.
        */
        inline AnimationClip *GetClip(const std::string_view &name) const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("GetClip"), 1).cast<AnimationClip *>();
            if (method.IsValid()) return method[(void *)this](Structures::Mono::String::Create(name));
            return nullptr;
        }

        /**
            @brief Get the number of clips currently assigned to this animation.
            @return Clip count integer.
        */
        inline int GetClipCount() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("GetClipCount"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Adds a clip to the animation with name newName.
            @param clip AnimationClip to add.
            @param newName Name of the clip.
        */
        inline void AddClip(AnimationClip *clip, const std::string_view &newName) {
            if (!IsValid() || !clip) return;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("AddClip"), 2).cast<void>();
            if (method.IsValid()) method[(void *)this]((void *)clip, Structures::Mono::String::Create(newName));
        }

        /**
            @brief Remove clip from the animation list.
            @param clip AnimationClip to remove.
        */
        inline void RemoveClip(AnimationClip *clip) {
            if (!IsValid() || !clip) return;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("RemoveClip"), 1).cast<void>();
            if (method.IsValid()) method[(void *)this]((void *)clip);
        }

        /**
            @brief Remove clip from the animation list by name.
            @param clipName Name of clip to remove.
        */
        inline void RemoveClip(const std::string_view &clipName) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("RemoveClip"), 1).cast<void>();
            if (method.IsValid()) method[(void *)this](Structures::Mono::String::Create(clipName));
        }

        /**
            @brief How should time beyond the playback range of the clip be treated?
            @return WrapMode integer.
        */
        inline int GetWrapMode() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("get_wrapMode"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets how time beyond playback range is treated.
            @param value WrapMode integer.
        */
        inline void SetWrapMode(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("set_wrapMode"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief When turned on, animations will be executed in the physics loop.
            @return True if animate physics is enabled.
        */
        inline bool GetAnimatePhysics() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("get_animatePhysics"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether animations are executed in physics loop.
            @param value True to animate in physics loop.
        */
        inline void SetAnimatePhysics(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animation>().ToClass().GetMethod(BNM_OBFUSCATE("set_animatePhysics"), 1).cast<void>();
            method[(void *)this](value);
        }
    };

    /**
        @brief Interface to control the Mecanim animation system.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Animator : public Behaviour {

        /**
            @brief Plays a state.
            @param stateName Name of the state.
            @param layer Layer index (-1 is default layer).
            @param normalizedTime Normalized playback start time (0.0 to 1.0).
        */
        inline void Play(const std::string_view &stateName, int layer = -1, float normalizedTime = 0.0f) {
            if (!IsValid()) return;
            static auto method3 = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("Play"), 3).cast<void>();
            if (method3.IsValid()) {
                method3[(void *)this](Structures::Mono::String::Create(stateName), layer, normalizedTime);
                return;
            }
            static auto method2 = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("Play"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](Structures::Mono::String::Create(stateName), layer);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("Play"), 1).cast<void>();
            if (method1.IsValid()) method1[(void *)this](Structures::Mono::String::Create(stateName));
        }

        /**
            @brief Plays a state by state hash.
            @param stateNameHash Hash of state name.
            @param layer Layer index.
            @param normalizedTime Normalized time.
        */
        inline void Play(int stateNameHash, int layer = -1, float normalizedTime = 0.0f) {
            if (!IsValid()) return;
            static auto method3 = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("Play"), {BNM::Defaults::Get<int>().ToClass(), BNM::Defaults::Get<int>().ToClass(), BNM::Defaults::Get<float>().ToClass()}).cast<void>();
            if (method3.IsValid()) {
                method3[(void *)this](stateNameHash, layer, normalizedTime);
                return;
            }
            static auto method1 = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("Play"), {BNM::Defaults::Get<int>().ToClass()}).cast<void>();
            if (method1.IsValid()) method1[(void *)this](stateNameHash);
        }

        /**
            @brief Plays a state in fixed time seconds.
            @param stateName Name of state.
            @param layer Layer index.
            @param fixedTime Fixed time in seconds.
        */
        inline void PlayInFixedTime(const std::string_view &stateName, int layer = -1, float fixedTime = 0.0f) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("PlayInFixedTime"), 3).cast<void>();
            if (method.IsValid()) method[(void *)this](Structures::Mono::String::Create(stateName), layer, fixedTime);
        }

        /**
            @brief Plays a state in fixed time seconds by hash.
            @param stateNameHash State hash.
            @param layer Layer index.
            @param fixedTime Fixed time in seconds.
        */
        inline void PlayInFixedTime(int stateNameHash, int layer = -1, float fixedTime = 0.0f) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("PlayInFixedTime"), {BNM::Defaults::Get<int>().ToClass(), BNM::Defaults::Get<int>().ToClass(), BNM::Defaults::Get<float>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](stateNameHash, layer, fixedTime);
        }

        /**
            @brief Creates a crossfade from current state to other state using normalized time.
            @param stateName Target state name.
            @param normalizedTransitionDuration Transition duration in normalized time.
            @param layer Layer index (-1 default).
            @param normalizedTime Start time in target state.
        */
        inline void CrossFade(const std::string_view &stateName, float normalizedTransitionDuration, int layer = -1, float normalizedTime = 0.0f) {
            if (!IsValid()) return;
            static auto method4 = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("CrossFade"), 4).cast<void>();
            if (method4.IsValid()) {
                method4[(void *)this](Structures::Mono::String::Create(stateName), normalizedTransitionDuration, layer, normalizedTime);
                return;
            }
            static auto method2 = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("CrossFade"), 2).cast<void>();
            if (method2.IsValid()) method2[(void *)this](Structures::Mono::String::Create(stateName), normalizedTransitionDuration);
        }

        /**
            @brief Creates a crossfade by state hash.
            @param stateNameHash Target state hash.
            @param normalizedTransitionDuration Transition duration.
            @param layer Layer index.
            @param normalizedTime Start time.
        */
        inline void CrossFade(int stateNameHash, float normalizedTransitionDuration, int layer = -1, float normalizedTime = 0.0f) {
            if (!IsValid()) return;
            static auto method4 = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("CrossFade"), {BNM::Defaults::Get<int>().ToClass(), BNM::Defaults::Get<float>().ToClass(), BNM::Defaults::Get<int>().ToClass(), BNM::Defaults::Get<float>().ToClass()}).cast<void>();
            if (method4.IsValid()) {
                method4[(void *)this](stateNameHash, normalizedTransitionDuration, layer, normalizedTime);
                return;
            }
            static auto method2 = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("CrossFade"), {BNM::Defaults::Get<int>().ToClass(), BNM::Defaults::Get<float>().ToClass()}).cast<void>();
            if (method2.IsValid()) method2[(void *)this](stateNameHash, normalizedTransitionDuration);
        }

        /**
            @brief Returns the value of the given float parameter.
            @param name Parameter name.
            @return Float value.
        */
        inline float GetFloat(const std::string_view &name) const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("GetFloat"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<float>();
            if (method.IsValid()) return method[(void *)this](Structures::Mono::String::Create(name));
            return 0.0f;
        }

        /**
            @brief Returns the value of the given float parameter by hash id.
            @param id Parameter hash ID.
            @return Float value.
        */
        inline float GetFloat(int id) const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("GetFloat"), {BNM::Defaults::Get<int>().ToClass()}).cast<float>();
            if (method.IsValid()) return method[(void *)this](id);
            return 0.0f;
        }

        /**
            @brief Sets the value of the given float parameter.
            @param name Parameter name.
            @param value Float value.
        */
        inline void SetFloat(const std::string_view &name, float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("SetFloat"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass(), BNM::Defaults::Get<float>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](Structures::Mono::String::Create(name), value);
        }

        /**
            @brief Sets the value of the given float parameter by hash id.
            @param id Parameter hash ID.
            @param value Float value.
        */
        inline void SetFloat(int id, float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("SetFloat"), {BNM::Defaults::Get<int>().ToClass(), BNM::Defaults::Get<float>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](id, value);
        }

        /**
            @brief Returns the value of the given bool parameter.
            @param name Parameter name.
            @return Boolean value.
        */
        inline bool GetBool(const std::string_view &name) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("GetBool"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<bool>();
            if (method.IsValid()) return method[(void *)this](Structures::Mono::String::Create(name));
            return false;
        }

        /**
            @brief Returns the value of the given bool parameter by hash id.
            @param id Parameter hash ID.
            @return Boolean value.
        */
        inline bool GetBool(int id) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("GetBool"), {BNM::Defaults::Get<int>().ToClass()}).cast<bool>();
            if (method.IsValid()) return method[(void *)this](id);
            return false;
        }

        /**
            @brief Sets the value of the given bool parameter.
            @param name Parameter name.
            @param value Boolean value.
        */
        inline void SetBool(const std::string_view &name, bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("SetBool"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass(), BNM::Defaults::Get<bool>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](Structures::Mono::String::Create(name), value);
        }

        /**
            @brief Sets the value of the given bool parameter by hash id.
            @param id Parameter hash ID.
            @param value Boolean value.
        */
        inline void SetBool(int id, bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("SetBool"), {BNM::Defaults::Get<int>().ToClass(), BNM::Defaults::Get<bool>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](id, value);
        }

        /**
            @brief Returns the value of the given integer parameter.
            @param name Parameter name.
            @return Integer value.
        */
        inline int GetInteger(const std::string_view &name) const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("GetInteger"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<int>();
            if (method.IsValid()) return method[(void *)this](Structures::Mono::String::Create(name));
            return 0;
        }

        /**
            @brief Returns the value of the given integer parameter by hash id.
            @param id Parameter hash ID.
            @return Integer value.
        */
        inline int GetInteger(int id) const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("GetInteger"), {BNM::Defaults::Get<int>().ToClass()}).cast<int>();
            if (method.IsValid()) return method[(void *)this](id);
            return 0;
        }

        /**
            @brief Sets the value of the given integer parameter.
            @param name Parameter name.
            @param value Integer value.
        */
        inline void SetInteger(const std::string_view &name, int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("SetInteger"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass(), BNM::Defaults::Get<int>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](Structures::Mono::String::Create(name), value);
        }

        /**
            @brief Sets the value of the given integer parameter by hash id.
            @param id Parameter hash ID.
            @param value Integer value.
        */
        inline void SetInteger(int id, int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("SetInteger"), {BNM::Defaults::Get<int>().ToClass(), BNM::Defaults::Get<int>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](id, value);
        }

        /**
            @brief Sets the trigger parameter to active.
            @param name Trigger name.
        */
        inline void SetTrigger(const std::string_view &name) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("SetTrigger"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Sets the trigger parameter to active by hash id.
            @param id Trigger hash ID.
        */
        inline void SetTrigger(int id) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("SetTrigger"), {BNM::Defaults::Get<int>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](id);
        }

        /**
            @brief Resets the trigger parameter to inactive.
            @param name Trigger name.
        */
        inline void ResetTrigger(const std::string_view &name) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("ResetTrigger"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](Structures::Mono::String::Create(name));
        }

        /**
            @brief Resets the trigger parameter to inactive by hash id.
            @param id Trigger hash ID.
        */
        inline void ResetTrigger(int id) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("ResetTrigger"), {BNM::Defaults::Get<int>().ToClass()}).cast<void>();
            if (method.IsValid()) method[(void *)this](id);
        }

        /**
            @brief Returns true if the parameter is controlled by a curve.
            @param name Parameter name.
            @return True if controlled by curve.
        */
        inline bool IsParameterControlledByCurve(const std::string_view &name) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("IsParameterControlledByCurve"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<bool>();
            if (method.IsValid()) return method[(void *)this](Structures::Mono::String::Create(name));
            return false;
        }

        /**
            @brief Returns true if the parameter is controlled by a curve.
            @param id Parameter hash id.
            @return True if controlled by curve.
        */
        inline bool IsParameterControlledByCurve(int id) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("IsParameterControlledByCurve"), {BNM::Defaults::Get<int>().ToClass()}).cast<bool>();
            if (method.IsValid()) return method[(void *)this](id);
            return false;
        }

        /**
            @brief The playback speed of the Animator. 1.0 is normal playback speed.
            @return Speed multiplier float.
        */
        inline float GetSpeed() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("get_speed"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the playback speed of the Animator. (Useful for animation speedhack).
            @param value Speed multiplier float (0.0 pauses animation, 2.0 is 2x speed).
        */
        inline void SetSpeed(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("set_speed"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Should root motion be applied?
            @return True if root motion enabled.
        */
        inline bool GetApplyRootMotion() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("get_applyRootMotion"), 0).cast<bool>();
            if (method.IsValid()) return method[(void *)this]();
            return false;
        }

        /**
            @brief Sets whether root motion is applied.
            @param value True to apply root motion.
        */
        inline void SetApplyRootMotion(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("set_applyRootMotion"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Controls culling of this Animator component (0: AlwaysAnimate, 1: CullUpdateTransforms, 2: CullCompletely).
            @return AnimatorCullingMode integer.
        */
        inline int GetCullingMode() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("get_cullingMode"), 0).cast<int>();
            if (method.IsValid()) return method[(void *)this]();
            return 0;
        }

        /**
            @brief Sets the culling mode of this Animator.
            @param value AnimatorCullingMode integer.
        */
        inline void SetCullingMode(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("set_cullingMode"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The number of layers in the controller.
            @return Layer count integer.
        */
        inline int GetLayerCount() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("get_layerCount"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Returns the layer name.
            @param layerIndex Layer index.
            @return Layer name Mono String.
        */
        inline Structures::Mono::String *GetLayerName(int layerIndex) const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("GetLayerName"), 1).cast<Structures::Mono::String *>();
            return method[(void *)this](layerIndex);
        }

        /**
            @brief Returns the index of the layer with the given name.
            @param layerName Layer name.
            @return Layer index integer.
        */
        inline int GetLayerIndex(const std::string_view &layerName) const {
            if (!IsValid()) return -1;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("GetLayerIndex"), 1).cast<int>();
            return method[(void *)this](Structures::Mono::String::Create(layerName));
        }

        /**
            @brief Returns the weight of the layer at the specified index.
            @param layerIndex Layer index.
            @return Layer weight float (0.0 to 1.0).
        */
        inline float GetLayerWeight(int layerIndex) const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("GetLayerWeight"), 1).cast<float>();
            return method[(void *)this](layerIndex);
        }

        /**
            @brief Sets the weight of the layer at the specified index.
            @param layerIndex Layer index.
            @param weight Layer weight float (0.0 to 1.0).
        */
        inline void SetLayerWeight(int layerIndex, float weight) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("SetLayerWeight"), 2).cast<void>();
            method[(void *)this](layerIndex, weight);
        }

        /**
            @brief Gets the current State information on given layer.
            @param layerIndex Layer index.
            @return AnimatorStateInfo as Il2CppObject pointer.
        */
        inline IL2CPP::Il2CppObject *GetCurrentAnimatorStateInfo(int layerIndex) const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("GetCurrentAnimatorStateInfo"), 1).cast<IL2CPP::Il2CppObject *>();
            if (method.IsValid()) return method[(void *)this](layerIndex);
            return nullptr;
        }

        /**
            @brief Returns true if the state exists on the specified layer.
            @param layerIndex Layer index.
            @param stateID State hash ID.
            @return True if state exists.
        */
        inline bool GetHasState(int layerIndex, int stateID) const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("HasState"), 2).cast<bool>();
            if (method.IsValid()) return method[(void *)this](layerIndex, stateID);
            return false;
        }

        /**
            @brief Rebind all the animated properties and mesh data with the Animator.
        */
        inline void Rebind() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("Rebind"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Evaluates the animator based on deltaTime.
            @param deltaTime Delta time float.
        */
        inline void Update(float deltaTime) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Animator>().ToClass().GetMethod(BNM_OBFUSCATE("Update"), 1).cast<void>();
            method[(void *)this](deltaTime);
        }

        /**
            @brief Generates an parameter id from a string.
            @param name Parameter or state name.
            @return Hash integer.
        */
        static inline int StringToHash(const std::string_view &name) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Animator")).GetMethod(BNM_OBFUSCATE("StringToHash"), 1).cast<int>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(name));
            return 0;
        }

        /**
            @brief Generates an parameter id from a Mono String.
            @param name Parameter or state Mono string.
            @return Hash integer.
        */
        static inline int StringToHash(Structures::Mono::String *name) {
            if (!name) return 0;
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Animator")).GetMethod(BNM_OBFUSCATE("StringToHash"), 1).cast<int>();
            if (method.IsValid()) return method(name);
            return 0;
        }
    };
}

#endif
