#pragma once

#include "../UserSettings/GlobalSettings.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.Collections.Generic.KeyValuePair<TKey, TValue> template implementation in BNM.
        @tparam TKey Key data type.
        @tparam TValue Value data type.
    */
    template<typename TKey, typename TValue>
    struct KeyValuePair {
        TKey key{};
        TValue value{};

        constexpr KeyValuePair() = default;
        constexpr KeyValuePair(const TKey &k, const TValue &v) : key(k), value(v) {}

        /**
            @brief Get entry key.
            @return Key reference.
        */
        [[nodiscard]] constexpr inline const TKey &Key() const { return key; }
        [[nodiscard]] constexpr inline TKey &Key() { return key; }

        /**
            @brief Get entry value.
            @return Value reference.
        */
        [[nodiscard]] constexpr inline const TValue &Value() const { return value; }
        [[nodiscard]] constexpr inline TValue &Value() { return value; }

        /**
            @brief C# property alias for key.
        */
        [[nodiscard]] constexpr inline const TKey &get_Key() const { return key; }

        /**
            @brief C# property alias for value.
        */
        [[nodiscard]] constexpr inline const TValue &get_Value() const { return value; }

        inline bool operator==(const KeyValuePair<TKey, TValue> &other) const {
            return key == other.key && value == other.value;
        }

        inline bool operator!=(const KeyValuePair<TKey, TValue> &other) const {
            return !(*this == other);
        }
    };

}
