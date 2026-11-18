#pragma once

#include <cstdint>
#include <chrono>
#include <string>
#include "../UserSettings/GlobalSettings.hpp"
#include "TimeSpan.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.DateTime struct representation in BNM.
    */
    struct DateTime {
        uint64_t dateData{};

        static constexpr uint64_t TicksMask = 0x3FFFFFFFFFFFFFFFULL;
        static constexpr int64_t FileTimeOffset = 504911232000000000LL;

        constexpr DateTime() = default;
        constexpr explicit DateTime(int64_t ticks) : dateData((uint64_t)ticks & TicksMask) {}

        /**
            @brief Create DateTime from raw internal 64-bit data.
            @param rawData Raw data containing ticks and kind.
            @return DateTime instance.
        */
        static constexpr inline DateTime FromRaw(uint64_t rawData) {
            DateTime dt{};
            dt.dateData = rawData;
            return dt;
        }

        /**
            @brief Get raw tick count (1 tick = 100ns).
            @return Number of ticks.
        */
        [[nodiscard]] constexpr inline int64_t Ticks() const {
            return (int64_t)(dateData & TicksMask);
        }

        /**
            @brief Get DateTimeKind (0 = Unspecified, 1 = Utc, 2 = Local).
            @return Kind enum value.
        */
        [[nodiscard]] constexpr inline int Kind() const {
            return (int)(dateData >> 62);
        }

        /**
            @brief Get second component (0-59).
        */
        [[nodiscard]] constexpr inline int Second() const {
            return (int)((Ticks() / TimeSpan::TicksPerSecond) % 60);
        }

        /**
            @brief Get minute component (0-59).
        */
        [[nodiscard]] constexpr inline int Minute() const {
            return (int)((Ticks() / TimeSpan::TicksPerMinute) % 60);
        }

        /**
            @brief Get hour component (0-23).
        */
        [[nodiscard]] constexpr inline int Hour() const {
            return (int)((Ticks() / TimeSpan::TicksPerHour) % 24);
        }

        /**
            @brief Get current system UTC time.
            @return DateTime in UTC.
        */
        static inline DateTime UtcNow() {
            auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();
            auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
            int64_t ticks = 621355968000000000LL + (nanos / 100);
            return FromRaw(((uint64_t)ticks & TicksMask) | (1ULL << 62));
        }

        /**
            @brief Get current local system time.
            @return DateTime in local time.
        */
        static inline DateTime Now() {
            auto utc = UtcNow();
            return FromRaw((utc.dateData & TicksMask) | (2ULL << 62));
        }

        constexpr inline DateTime operator+(const TimeSpan &ts) const {
            return FromRaw((uint64_t)(Ticks() + ts.Ticks()) | (dateData & ~TicksMask));
        }

        constexpr inline DateTime operator-(const TimeSpan &ts) const {
            return FromRaw((uint64_t)(Ticks() - ts.Ticks()) | (dateData & ~TicksMask));
        }

        constexpr inline TimeSpan operator-(const DateTime &other) const {
            return TimeSpan(Ticks() - other.Ticks());
        }

        constexpr inline bool operator==(const DateTime &other) const { return Ticks() == other.Ticks(); }
        constexpr inline bool operator!=(const DateTime &other) const { return Ticks() != other.Ticks(); }
        constexpr inline bool operator<(const DateTime &other) const { return Ticks() < other.Ticks(); }
        constexpr inline bool operator>(const DateTime &other) const { return Ticks() > other.Ticks(); }
        constexpr inline bool operator<=(const DateTime &other) const { return Ticks() <= other.Ticks(); }
        constexpr inline bool operator>=(const DateTime &other) const { return Ticks() >= other.Ticks(); }
    };

}
