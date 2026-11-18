#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"

#ifdef BNM_UNITY_AUDIO

#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Object.hpp"
#include "Behaviour.hpp"
#include "Vector3.hpp"

namespace BNM::UnityEngine {

    /**
        @brief A container for audio data.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct AudioClip : public Object {

        /**
            @brief The length of the audio clip in seconds.
            @return Length in seconds float.
        */
        inline float GetLength() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<AudioClip>().ToClass().GetMethod(BNM_OBFUSCATE("get_length"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief The length of the audio clip in samples.
            @return Sample count integer.
        */
        inline int GetSamples() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<AudioClip>().ToClass().GetMethod(BNM_OBFUSCATE("get_samples"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief The number of channels in the audio clip (1: Mono, 2: Stereo).
            @return Channel count integer.
        */
        inline int GetChannels() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<AudioClip>().ToClass().GetMethod(BNM_OBFUSCATE("get_channels"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief The sample frequency of the clip in Hertz (Hz).
            @return Frequency in Hz integer.
        */
        inline int GetFrequency() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<AudioClip>().ToClass().GetMethod(BNM_OBFUSCATE("get_frequency"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Loads the audio data from this clip.
            @return True if loaded successfully.
        */
        inline bool LoadAudioData() {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<AudioClip>().ToClass().GetMethod(BNM_OBFUSCATE("LoadAudioData"), 0).cast<bool>();
            if (method.IsValid()) return method[(void *)this]();
            return false;
        }

        /**
            @brief Unloads the audio data associated with the clip.
            @return True if unloaded successfully.
        */
        inline bool UnloadAudioData() {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<AudioClip>().ToClass().GetMethod(BNM_OBFUSCATE("UnloadAudioData"), 0).cast<bool>();
            if (method.IsValid()) return method[(void *)this]();
            return false;
        }

        /**
            @brief Returns the current load state of the audio data associated with an AudioClip (0: Unloaded, 1: Loading, 2: Loaded, 3: Failed).
            @return AudioDataLoadState integer.
        */
        inline int GetLoadState() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<AudioClip>().ToClass().GetMethod(BNM_OBFUSCATE("get_loadState"), 0).cast<int>();
            return method[(void *)this]();
        }
    };

    /**
        @brief A representation of audio sources in 3D / 2D space.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct AudioSource : public Behaviour {

        /**
            @brief The default AudioClip to play.
            @return AudioClip pointer.
        */
        inline AudioClip *GetClip() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("get_clip"), 0).cast<AudioClip *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the default AudioClip to play.
            @param clip AudioClip pointer.
        */
        inline void SetClip(AudioClip *clip) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("set_clip"), 1).cast<void>();
            method[(void *)this]((void *)clip);
        }

        /**
            @brief Plays the clip.
        */
        inline void Play() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("Play"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Plays the clip with an obsolete delay parameter.
            @param delay Delay in milliseconds.
        */
        inline void Play(unsigned long delay) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("Play"), 1).cast<void>();
            if (method.IsValid()) method[(void *)this](delay);
            else Play();
        }

        /**
            @brief Plays the clip with a delay specified in seconds.
            @param delay Delay in seconds.
        */
        inline void PlayDelayed(float delay) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("PlayDelayed"), 1).cast<void>();
            if (method.IsValid()) method[(void *)this](delay);
            else Play();
        }

        /**
            @brief Plays the clip at a specific time in the absolute time system.
            @param time Absolute time in seconds.
        */
        inline void PlayScheduled(double time) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("PlayScheduled"), 1).cast<void>();
            if (method.IsValid()) method[(void *)this](time);
            else Play();
        }

        /**
            @brief Plays an AudioClip, and scales the AudioSource volume by volumeScale.
            @param clip Audio clip to play.
            @param volumeScale Volume multiplier.
        */
        inline void PlayOneShot(AudioClip *clip, float volumeScale = 1.0f) {
            if (!IsValid() || !clip) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("PlayOneShot"), 2).cast<void>();
            if (method.IsValid()) {
                method[(void *)this]((void *)clip, volumeScale);
                return;
            }
            static auto method1 = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("PlayOneShot"), 1).cast<void>();
            if (method1.IsValid()) method1[(void *)this]((void *)clip);
        }

        /**
            @brief Pauses playing the current audio clip.
        */
        inline void Pause() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("Pause"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Unpauses the paused audio clip.
        */
        inline void UnPause() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("UnPause"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Stops playing the audio clip.
        */
        inline void Stop() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("Stop"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Is the clip playing right now?
            @return True if playing.
        */
        inline bool GetIsPlaying() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("get_isPlaying"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Is the audio clip looping?
            @return True if looping.
        */
        inline bool GetLoop() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("get_loop"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether the audio clip loops.
            @param value True to loop.
        */
        inline void SetLoop(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("set_loop"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The volume of the audio source (0.0 to 1.0).
            @return Volume float.
        */
        inline float GetVolume() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("get_volume"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the volume of the audio source (0.0 to 1.0).
            @param value Volume float.
        */
        inline void SetVolume(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("set_volume"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The pitch of the audio source.
            @return Pitch float (1.0 is default).
        */
        inline float GetPitch() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("get_pitch"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the pitch of the audio source.
            @param value Pitch float.
        */
        inline void SetPitch(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("set_pitch"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Playback position in seconds.
            @return Playback time in seconds.
        */
        inline float GetTime() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("get_time"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the playback position in seconds.
            @param value Time in seconds.
        */
        inline void SetTime(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("set_time"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Sets the how much this AudioSource is affected by 3D spatialisation calculations (0.0: 2D, 1.0: 3D).
            @return Spatial blend float.
        */
        inline float GetSpatialBlend() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("get_spatialBlend"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets spatial blend (0.0: 2D, 1.0: 3D).
            @param value Spatial blend float.
        */
        inline void SetSpatialBlend(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("set_spatialBlend"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Un-mute / Mutes the AudioSource.
            @return True if muted.
        */
        inline bool GetMute() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("get_mute"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets mute status.
            @param value True to mute.
        */
        inline void SetMute(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("set_mute"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Within the MinDistance, the AudioSource will cease to grow louder in volume.
            @return Min distance float.
        */
        inline float GetMinDistance() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("get_minDistance"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets min distance for 3D attenuation.
            @param value Min distance float.
        */
        inline void SetMinDistance(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("set_minDistance"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief MaxDistance is the distance a sound stops attenuating at.
            @return Max distance float.
        */
        inline float GetMaxDistance() const {
            if (!IsValid()) return 500.0f;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("get_maxDistance"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets max distance for 3D attenuation.
            @param value Max distance float.
        */
        inline void SetMaxDistance(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<AudioSource>().ToClass().GetMethod(BNM_OBFUSCATE("set_maxDistance"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Plays an AudioClip at a given position in world space.
            @param clip Audio clip to play.
            @param position Position in world space.
            @param volume Volume scale.
        */
        static inline void PlayClipAtPoint(AudioClip *clip, Structures::Unity::Vector3 position, float volume = 1.0f) {
            if (!clip) return;
            static auto method3 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("AudioSource")).GetMethod(BNM_OBFUSCATE("PlayClipAtPoint"), 3).cast<void>();
            if (method3.IsValid()) {
                method3((void *)clip, position, volume);
                return;
            }
            static auto method2 = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("AudioSource")).GetMethod(BNM_OBFUSCATE("PlayClipAtPoint"), 2).cast<void>();
            if (method2.IsValid()) method2((void *)clip, position);
        }
    };

    /**
        @brief Representation of a listener in 3D / 2D space.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct AudioListener : public Behaviour {

        /**
            @brief Controls the game sound volume (0.0 to 1.0).
            @return Global audio volume float.
        */
        static inline float GetVolume() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("AudioListener")).GetMethod(BNM_OBFUSCATE("get_volume"), 0).cast<float>();
            if (method.IsValid()) return method();
            return 1.0f;
        }

        /**
            @brief Sets the game sound volume.
            @param value Global volume float (0.0 to 1.0).
        */
        static inline void SetVolume(float value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("AudioListener")).GetMethod(BNM_OBFUSCATE("set_volume"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief The paused state of the audio system.
            @return True if global audio is paused.
        */
        static inline bool GetPause() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("AudioListener")).GetMethod(BNM_OBFUSCATE("get_pause"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return false;
        }

        /**
            @brief Pauses/unpauses the entire audio system.
            @param value True to pause audio system.
        */
        static inline void SetPause(bool value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("AudioListener")).GetMethod(BNM_OBFUSCATE("set_pause"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }
    };
}

#endif
