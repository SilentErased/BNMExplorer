#pragma once

#include <cstdint>
#include <string>
#include <cstdio>
#include "../UserSettings/GlobalSettings.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.TimeSpan struct representation in BNM.
    */
    struct TimeSpan {
        int64_t _ticks{};

        static constexpr int64_t TicksPerMillisecond = 10000;
        static constexpr int64_t TicksPerSecond = TicksPerMillisecond * 1000;
        static constexpr int64_t TicksPerMinute = TicksPerSecond * 60;
        static constexpr int64_t TicksPerHour = TicksPerMinute * 60;
        static constexpr int64_t TicksPerDay = TicksPerHour * 24;

        constexpr TimeSpan() = default;
        constexpr explicit TimeSpan(int64_t ticks) : _ticks(ticks) {}
        constexpr TimeSpan(int hours, int minutes, int seconds)
            : _ticks((int64_t)hours * TicksPerHour + (int64_t)minutes * TicksPerMinute + (int64_t)seconds * TicksPerSecond) {}
        constexpr TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds = 0)
            : _ticks((int64_t)days * TicksPerDay + (int64_t)hours * TicksPerHour + (int64_t)minutes * TicksPerMinute + (int64_t)seconds * TicksPerSecond + (int64_t)milliseconds * TicksPerMillisecond) {}

        [[nodiscard]] constexpr inline int64_t Ticks() const { return _ticks; }

        [[nodiscard]] constexpr inline int Days() const { return (int)(_ticks / TicksPerDay); }
        [[nodiscard]] constexpr inline int Hours() const { return (int)((_ticks / TicksPerHour) % 24); }
        [[nodiscard]] constexpr inline int Minutes() const { return (int)((_ticks / TicksPerMinute) % 60); }
        [[nodiscard]] constexpr inline int Seconds() const { return (int)((_ticks / TicksPerSecond) % 60); }
        [[nodiscard]] constexpr inline int Milliseconds() const { return (int)((_ticks / TicksPerMillisecond) % 1000); }

        [[nodiscard]] constexpr inline double TotalDays() const { return (double)_ticks / (double)TicksPerDay; }
        [[nodiscard]] constexpr inline double TotalHours() const { return (double)_ticks / (double)TicksPerHour; }
        [[nodiscard]] constexpr inline double TotalMinutes() const { return (double)_ticks / (double)TicksPerMinute; }
        [[nodiscard]] constexpr inline double TotalSeconds() const { return (double)_ticks / (double)TicksPerSecond; }
        [[nodiscard]] constexpr inline double TotalMilliseconds() const { return (double)_ticks / (double)TicksPerMillisecond; }

        static constexpr inline TimeSpan Zero() { return TimeSpan(0); }
        static constexpr inline TimeSpan FromTicks(int64_t ticks) { return TimeSpan(ticks); }
        static constexpr inline TimeSpan FromMilliseconds(double ms) { return TimeSpan((int64_t)(ms * TicksPerMillisecond)); }
        static constexpr inline TimeSpan FromSeconds(double sec) { return TimeSpan((int64_t)(sec * TicksPerSecond)); }
        static constexpr inline TimeSpan FromMinutes(double min) { return TimeSpan((int64_t)(min * TicksPerMinute)); }
        static constexpr inline TimeSpan FromHours(double hrs) { return TimeSpan((int64_t)(hrs * TicksPerHour)); }
        static constexpr inline TimeSpan FromDays(double days) { return TimeSpan((int64_t)(days * TicksPerDay)); }

        constexpr inline TimeSpan operator+(const TimeSpan &other) const { return TimeSpan(_ticks + other._ticks); }
        constexpr inline TimeSpan operator-(const TimeSpan &other) const { return TimeSpan(_ticks - other._ticks); }
        constexpr inline bool operator==(const TimeSpan &other) const { return _ticks == other._ticks; }
        constexpr inline bool operator!=(const TimeSpan &other) const { return _ticks != other._ticks; }
        constexpr inline bool operator<(const TimeSpan &other) const { return _ticks < other._ticks; }
        constexpr inline bool operator>(const TimeSpan &other) const { return _ticks > other._ticks; }
        constexpr inline bool operator<=(const TimeSpan &other) const { return _ticks <= other._ticks; }
        constexpr inline bool operator>=(const TimeSpan &other) const { return _ticks >= other._ticks; }
    };

}
