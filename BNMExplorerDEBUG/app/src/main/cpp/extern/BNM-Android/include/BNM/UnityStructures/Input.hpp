#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Structure describing the status of a multi-touch finger gesture.
    */
    struct Touch {
        int m_FingerId{0};
        Structures::Unity::Vector2 m_Position{};
        Structures::Unity::Vector2 m_RawPosition{};
        Structures::Unity::Vector2 m_PositionDelta{};
        float m_TimeDelta{0.0f};
        int m_TapCount{0};
        int m_Phase{0}; // 0: Began, 1: Moved, 2: Stationary, 3: Ended, 4: Canceled
        int m_Type{0};  // 0: Direct, 1: Indirect, 2: Stylus
        float m_Pressure{0.0f};
        float m_MaximumPossiblePressure{0.0f};
        float m_Radius{0.0f};
        float m_RadiusVariance{0.0f};
        float m_AltitudeAngle{0.0f};
        float m_AzimuthAngle{0.0f};

        inline int GetFingerId() const { return m_FingerId; }
        inline Structures::Unity::Vector2 GetPosition() const { return m_Position; }
        inline Structures::Unity::Vector2 GetRawPosition() const { return m_RawPosition; }
        inline Structures::Unity::Vector2 GetDeltaPosition() const { return m_PositionDelta; }
        inline float GetDeltaTime() const { return m_TimeDelta; }
        inline int GetTapCount() const { return m_TapCount; }
        inline int GetPhase() const { return m_Phase; }
        inline int GetType() const { return m_Type; }
        inline float GetPressure() const { return m_Pressure; }
        inline float GetMaximumPossiblePressure() const { return m_MaximumPossiblePressure; }
        inline float GetRadius() const { return m_Radius; }
        inline float GetRadiusVariance() const { return m_RadiusVariance; }
        inline float GetAltitudeAngle() const { return m_AltitudeAngle; }
        inline float GetAzimuthAngle() const { return m_AzimuthAngle; }
    };

    /**
        @brief Interface into the Input system.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Input {
        Input() = delete;

        /**
            @brief Returns true while the user holds down the key identified by the integer KeyCode.
            @param keyCode KeyCode integer value.
            @return True if held down.
        */
        static inline bool GetKey(int keyCode) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetKey"), {BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("KeyCode"))}).cast<bool>();
            if (method.IsValid()) return method(keyCode);
            static auto methodInt = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetKey"), 1).cast<bool>();
            if (methodInt.IsValid()) return methodInt(keyCode);
            return false;
        }

        /**
            @brief Returns true while the user holds down the key identified by name.
            @param name Key name (e.g. "space", "a", "return").
            @return True if held down.
        */
        static inline bool GetKey(const std::string_view &name) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetKey"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<bool>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(name));
            return false;
        }

        /**
            @brief Returns true during the frame the user starts pressing down the key identified by KeyCode.
            @param keyCode KeyCode integer value.
            @return True on key down frame.
        */
        static inline bool GetKeyDown(int keyCode) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetKeyDown"), {BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("KeyCode"))}).cast<bool>();
            if (method.IsValid()) return method(keyCode);
            static auto methodInt = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetKeyDown"), 1).cast<bool>();
            if (methodInt.IsValid()) return methodInt(keyCode);
            return false;
        }

        /**
            @brief Returns true during the frame the user starts pressing down the key identified by name.
            @param name Key name.
            @return True on key down frame.
        */
        static inline bool GetKeyDown(const std::string_view &name) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetKeyDown"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<bool>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(name));
            return false;
        }

        /**
            @brief Returns true during the frame the user releases the key identified by KeyCode.
            @param keyCode KeyCode integer value.
            @return True on key up frame.
        */
        static inline bool GetKeyUp(int keyCode) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetKeyUp"), {BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("KeyCode"))}).cast<bool>();
            if (method.IsValid()) return method(keyCode);
            static auto methodInt = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetKeyUp"), 1).cast<bool>();
            if (methodInt.IsValid()) return methodInt(keyCode);
            return false;
        }

        /**
            @brief Returns true during the frame the user releases the key identified by name.
            @param name Key name.
            @return True on key up frame.
        */
        static inline bool GetKeyUp(const std::string_view &name) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetKeyUp"), {BNM::Defaults::Get<Structures::Mono::String *>().ToClass()}).cast<bool>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(name));
            return false;
        }

        /**
            @brief Returns whether the given mouse button is held down (0 = Left, 1 = Right, 2 = Middle).
            @param button Mouse button index.
            @return True if held down.
        */
        static inline bool GetMouseButton(int button) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetMouseButton"), 1).cast<bool>();
            if (method.IsValid()) return method(button);
            return false;
        }

        /**
            @brief Returns true during the frame the user pressed the given mouse button.
            @param button Mouse button index.
            @return True on button down frame.
        */
        static inline bool GetMouseButtonDown(int button) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetMouseButtonDown"), 1).cast<bool>();
            if (method.IsValid()) return method(button);
            return false;
        }

        /**
            @brief Returns true during the frame the user releases the given mouse button.
            @param button Mouse button index.
            @return True on button up frame.
        */
        static inline bool GetMouseButtonUp(int button) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetMouseButtonUp"), 1).cast<bool>();
            if (method.IsValid()) return method(button);
            return false;
        }

        /**
            @brief The current mouse position in pixel coordinates.
            @return Mouse position as Vector3.
        */
        static inline Structures::Unity::Vector3 GetMousePosition() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_mousePosition"), 0).cast<Structures::Unity::Vector3>();
            if (method.IsValid()) return method();
            return {};
        }

        /**
            @brief The current mouse scroll delta.
            @return Scroll delta as Vector2.
        */
        static inline Structures::Unity::Vector2 GetMouseScrollDelta() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_mouseScrollDelta"), 0).cast<Structures::Unity::Vector2>();
            if (method.IsValid()) return method();
            return {};
        }

        /**
            @brief Returns the value of the virtual axis identified by axisName.
            @param axisName Axis name (e.g. "Horizontal", "Vertical").
            @return Axis value float (-1.0 to 1.0).
        */
        static inline float GetAxis(const std::string_view &axisName) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetAxis"), 1).cast<float>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(axisName));
            return 0.0f;
        }

        /**
            @brief Returns the value of the virtual axis with no smoothing filtering applied.
            @param axisName Axis name.
            @return Raw axis value float (-1, 0, or 1).
        */
        static inline float GetAxisRaw(const std::string_view &axisName) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetAxisRaw"), 1).cast<float>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(axisName));
            return 0.0f;
        }

        /**
            @brief Returns true while the virtual button identified by buttonName is held down.
            @param buttonName Button name (e.g. "Fire1", "Jump").
            @return True if button held down.
        */
        static inline bool GetButton(const std::string_view &buttonName) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetButton"), 1).cast<bool>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(buttonName));
            return false;
        }

        /**
            @brief Returns true during the frame the user pressed the given virtual button.
            @param buttonName Button name.
            @return True on button down frame.
        */
        static inline bool GetButtonDown(const std::string_view &buttonName) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetButtonDown"), 1).cast<bool>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(buttonName));
            return false;
        }

        /**
            @brief Returns true the first frame the user releases the given virtual button.
            @param buttonName Button name.
            @return True on button up frame.
        */
        static inline bool GetButtonUp(const std::string_view &buttonName) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetButtonUp"), 1).cast<bool>();
            if (method.IsValid()) return method(Structures::Mono::String::Create(buttonName));
            return false;
        }

        /**
            @brief Number of active touches (mobile devices).
            @return Touch count integer.
        */
        static inline int GetTouchCount() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_touchCount"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Returns object representing status of a specific touch.
            @param index Touch index (0 to touchCount - 1).
            @return Touch structure.
        */
        static inline Touch GetTouch(int index) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("GetTouch"), 1).cast<Touch>();
            if (method.IsValid()) return method(index);
            return {};
        }

        /**
            @brief Returns list of objects representing status of all active touches during last frame.
            @return Mono Array of Touch structures.
        */
        static inline Structures::Mono::Array<Touch> *GetTouches() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_touches"), 0).cast<Structures::Mono::Array<Touch> *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief Last measured linear acceleration of a device in three-dimensional space.
            @return Acceleration as Vector3.
        */
        static inline Structures::Unity::Vector3 GetAcceleration() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_acceleration"), 0).cast<Structures::Unity::Vector3>();
            if (method.IsValid()) return method();
            return {};
        }

        /**
            @brief Returns whether the device on which the application is currently running supports touch input.
            @return True if touch is supported.
        */
        static inline bool GetTouchSupported() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_touchSupported"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return false;
        }

        /**
            @brief Property indicating whether the system handles multiple touches.
            @return True if multi-touch is enabled.
        */
        static inline bool GetMultiTouchEnabled() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_multiTouchEnabled"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return true;
        }

        /**
            @brief Sets whether the system handles multiple touches.
            @param value True to enable multi-touch.
        */
        static inline void SetMultiTouchEnabled(bool value) {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("set_multiTouchEnabled"), 1).cast<void>();
            if (method.IsValid()) method(value);
        }

        /**
            @brief Is any key or mouse button currently held down?
            @return True if any key is held.
        */
        static inline bool GetAnyKey() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_anyKey"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return false;
        }

        /**
            @brief Returns true the first frame the user hits any key or mouse button.
            @return True on first frame any key is hit.
        */
        static inline bool GetAnyKeyDown() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_anyKeyDown"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return false;
        }

        /**
            @brief Returns the keyboard input entered this frame as a Mono String.
            @return String of characters entered.
        */
        static inline Structures::Mono::String *GetInputString() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_inputString"), 0).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief Property for accessing device location (LocationService).
            @return LocationService pointer as Il2CppObject.
        */
        static inline IL2CPP::Il2CppObject *GetLocation() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_location"), 0).cast<IL2CPP::Il2CppObject *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief Returns default Gyroscope.
            @return Gyroscope pointer as Il2CppObject.
        */
        static inline IL2CPP::Il2CppObject *GetGyro() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_gyro"), 0).cast<IL2CPP::Il2CppObject *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief Property for accessing compass (Compass).
            @return Compass pointer as Il2CppObject.
        */
        static inline IL2CPP::Il2CppObject *GetCompass() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Input")).GetMethod(BNM_OBFUSCATE("get_compass"), 0).cast<IL2CPP::Il2CppObject *>();
            if (method.IsValid()) return method();
            return nullptr;
        }
    };
}
