#pragma once

#include <cstdint>
#include <cmath>
#include <string>
#include "../UserSettings/GlobalSettings.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.Decimal binary layout implementation.
        @note Represents 128-bit decimal floating point numbers used by C# decimal.
    */
    struct decimal {
        int flags{};
        int hi{};
        int lo{};
        int mid{};

        constexpr decimal() = default;
        constexpr decimal(int flags, int hi, int lo, int mid) : flags(flags), hi(hi), lo(lo), mid(mid) {}

        /**
            @brief Convert decimal to double floating-point approximation.
            @return Converted double value.
        */
        [[nodiscard]] inline double ToDouble() const {
            uint64_t low64 = (static_cast<uint64_t>(static_cast<uint32_t>(mid)) << 32) | static_cast<uint32_t>(lo);
            double val = static_cast<double>(low64) + static_cast<double>(static_cast<uint32_t>(hi)) * 18446744073709551616.0;
            int scale = (flags >> 16) & 0xFF;
            if (scale > 0) val /= std::pow(10.0, scale);
            if (flags < 0) val = -val;
            return val;
        }

        /**
            @brief Convert decimal to float approximation.
            @return Converted float value.
        */
        [[nodiscard]] inline float ToFloat() const {
            return static_cast<float>(ToDouble());
        }

        /**
            @brief Convert decimal to 64-bit integer (truncated).
            @return Converted int64_t value.
        */
        [[nodiscard]] inline int64_t ToInt64() const {
            return static_cast<int64_t>(ToDouble());
        }

        /**
            @brief Create decimal from double value.
            @param val Source double value.
            @return Approximate decimal struct.
        */
        static inline decimal FromDouble(double val) {
            decimal d{};
            if (val < 0.0) { d.flags = static_cast<int>(0x80000000); val = -val; }
            int scale = 0;
            while (val > 0.0 && val != std::floor(val) && scale < 28) {
                val *= 10.0;
                scale++;
            }
            d.flags |= (scale << 16);
            auto intVal = static_cast<uint64_t>(val);
            d.lo = static_cast<int>(intVal & 0xFFFFFFFF);
            d.mid = static_cast<int>((intVal >> 32) & 0xFFFFFFFF);
            d.hi = 0;
            return d;
        }

        /**
            @brief Create decimal from 64-bit integer.
            @param val Source integer.
            @return Converted decimal struct.
        */
        static inline decimal FromInt64(int64_t val) {
            decimal d{};
            if (val < 0) { d.flags = static_cast<int>(0x80000000); val = -val; }
            d.lo = static_cast<int>(static_cast<uint64_t>(val) & 0xFFFFFFFF);
            d.mid = static_cast<int>((static_cast<uint64_t>(val) >> 32) & 0xFFFFFFFF);
            d.hi = 0;
            return d;
        }

        inline explicit operator double() const { return ToDouble(); }
        inline explicit operator float() const { return ToFloat(); }
        inline explicit operator int64_t() const { return ToInt64(); }

        inline bool operator==(const decimal &other) const {
            return flags == other.flags && hi == other.hi && lo == other.lo && mid == other.mid;
        }

        inline bool operator!=(const decimal &other) const {
            return !(*this == other);
        }
    };

}
