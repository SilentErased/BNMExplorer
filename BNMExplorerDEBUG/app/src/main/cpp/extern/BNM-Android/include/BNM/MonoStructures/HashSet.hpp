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
        @brief System.Collections.Generic.HashSet template implementation in BNM.
        @tparam T Element data type.
    */
    template<typename T>
    struct HashSet : BNM::IL2CPP::Il2CppObject {

        /**
            @brief Internal slot layout (matches IL2CPP HashSet.Slot<T>).
        */
        struct Slot {
            int hashCode{};
            int next{};
            T value{};
        };

        Array<int> *_buckets{};
        Array<Slot> *_slots{};
        int _count{};
        int _lastIndex{};
        int _freeList{};
        void *_comparer{};
        int _version{};
        void *_siInfo{};

        /**
            @brief Get element count in hash set.
            @return Element count.
        */
        [[nodiscard]] inline int GetCount() const { return _count; }

        /**
            @brief Alias for GetCount().
            @return Element count.
        */
        [[nodiscard]] inline int GetSize() const { return _count; }

        /**
            @brief Check if hash set is empty.
            @return True if empty.
        */
        [[nodiscard]] inline bool Empty() const { return _count <= 0; }

        /**
            @brief Check if hash set contains an element (via IL2CPP — element
            hashing/equality follows the C# comparer, not pointer identity).
            @param item Target element.
            @return True if found.
        */
        [[nodiscard]] bool Contains(T item) const {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("Contains"), 1).template cast<bool>()[(void *)this](item);
        }

        /**
            @brief Convert hash set to std::vector.
            @return Vector containing elements.
        */
        [[nodiscard]] std::vector<T> ToVector() const {
            std::vector<T> ret{};
            BNM_CHECK_SELF(ret);
            if (!_slots) return ret;
            ret.reserve(_count);
            for (int i = 0; i < _lastIndex; i++) {
                if (_slots->m_Items[i].hashCode >= 0) ret.push_back(_slots->m_Items[i].value);
            }
            return ret;
        }

        /**
            @brief Convert hash set to a new Mono Array.
            @return New Mono Array.
        */
        [[nodiscard]] Array<T> *ToArray() const {
            if (!_slots || _count <= 0) return Array<T>::Create(0);
            auto arr = Array<T>::Create(_count);
            int idx = 0;
            for (int i = 0; i < _lastIndex; i++) {
                if (_slots->m_Items[i].hashCode >= 0) arr->m_Items[idx++] = _slots->m_Items[i].value;
            }
            return arr;
        }

        /**
            @brief Add element to the set (via IL2CPP — requires resize + rehash).
            @param item Element to add.
            @return True if added (false if already present).
        */
        bool Add(T item) {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("Add"), 1).template cast<bool>()[(void *)this](item);
        }

        /**
            @brief Remove element from the set (via IL2CPP).
            @param item Element to remove.
            @return True if removed.
        */
        bool Remove(T item) {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("Remove"), 1).template cast<bool>()[(void *)this](item);
        }

        /**
            @brief Clear all elements (via IL2CPP).
        */
        void Clear() {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("Clear"), 0).template cast<void>()[(void *)this]();
        }

        /**
            @brief Union with another collection (via IL2CPP).
            @param other IEnumerable<T> to union with.
        */
        void UnionWith(BNM::IL2CPP::Il2CppObject *other) {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("UnionWith"), 1).template cast<void>()[(void *)this](other);
        }

        /**
            @brief Intersect with another collection (via IL2CPP).
            @param other IEnumerable<T> to intersect with.
        */
        void IntersectWith(BNM::IL2CPP::Il2CppObject *other) {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("IntersectWith"), 1).template cast<void>()[(void *)this](other);
        }

        /**
            @brief Except with another collection (via IL2CPP).
            @param other IEnumerable<T> to except with.
        */
        void ExceptWith(BNM::IL2CPP::Il2CppObject *other) {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("ExceptWith"), 1).template cast<void>()[(void *)this](other);
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

}
