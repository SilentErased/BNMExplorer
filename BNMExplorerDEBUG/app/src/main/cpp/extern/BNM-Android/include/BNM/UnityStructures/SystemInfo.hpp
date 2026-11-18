#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Access system and hardware device information.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct SystemInfo {
        SystemInfo() = delete;

        /**
            @brief A unique device identifier. Guaranteed to be unique for every device (e.g. Android ID / iOS IDFV).
            @return Mono String containing unique device ID.
        */
        static inline Structures::Mono::String *GetDeviceUniqueIdentifier() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_deviceUniqueIdentifier"), 0).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief The user-defined name of the device.
            @return Mono String with device name.
        */
        static inline Structures::Mono::String *GetDeviceName() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_deviceName"), 0).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief The model of the device.
            @return Mono String with device model.
        */
        static inline Structures::Mono::String *GetDeviceModel() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_deviceModel"), 0).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief The type of the device (0: Unknown, 1: Handheld, 2: Console, 3: Desktop).
            @return DeviceType integer enum.
        */
        static inline int GetDeviceType() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_deviceType"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Operating system name and version.
            @return Mono String with OS info.
        */
        static inline Structures::Mono::String *GetOperatingSystem() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_operatingSystem"), 0).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief Operating system family (0: Other, 1: MacOSX, 2: Windows, 3: Linux, 4: iOS, 5: Android).
            @return OperatingSystemFamily integer enum.
        */
        static inline int GetOperatingSystemFamily() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_operatingSystemFamily"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Processor name.
            @return Mono String with processor type.
        */
        static inline Structures::Mono::String *GetProcessorType() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_processorType"), 0).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief Number of processors / cores present.
            @return Processor count integer.
        */
        static inline int GetProcessorCount() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_processorCount"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 1;
        }

        /**
            @brief Processor frequency in MHz.
            @return Frequency in MHz integer.
        */
        static inline int GetProcessorFrequency() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_processorFrequency"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Alias for GetProcessorFrequency().
            @return Frequency in MHz integer.
        */
        static inline int GetProcessorFrequencyMHz() { return GetProcessorFrequency(); }

        /**
            @brief Amount of system memory (RAM) in megabytes.
            @return System memory in MB.
        */
        static inline int GetSystemMemorySize() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_systemMemorySize"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Alias for GetSystemMemorySize().
            @return System memory in MB.
        */
        static inline int GetSystemMemorySizeMB() { return GetSystemMemorySize(); }

        /**
            @brief The name of the graphics device (GPU).
            @return Mono String with GPU name.
        */
        static inline Structures::Mono::String *GetGraphicsDeviceName() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_graphicsDeviceName"), 0).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief The vendor of the graphics device.
            @return Mono String with GPU vendor.
        */
        static inline Structures::Mono::String *GetGraphicsDeviceVendor() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_graphicsDeviceVendor"), 0).cast<Structures::Mono::String *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief The graphics API type being used (OpenGL, Vulkan, Metal, Direct3D).
            @return GraphicsDeviceType integer enum.
        */
        static inline int GetGraphicsDeviceType() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_graphicsDeviceType"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Amount of dedicated video memory in megabytes.
            @return VRAM in MB.
        */
        static inline int GetGraphicsMemorySize() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_graphicsMemorySize"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Alias for GetGraphicsMemorySize().
            @return VRAM in MB.
        */
        static inline int GetGraphicsMemorySizeMB() { return GetGraphicsMemorySize(); }

        /**
            @brief Are built-in shadows supported?
            @return True if shadows are supported.
        */
        static inline bool GetSupportsShadows() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_supportsShadows"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return true;
        }

        /**
            @brief Graphics shader capability level.
            @return Shader level integer.
        */
        static inline int GetGraphicsShaderLevel() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_graphicsShaderLevel"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }

        /**
            @brief Is GPU draw call instancing supported?
            @return True if supported.
        */
        static inline bool GetSupportsInstancing() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_supportsInstancing"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return false;
        }

        /**
            @brief Are compute shaders supported?
            @return True if supported.
        */
        static inline bool GetSupportsComputeShaders() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_supportsComputeShaders"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return false;
        }

        /**
            @brief Are 3D (volume) textures supported?
            @return True if supported.
        */
        static inline bool GetSupports3DTextures() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_supports3DTextures"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return false;
        }

        /**
            @brief Are 2D array textures supported?
            @return True if supported.
        */
        static inline bool GetSupports2DArrayTextures() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_supports2DArrayTextures"), 0).cast<bool>();
            if (method.IsValid()) return method();
            return false;
        }

        /**
            @brief Maximum texture size in pixels (width or height).
            @return Maximum texture size integer.
        */
        static inline int GetMaxTextureSize() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_maxTextureSize"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 2048;
        }

        /**
            @brief The current battery level (0.0 to 1.0, -1.0 if unsupported).
            @return Battery level float.
        */
        static inline float GetBatteryLevel() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_batteryLevel"), 0).cast<float>();
            if (method.IsValid()) return method();
            return -1.0f;
        }

        /**
            @brief The current battery status (0: Unknown, 1: Charging, 2: Discharging, 3: NotCharging, 4: Full).
            @return BatteryStatus integer enum.
        */
        static inline int GetBatteryStatus() {
            static auto method = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("SystemInfo")).GetMethod(BNM_OBFUSCATE("get_batteryStatus"), 0).cast<int>();
            if (method.IsValid()) return method();
            return 0;
        }
    };
}
