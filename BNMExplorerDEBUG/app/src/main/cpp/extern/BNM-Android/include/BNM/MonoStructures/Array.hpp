#pragma once

#include <vector>
#include <cstring>
#include <algorithm>
#include <functional>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../DebugMessages.hpp"
#include "../Utils.hpp"
#include "../Defaults.hpp"
#include "DataIterator.hpp"

namespace BNM::Structures::Mono {

    /// @cond
    namespace __Internal_Array {
        void *ArrayFromClass(IL2CPP::Il2CppClass *, IL2CPP::il2cpp_array_size_t);
    }
    /// @endcond

    /**
        @brief C# System.Array template implementation in BNM.
        @tparam T Element data type.
    */
    template<typename T>
    struct Array : BNM::IL2CPP::Il2CppObject {
        IL2CPP::Il2CppArrayBounds *bounds{};
        IL2CPP::il2cpp_array_size_t capacity{};
        T m_Items[0];

        /**
            @brief Get total capacity (length) of array.
            @return Element count.
        */
        [[nodiscard]] inline IL2CPP::il2cpp_array_size_t GetCapacity() const {
            BNM_CHECK_SELF(0);
            return capacity;
        }

        /**
            @brief Alias for GetCapacity().
            @return Element count.
        */
        [[nodiscard]] inline IL2CPP::il2cpp_array_size_t GetSize() const { return GetCapacity(); }

        /**
            @brief Alias for GetCapacity().
            @return Element count.
        */
        [[nodiscard]] inline IL2CPP::il2cpp_array_size_t GetLength() const { return GetCapacity(); }

        /**
            @brief Get pointer to internal elements buffer.
            @return Pointer to elements.
        */
        [[nodiscard]] inline T *GetData() const {
            BNM_CHECK_SELF(nullptr);
            return (T *const) &m_Items[0];
        }

        /**
            @brief Convert Mono Array to C++ std::vector.
            @return Vector containing elements.
        */
        [[nodiscard]] std::vector<T> ToVector() const {
            std::vector<T> ret;
            BNM_CHECK_SELF(ret);
            ret.reserve(capacity);
            for (IL2CPP::il2cpp_array_size_t i = 0; i < capacity; i++) ret.push_back(m_Items[i]);
            return ret;
        }

        /**
            @brief Copy elements from std::vector into this array.
            @param vec Source vector.
            @return True if copied successfully.
        */
        inline bool CopyFrom(const std::vector<T> &vec) {
            BNM_CHECK_SELF(false);
            if (vec.empty()) return false;
            return CopyFrom((T *)vec.data(), (IL2CPP::il2cpp_array_size_t)vec.size());
        }

        /**
            @brief Copy elements from raw pointer into this array.
            @param arr Source pointer.
            @param size Number of elements to copy.
            @return True if size is within capacity.
        */
        bool CopyFrom(const T *arr, IL2CPP::il2cpp_array_size_t size) {
            BNM_CHECK_SELF(false);
            if (!arr || size > capacity) return false;
            memcpy(&m_Items[0], arr, size * sizeof(T));
            return true;
        }

        /**
            @brief Copy array elements into destination buffer.
            @param arr Destination pointer.
        */
        inline void CopyTo(T *arr) const {
            BNM_CHECK_SELF();
            if (!arr || !IsAllocated(m_Items)) return;
            memcpy(arr, m_Items, sizeof(T) * capacity);
        }

        /**
            @brief Get iterator wrapper for element at index.
            @param index 0-based element index.
            @return DataIterator pointing to element.
        */
        inline Utils::DataIterator<T> At(IL2CPP::il2cpp_array_size_t index) const {
            BNM_CHECK_SELF({});
            if (index >= capacity) return {};
            return &m_Items[index];
        }

        /**
            @brief Subscript operator to access element at index.
        */
        inline Utils::DataIterator<T> operator[](IL2CPP::il2cpp_array_size_t index) const {
            return At(index);
        }

        /**
            @brief Check if array is empty.
            @return True if capacity is 0.
        */
        [[nodiscard]] inline bool Empty() const {
            BNM_CHECK_SELF(false);
            return capacity <= 0;
        }

        //! =========================================================================================
        //! Modern C++ Range-Based Iterators
        //! =========================================================================================

        inline T* begin() { return &m_Items[0]; }
        inline T* end() { return &m_Items[capacity]; }
        [[nodiscard]] inline const T* begin() const { return &m_Items[0]; }
        [[nodiscard]] inline const T* end() const { return &m_Items[capacity]; }

        //! =========================================================================================
        //! LINQ & Search Helper Methods
        //! =========================================================================================

        /**
            @brief Find first index of matching value.
            @param val Value to find.
            @return 0-based index or -1 if not found.
        */
        [[nodiscard]] int IndexOf(const T &val) const {
            for (IL2CPP::il2cpp_array_size_t i = 0; i < capacity; ++i) {
                if (m_Items[i] == val) return (int)i;
            }
            return -1;
        }

        /**
            @brief Find last index of matching value.
            @param val Value to find.
            @return 0-based index or -1 if not found.
        */
        [[nodiscard]] int LastIndexOf(const T &val) const {
            for (int i = (int)capacity - 1; i >= 0; --i) {
                if (m_Items[i] == val) return i;
            }
            return -1;
        }

        /**
            @brief Check if array contains value.
            @param val Value to check.
            @return True if found.
        */
        [[nodiscard]] bool Contains(const T &val) const {
            return IndexOf(val) != -1;
        }

        /**
            @brief Check if any element satisfies a predicate.
            @tparam Predicate Callable bool(const T&).
            @param pred Filter predicate.
            @return True if match exists.
        */
        template<typename Predicate>
        [[nodiscard]] bool Exists(Predicate &&pred) const {
            for (IL2CPP::il2cpp_array_size_t i = 0; i < capacity; ++i) {
                if (pred(m_Items[i])) return true;
            }
            return false;
        }

        /**
            @brief Find first element matching predicate.
            @tparam Predicate Callable bool(const T&).
            @param pred Filter predicate.
            @return Pointer to element or nullptr if not found.
        */
        template<typename Predicate>
        [[nodiscard]] T *Find(Predicate &&pred) {
            for (IL2CPP::il2cpp_array_size_t i = 0; i < capacity; ++i) {
                if (pred(m_Items[i])) return &m_Items[i];
            }
            return nullptr;
        }

        /**
            @brief Find first index matching predicate.
            @tparam Predicate Callable bool(const T&).
            @param pred Filter predicate.
            @return 0-based index or -1 if not found.
        */
        template<typename Predicate>
        [[nodiscard]] int FindIndex(Predicate &&pred) const {
            for (IL2CPP::il2cpp_array_size_t i = 0; i < capacity; ++i) {
                if (pred(m_Items[i])) return (int)i;
            }
            return -1;
        }

        /**
            @brief Reverse the order of elements in array in-place.
        */
        inline void Reverse() {
            std::reverse(begin(), end());
        }

        /**
            @brief Sort elements in array ascending in-place.
        */
        inline void Sort() {
            std::sort(begin(), end());
        }

        /**
            @brief Sort elements in array using custom comparator.
            @tparam Compare Callable bool(const T&, const T&).
            @param comp Comparator function.
        */
        template<typename Compare>
        inline void Sort(Compare &&comp) {
            std::sort(begin(), end(), std::forward<Compare>(comp));
        }

        /**
            @brief Fill all elements in array with a given value.
            @param val Value to fill.
        */
        inline void Fill(const T &val) {
            std::fill(begin(), end(), val);
        }

        /**
            @brief Clear array buffer to zeroes.
        */
        inline void Clear() {
            if (capacity > 0) memset(&m_Items[0], 0, sizeof(T) * capacity);
        }

        /**
            @brief Create a shallow clone of this array.
            @return New Array pointer.
        */
        [[nodiscard]] Array<T> *Clone() const {
            auto newArr = Create(capacity);
            newArr->CopyFrom(&m_Items[0], capacity);
            return newArr;
        }

        //! =========================================================================================
        //! Creation & Lifetime Management
        //! =========================================================================================

        /**
            @brief Create new empty Mono Array with specified capacity.
            @param capacity Array capacity.
            @param _forceUseAlloc Force use internal allocator (advanced).
            @return New Array pointer.
        */
        static Array<T> *Create(size_t capacity, bool _forceUseAlloc = false) {
            auto cls = _forceUseAlloc ? BNM::Defaults::DefaultTypeRef{} : BNM::Defaults::Get<T>();
#ifndef BNM_USE_IL2CPP_ALLOCATOR
            auto monoArr = (Array<T> *) (cls.IsValid() ? __Internal_Array::ArrayFromClass(cls, capacity) : BNM_malloc(sizeof(Array) + sizeof(T) * capacity));
#else
            auto monoArr = (Array<T> *) (cls.IsValid() ? __Internal_Array::ArrayFromClass(cls, capacity) : BNM::Allocate(sizeof(Array) + sizeof(T) * capacity));
#endif
            memset(monoArr, 0, sizeof(Array) + sizeof(T) * capacity);
            if (!cls.IsValid()) monoArr->klass = nullptr;
            monoArr->capacity = (IL2CPP::il2cpp_array_size_t)capacity;
            return monoArr;
        }

        /**
            @brief Create new Mono Array from std::vector.
            @param vec Source vector.
            @param _forceUseAlloc Force use internal allocator.
            @return New Array pointer.
        */
        static Array<T> *Create(const std::vector<T> &vec, bool _forceUseAlloc = false) {
            return Create((T *)vec.data(), vec.size(), _forceUseAlloc);
        }

        /**
            @brief Create new Mono Array from raw pointer.
            @param arr Source pointer.
            @param size Number of elements.
            @param _forceUseAlloc Force use internal allocator.
            @return New Array pointer.
        */
        static Array<T> *Create(const T *arr, size_t size, bool _forceUseAlloc = false) {
            Array<T> *monoArr = Create(size, _forceUseAlloc);
            if (arr && size > 0) monoArr->CopyFrom(arr, (IL2CPP::il2cpp_array_size_t)size);
            return monoArr;
        }

        /**
            @brief Free allocated memory for this array (only for arrays created via BNM).
        */
        inline void Destroy() {
#ifndef BNM_USE_IL2CPP_ALLOCATOR
            if (!klass) BNM_free(this);
#else
            if (!klass) BNM::Free(this);
#endif
        }

#ifdef BNM_ALLOW_SELF_CHECKS
        /**
            @brief Self-check for null pointer in debug builds.
            @return True if valid.
        */
        [[nodiscard]] bool SelfCheck() const {
            if (CheckForNull(this)) return true;
            BNM_LOG_ERR(DBG_BNM_MSG_Array_SelfCheck_Error);
            return false;
        }
#endif

        inline constexpr Array() : BNM::IL2CPP::Il2CppObject() {
            klass = nullptr;
            monitor = nullptr;
        }
    };

}
