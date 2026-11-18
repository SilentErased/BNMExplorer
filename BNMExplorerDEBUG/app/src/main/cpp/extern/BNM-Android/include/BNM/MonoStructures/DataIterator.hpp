#pragma once

#include "../UserSettings/GlobalSettings.hpp"
#include "../DebugMessages.hpp"
#include "../Utils.hpp"

namespace BNM::Utils {

    /**
        @brief Helper struct for checking and referencing values from Mono::Array and Mono::List.
        @tparam T Data type.
    */
    template<typename T>
    struct DataIterator {
        T *value{};

        constexpr DataIterator() = default;
        constexpr DataIterator(const T *value) : value((T *)value) {}

        inline T& operator *() {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }

        inline T& operator *() const {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }

        inline operator T&() {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }

        inline operator T&() const {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }

        inline T& operator ->() {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }

        inline T& operator ->() const {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }

        inline DataIterator &operator=(T t) {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            if (value) *this->value = *(T*)&t;
            return *this;
        }

        inline DataIterator &operator=(T t) const {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            if (value) *this->value = *(T*)&t;
            return *this;
        }

        [[nodiscard]] inline bool IsValid() const { return value != nullptr; }
    };

}
