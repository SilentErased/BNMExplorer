#pragma once

#include <map>
#include <vector>
#include <utility>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "../Delegates.hpp"
#include "Array.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.Collections.Generic.Dictionary template implementation in BNM.
        @tparam TKey Key data type.
        @tparam TValue Value data type.
    */
    template<typename TKey, typename TValue>
    struct Dictionary : BNM::IL2CPP::Il2CppObject {
#ifdef BNM_DOTNET35
        /**
            @brief System.Collections.Generic.Dictionary.Link type implementation for legacy .NET 3.5.
        */
        struct Link {
            int HashCode{};
            int Next{};
        };
        Array<int> *table{};
        Array<Link> *linkSlots{};
        Array<TKey> *keySlots{};
        Array<TValue> *valueSlots{};
        void *touchedSlots{};
        void *emptySlot{};
        int count{};
        int threshold{};
        void *hcp{};
        void *serializationInfo{};
        int generation{};
#else
        /**
            @brief System.Collections.Generic.Dictionary.Entry type implementation for modern .NET.
        */
        struct Entry {
            int hashCode{};
            int next{};
            TKey key{};
            TValue value{};
        };
        Array<int> *buckets{};
        Array<Entry> *entries{};
        int count{};
        int version{};
        int freeList{};
        int freeCount{};
        void *comparer{};
        Array<TKey> *keys{};
        Array<TValue> *values{};
        void *syncRoot{};
#endif

        /**
            @brief Get element count in dictionary.
            @return Element count.
        */
#ifdef BNM_DOTNET35
        [[nodiscard]] inline int GetSize() const { return count; }
        [[nodiscard]] inline int GetCount() const { return count; }
        [[nodiscard]] inline bool Empty() const { return count <= 0; }
#else
        [[nodiscard]] inline int GetSize() const { return count - freeCount >= 0 ? count - freeCount : 0; }
        [[nodiscard]] inline int GetCount() const { return GetSize(); }
        [[nodiscard]] inline bool Empty() const { return GetSize() <= 0; }
#endif

        /**
            @brief Convert dictionary to std::map.
            @return std::map containing all key-value pairs.
        */
        [[nodiscard]] std::map<TKey, TValue> ToMap() const {
            std::map<TKey, TValue> ret{};
#ifdef BNM_DOTNET35
            if (!keySlots || !valueSlots) return ret;
            for (int i = 0; i < count; i++) ret[keySlots->m_Items[i]] = valueSlots->m_Items[i];
#else
            if (!entries) return ret;
            for (int i = 0; i < count; i++) {
                if (entries->m_Items[i].hashCode >= 0) {
                    ret[entries->m_Items[i].key] = entries->m_Items[i].value;
                }
            }
#endif
            return ret;
        }

        /**
            @brief Convert dictionary to std::vector of std::pair.
            @return Vector of pairs.
        */
        [[nodiscard]] std::vector<std::pair<TKey, TValue>> ToVector() const {
            std::vector<std::pair<TKey, TValue>> ret{};
#ifdef BNM_DOTNET35
            if (!keySlots || !valueSlots) return ret;
            ret.reserve(count);
            for (int i = 0; i < count; i++) ret.push_back({keySlots->m_Items[i], valueSlots->m_Items[i]});
#else
            if (!entries) return ret;
            ret.reserve(count - freeCount >= 0 ? count - freeCount : 0);
            for (int i = 0; i < count; i++) {
                if (entries->m_Items[i].hashCode >= 0) {
                    ret.push_back({entries->m_Items[i].key, entries->m_Items[i].value});
                }
            }
#endif
            return ret;
        }

        /**
            @brief Get all keys as a std::vector.
            @return Vector containing all keys.
        */
        [[nodiscard]] std::vector<TKey> GetKeys() const {
            std::vector<TKey> ret{};
#ifdef BNM_DOTNET35
            if (!keySlots) return ret;
            ret.reserve(count);
            for (int i = 0; i < count; i++) ret.push_back(keySlots->m_Items[i]);
#else
            if (!entries) return ret;
            ret.reserve(count - freeCount >= 0 ? count - freeCount : 0);
            for (int i = 0; i < count; i++) {
                if (entries->m_Items[i].hashCode >= 0) {
                    ret.push_back(entries->m_Items[i].key);
                }
            }
#endif
            return ret;
        }

        /**
            @brief Get all values as a std::vector.
            @return Vector containing all values.
        */
        [[nodiscard]] std::vector<TValue> GetValues() const {
            std::vector<TValue> ret{};
#ifdef BNM_DOTNET35
            if (!valueSlots) return ret;
            ret.reserve(count);
            for (int i = 0; i < count; i++) ret.push_back(valueSlots->m_Items[i]);
#else
            if (!entries) return ret;
            ret.reserve(count - freeCount >= 0 ? count - freeCount : 0);
            for (int i = 0; i < count; i++) {
                if (entries->m_Items[i].hashCode >= 0) {
                    ret.push_back(entries->m_Items[i].value);
                }
            }
#endif
            return ret;
        }

        /**
            @brief Try getting value by key.
            @param key Target key.
            @param value Output pointer to store value.
            @return True if key was found.
        */
        bool TryGet(TKey key, TValue *value) const {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("TryGetValue"), 2).template cast<bool>()[(void *)this](key, value);
        }

        /**
            @brief Alias for TryGet().
            @param key Target key.
            @param value Output pointer.
            @return True if found.
        */
        inline bool TryGetValue(TKey key, TValue *value) const {
            return TryGet(key, value);
        }

        /**
            @brief Check if dictionary contains key.
            @param key Target key.
            @return True if key exists.
        */
        [[nodiscard]] bool ContainsKey(TKey key) const {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("ContainsKey"), 1).template cast<bool>()[(void *)this](key);
        }

        /**
            @brief Alias for ContainsKey().
        */
        [[nodiscard]] inline bool Contains(TKey key) const {
            return ContainsKey(key);
        }

        /**
            @brief Check if dictionary contains value.
            @param value Target value.
            @return True if value exists.
        */
        [[nodiscard]] bool ContainsValue(TValue value) const {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("ContainsValue"), 1).template cast<bool>()[(void *)this](value);
        }

        /**
            @brief Add key-value pair to dictionary.
            @param key Key.
            @param value Value.
        */
        void Add(TKey key, TValue value) {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("Add"), 2).template cast<void>()[(void *)this](key, value);
        }

        /**
            @brief Set (or replace) key-value pair in dictionary.
            @param key Key.
            @param value Value.
        */
        void Set(TKey key, TValue value) {
            auto method = Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("set_Item"), 2).template cast<void>();
            if (method.IsValid()) method[(void *)this](key, value);
            else Add(key, value);
        }

        /**
            @brief Remove entry by key.
            @param key Target key.
            @return True if entry was removed.
        */
        bool Remove(TKey key) {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("Remove"), 1).template cast<bool>()[(void *)this](key);
        }

        /**
            @brief Clear all entries in dictionary.
        */
        void Clear() {
            return Class((IL2CPP::Il2CppObject *)this).GetMethod(BNM_OBFUSCATE("Clear"), 0).template cast<void>()[(void *)this]();
        }

        /**
            @brief Get value by key.
            @param key Target key.
            @return Value if found, otherwise default value.
        */
        [[nodiscard]] TValue Get(TKey key) const {
            TValue ret{};
            if (TryGet(key, &ret)) return ret;
            return {};
        }

        /**
            @brief Subscript operator to retrieve value by key.
        */
        [[nodiscard]] TValue operator[](TKey key) const { return Get(key); }
    };

}
