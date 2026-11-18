#pragma once

#include <vector>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../DebugMessages.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Array.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.Collections.Generic.IReadOnlyList view implementation in BNM.
        This is a read-only view over an IL2CPP IReadOnlyList<T> — no native layout,
        all access goes through reflection (get_Count / get_Item).
        @tparam T Element data type.
    */
    template<typename T>
    struct IReadOnlyList : BNM::IL2CPP::Il2CppObject {

        /**
            @brief Get element count.
            @return Element count.
        */
        [[nodiscard]] int GetCount() const {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("get_Count"), 0).template cast<int>()[(void *)this]();
        }

        /**
            @brief Alias for GetCount().
            @return Element count.
        */
        [[nodiscard]] inline int GetSize() const { return GetCount(); }

        /**
            @brief Check if list is empty.
            @return True if empty.
        */
        [[nodiscard]] inline bool Empty() const { return GetCount() <= 0; }

        /**
            @brief Get element at index.
            @param index Element index.
            @return Element at index.
        */
        [[nodiscard]] T GetItem(int index) const {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("get_Item"), 1).template cast<T>()[(void *)this](index);
        }

        /**
            @brief Subscript operator to retrieve element at index.
        */
        [[nodiscard]] inline T operator[](int index) const { return GetItem(index); }

        /**
            @brief Convert to std::vector.
            @return Vector containing all elements.
        */
        [[nodiscard]] std::vector<T> ToVector() const {
            std::vector<T> ret{};
            BNM_CHECK_SELF(ret);
            int count = GetCount();
            ret.reserve(count);
            for (int i = 0; i < count; i++) ret.push_back(GetItem(i));
            return ret;
        }

        /**
            @brief Convert to a new Mono Array.
            @return New Mono Array.
        */
        [[nodiscard]] Array<T> *ToArray() const {
            int count = GetCount();
            if (count <= 0) return Array<T>::Create(0);
            auto arr = Array<T>::Create(count);
            for (int i = 0; i < count; i++) arr->m_Items[i] = GetItem(i);
            return arr;
        }

#ifdef BNM_ALLOW_SELF_CHECKS
        /**
            @brief Self-check for null pointer in debug builds.
            @return True if valid.
        */
        [[nodiscard]] bool SelfCheck() const {
            if (CheckForNull(this)) return true;
            BNM_LOG_ERR(DBG_BNM_MSG_List_SelfCheck_Error);
            return false;
        }
#endif
    };

    /**
        @brief System.Collections.Generic.IReadOnlyCollection view — alias for IReadOnlyList.
        IReadOnlyCollection<T> is the base interface of IReadOnlyList<T>; both expose get_Count.
        @tparam T Element data type.
    */
    template<typename T>
    struct IReadOnlyCollection : BNM::IL2CPP::Il2CppObject {

        /**
            @brief Get element count.
            @return Element count.
        */
        [[nodiscard]] int GetCount() const {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("get_Count"), 0).template cast<int>()[(void *)this]();
        }

        /**
            @brief Alias for GetCount().
            @return Element count.
        */
        [[nodiscard]] inline int GetSize() const { return GetCount(); }

        /**
            @brief Check if collection is empty.
            @return True if empty.
        */
        [[nodiscard]] inline bool Empty() const { return GetCount() <= 0; }

#ifdef BNM_ALLOW_SELF_CHECKS
        /**
            @brief Self-check for null pointer in debug builds.
            @return True if valid.
        */
        [[nodiscard]] bool SelfCheck() const {
            if (CheckForNull(this)) return true;
            BNM_LOG_ERR(DBG_BNM_MSG_List_SelfCheck_Error);
            return false;
        }
#endif
    };

}
