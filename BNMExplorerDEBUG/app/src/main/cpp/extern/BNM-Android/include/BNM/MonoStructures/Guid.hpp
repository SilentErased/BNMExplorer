#pragma once

#include <cstdint>
#include <string>
#include <cstdio>
#include <cstring>
#include "../UserSettings/GlobalSettings.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.Guid struct implementation in BNM (128-bit UUID layout).
    */
    struct Guid {
        int32_t _a{};
        int16_t _b{};
        int16_t _c{};
        uint8_t _d{};
        uint8_t _e{};
        uint8_t _f{};
        uint8_t _g{};
        uint8_t _h{};
        uint8_t _i{};
        uint8_t _j{};
        uint8_t _k{};

        constexpr Guid() = default;

        constexpr Guid(int32_t a, int16_t b, int16_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h, uint8_t i, uint8_t j, uint8_t k)
            : _a(a), _b(b), _c(c), _d(d), _e(e), _f(f), _g(g), _h(h), _i(i), _j(j), _k(k) {}

        explicit Guid(const uint8_t bytes[16]) {
            if (!bytes) return;
            memcpy(this, bytes, 16);
        }

        /**
            @brief Check if Guid is empty (all zeroes).
            @return True if empty.
        */
        [[nodiscard]] inline bool IsEmpty() const {
            return _a == 0 && _b == 0 && _c == 0 &&
                   _d == 0 && _e == 0 && _f == 0 && _g == 0 &&
                   _h == 0 && _i == 0 && _j == 0 && _k == 0;
        }

        /**
            @brief Static empty Guid.
            @return All-zero Guid.
        */
        static constexpr Guid Empty() { return {}; }

        /**
            @brief Convert Guid to standard 8-4-4-4-12 string representation.
            @return Standard GUID string.
        */
        [[nodiscard]] std::string ToString() const {
            char buf[37];
            snprintf(buf, sizeof(buf), "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                     _a, (uint16_t)_b, (uint16_t)_c,
                     _d, _e, _f, _g, _h, _i, _j, _k);
            return std::string(buf);
        }

        inline bool operator==(const Guid &other) const {
            return memcmp(this, &other, sizeof(Guid)) == 0;
        }

        inline bool operator!=(const Guid &other) const {
            return !(*this == other);
        }
    };

}
