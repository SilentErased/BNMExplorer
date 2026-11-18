#pragma once

#include "../UserSettings/GlobalSettings.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.Nullable<T> template implementation in BNM.
        @tparam T Value data type.
    */
    template<typename T>
    struct Nullable {
        bool hasValue{};
        T value{};

        constexpr Nullable() = default;
        constexpr Nullable(const T &val) : hasValue(true), value(val) {}

        /**
            @brief Check if nullable contains a value.
            @return True if hasValue is true.
        */
        [[nodiscard]] constexpr inline bool HasValue() const { return hasValue; }

        /**
            @brief Get underlying value.
            @return Stored value.
        */
        [[nodiscard]] constexpr inline const T &Value() const { return value; }
        [[nodiscard]] constexpr inline T &Value() { return value; }

        /**
            @brief Get value or default if null.
            @param defaultValue Fallback value if hasValue is false.
            @return Stored value or defaultValue.
        */
        [[nodiscard]] constexpr inline T GetValueOrDefault(const T &defaultValue = T{}) const {
            return hasValue ? value : defaultValue;
        }

        constexpr inline explicit operator bool() const { return hasValue; }
        constexpr inline const T& operator *() const { return value; }
        constexpr inline T& operator *() { return value; }
        constexpr inline const T* operator ->() const { return &value; }
        constexpr inline T* operator ->() { return &value; }

        inline Nullable &operator=(const T &val) {
            hasValue = true;
            value = val;
            return *this;
        }

        inline void Reset() {
            hasValue = false;
            value = T{};
        }

        inline bool operator==(const Nullable<T> &other) const {
            if (hasValue != other.hasValue) return false;
            return !hasValue || value == other.value;
        }

        inline bool operator!=(const Nullable<T> &other) const {
            return !(*this == other);
        }
    };

}
