#pragma once

#include <vector>
#include <cstring>
#include <array>
#include <string_view>
#include <algorithm>
#include <functional>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../DebugMessages.hpp"
#include "../Utils.hpp"
#include "DataIterator.hpp"
#include "Array.hpp"

// NOLINTBEGIN
namespace BNM::Structures::Mono {

    template<typename T> struct List;

    /// @cond
    namespace PRIVATE_MonoListData {
        void *CompareExchange4List(void *syncRoot);
        template<typename T>
        void InitMonoListVTable(List<T> *list);
    }
    /// @endcond

    /**
        @brief System.Collections.Generic.List template implementation in BNM.
        @tparam T Element data type.
    */
    template<typename T>
    struct List : BNM::IL2CPP::Il2CppObject {

        /**
            @brief List enumerator struct for C# compatibility.
        */
        struct Enumerator {
            List<T> *list{};
            int index{};
            int version{};
            T current{};
            constexpr Enumerator() = default;
            explicit Enumerator(List<T> *list) : Enumerator() { this->list = list; }

            inline T* begin() { return &list->items->m_Items[0]; }
            inline T* end() { return &list->items->m_Items[list->size]; }
            [[nodiscard]] inline const T* begin() const { return &list->items->m_Items[0]; }
            [[nodiscard]] inline const T* end() const { return &list->items->m_Items[list->size]; }
        };

        Array<T> *items{};
        int size{};
        int version{};
        void *syncRoot{};

        /**
            @brief Get pointer to underlying elements buffer.
            @return Pointer to elements.
        */
        [[nodiscard]] inline T *GetData() const { return items ? items->GetData() : nullptr; }

        /**
            @brief Get current element count in list.
            @return Element count.
        */
        [[nodiscard]] inline int GetSize() const { return size; }

        /**
            @brief Alias for GetSize().
            @return Element count.
        */
        [[nodiscard]] inline int GetCount() const { return size; }

        /**
            @brief Get allocated internal array capacity.
            @return Capacity count.
        */
        [[nodiscard]] inline int GetCapacity() const { return items ? (int)items->GetCapacity() : 0; }

        /**
            @brief Get version number of list (increments on modification).
            @return Version integer.
        */
        [[nodiscard]] inline int GetVersion() const { return version; }

        /**
            @brief Check if list is empty.
            @return True if size is 0.
        */
        [[nodiscard]] inline bool Empty() const { return size <= 0; }

        /**
            @brief Convert list elements into a C++ std::vector.
            @return Vector containing elements.
        */
        [[nodiscard]] std::vector<T> ToVector() const {
            std::vector<T> ret{};
            BNM_CHECK_SELF(ret);
            if (!items) return ret;
            ret.reserve(size);
            auto data = GetData();
            for (int i = 0; i < size; i++) ret.push_back(data[i]);
            return ret;
        }

        /**
            @brief Convert list into a new Mono Array instance.
            @return New Mono Array.
        */
        [[nodiscard]] Array<T> *ToArray() const {
            if (!items || size <= 0) return Array<T>::Create(0);
            auto arr = Array<T>::Create(size);
            arr->CopyFrom(GetData(), size);
            return arr;
        }

        //! =========================================================================================
        //! Modern C++ Range-Based Iterators
        //! =========================================================================================

        inline T* begin() { return items ? &items->m_Items[0] : nullptr; }
        inline T* end() { return items ? &items->m_Items[size] : nullptr; }
        [[nodiscard]] inline const T* begin() const { return items ? &items->m_Items[0] : nullptr; }
        [[nodiscard]] inline const T* end() const { return items ? &items->m_Items[size] : nullptr; }

        //! =========================================================================================
        //! Element Manipulation Methods
        //! =========================================================================================

        /**
            @brief Append an element to the end of the list.
            @param val Element value.
        */
        void Add(T val) {
            GrowIfNeeded(1);
            items->m_Items[size] = val;
            size++;
            version++;
        }

        /**
            @brief Append multiple elements from a std::vector.
            @param range Vector of elements.
        */
        void AddRange(const std::vector<T> &range) {
            if (range.empty()) return;
            GrowIfNeeded((int)range.size());
            for (const auto &elem : range) {
                items->m_Items[size++] = elem;
            }
            version++;
        }

        /**
            @brief Append elements from a Mono Array.
            @param arr Source Mono Array.
        */
        void AddRange(const Array<T> *arr) {
            if (!arr || arr->GetCapacity() == 0) return;
            auto count = (int)arr->GetCapacity();
            GrowIfNeeded(count);
            for (int i = 0; i < count; ++i) {
                items->m_Items[size++] = arr->m_Items[i];
            }
            version++;
        }

        /**
            @brief Append elements from another List.
            @param otherList Source list.
        */
        void AddRange(const List<T> *otherList) {
            if (!otherList || otherList->size == 0) return;
            GrowIfNeeded(otherList->size);
            for (int i = 0; i < otherList->size; ++i) {
                items->m_Items[size++] = otherList->items->m_Items[i];
            }
            version++;
        }

        /**
            @brief Find first index of matching element.
            @param val Target value.
            @return 0-based index or -1 if not found.
        */
        [[nodiscard]] int IndexOf(T val) const {
            if (!items) return -1;
            for (int i = 0; i < size; i++) if (items->m_Items[i] == val) return i;
            return -1;
        }

        /**
            @brief Find last index of matching element.
            @param val Target value.
            @return 0-based index or -1 if not found.
        */
        [[nodiscard]] int LastIndexOf(T val) const {
            if (!items) return -1;
            for (int i = size - 1; i >= 0; --i) if (items->m_Items[i] == val) return i;
            return -1;
        }

        /**
            @brief Check if list contains matching element.
            @param item Element to check.
            @return True if found.
        */
        [[nodiscard]] bool Contains(T item) const {
            return IndexOf(item) != -1;
        }

        /**
            @brief Remove element at index.
            @param index 0-based index.
        */
        void RemoveAt(int index) {
            if (index >= 0 && index < size) {
                Shift(index, -1);
                version++;
            }
        }

        /**
            @brief Remove first occurrence of element.
            @param val Target value.
            @return True if removed.
        */
        bool Remove(T val) {
            int i = IndexOf(val);
            if (i == -1) return false;
            RemoveAt(i);
            return true;
        }

        /**
            @brief Remove all elements matching predicate.
            @tparam Predicate Callable bool(const T&).
            @param pred Matching filter.
            @return Number of elements removed.
        */
        template<typename Predicate>
        int RemoveAll(Predicate &&pred) {
            int removed = 0;
            for (int i = size - 1; i >= 0; --i) {
                if (pred(items->m_Items[i])) {
                    RemoveAt(i);
                    removed++;
                }
            }
            return removed;
        }

        /**
            @brief Remove a range of elements.
            @param index Starting index.
            @param count Number of elements to remove.
        */
        void RemoveRange(int index, int count) {
            if (index < 0 || count < 0 || index + count > size) return;
            if (count > 0) {
                Shift(index, -count);
                version++;
            }
        }

        /**
            @brief Resize internal array capacity.
            @param newCapacity Target capacity.
            @return True if resized.
        */
        bool Resize(int newCapacity) {
            BNM_CHECK_SELF(false);
            if (!items) {
                items = Array<T>::Create(newCapacity);
                return true;
            }
            if (newCapacity <= (int)items->capacity) return false;
            auto nItems = Array<T>::Create(newCapacity);
            nItems->klass = items->klass;
            nItems->monitor = items->monitor;
            nItems->bounds = items->bounds;
            nItems->capacity = newCapacity;
            if (items->capacity > 0 && size > 0)
                memcpy(&nItems->m_Items[0], &items->m_Items[0], size * sizeof(T));
            items = nItems;
            return true;
        }

        /**
            @brief Get iterator wrapper for element at index.
            @param index 0-based index.
            @return DataIterator pointing to element.
        */
        [[nodiscard]] Utils::DataIterator<T> At(int index) const {
            if (!items || index < 0 || index >= size) return {};
            return &items->m_Items[index];
        }

        /**
            @brief Subscript operator to access element at index.
        */
        Utils::DataIterator<T> operator[](int index) const { return At(index); }

        /**
            @brief Copy elements from std::vector into this list.
            @param vec Source vector.
            @return True if successful.
        */
        inline bool CopyFrom(const std::vector<T> &vec) { return CopyFrom((T *)vec.data(), (int)vec.size()); }

        /**
            @brief Copy elements from raw pointer buffer into this list.
            @param arr Source pointer.
            @param arrSize Element count.
            @return True if copied.
        */
        bool CopyFrom(const T *arr, int arrSize) {
            BNM_CHECK_SELF(false);
            if (!arr || arrSize < 0) return false;
            Resize(arrSize);
            memcpy(items->m_Items, arr, arrSize * sizeof(T));
            size = arrSize;
            version++;
            return true;
        }

        /**
            @brief Clear all elements from the list.
        */
        void Clear() {
            if (size > 0 && items) memset(items->m_Items, 0, size * sizeof(T));
            ++version;
            size = 0;
        }

        Enumerator GetEnumerator() { return this; }

        [[nodiscard]] T get_Item(int index) const {
            if (!items || index < 0 || index >= size) return {};
            return items->m_Items[index];
        }

        void set_Item(int index, T item) {
            if (!items || index < 0 || index >= size) return;
            items->m_Items[index] = item;
            ++version;
        }

        /**
            @brief Insert element at specified index.
            @param index Insertion index.
            @param item Element to insert.
        */
        void Insert(int index, T item) {
            if (index < 0 || index > size) return;
            if (!items || size == (int)items->capacity) GrowIfNeeded(1);
            if (index < size) memmove(items->m_Items + index + 1, items->m_Items + index, (size - index) * sizeof(T));
            items->m_Items[index] = item;
            ++size;
            ++version;
        }

        /**
            @brief Insert range of elements at specified index.
            @param index Insertion index.
            @param range Vector of elements to insert.
        */
        void InsertRange(int index, const std::vector<T> &range) {
            if (index < 0 || index > size || range.empty()) return;
            int count = (int)range.size();
            GrowIfNeeded(count);
            if (index < size) memmove(items->m_Items + index + count, items->m_Items + index, (size - index) * sizeof(T));
            for (int i = 0; i < count; ++i) {
                items->m_Items[index + i] = range[i];
            }
            size += count;
            version++;
        }

        /// @cond
        void *get_SyncRoot() {
            if (!syncRoot) syncRoot = PRIVATE_MonoListData::CompareExchange4List(syncRoot);
            return syncRoot;
        }
        bool get_false() const { return false; }
        /// @endcond

        void CopyTo(Array<T>* arr, int arrIndex) const {
            if (!arr || !items || arrIndex < 0) return;
            memcpy(arr->m_Items + arrIndex, items->m_Items, size * sizeof(T));
        }

        void GrowIfNeeded(int n) {
            if (!items) {
                Resize(std::max(4, n));
                return;
            }
            if (size + n > (int)items->capacity) {
                int newCap = std::max((int)items->capacity * 2, size + n);
                Resize(newCap);
            }
        }

        void Shift(int start, int delta) {
            if (!items) return;
            if (delta < 0) start -= delta;
            if (start < size) memmove(items->m_Items + start + delta, items->m_Items + start, (size - start) * sizeof(T));
            size += delta;
            if (delta < 0) memset(items->m_Items + size + delta, 0, -delta * sizeof(T));
        }

        //! =========================================================================================
        //! LINQ & Functional Helper Methods
        //! =========================================================================================

        /**
            @brief Check if any element matches predicate.
            @tparam Predicate Callable bool(const T&).
            @param pred Filter predicate.
            @return True if match exists.
        */
        template<typename Predicate>
        [[nodiscard]] bool Exists(Predicate &&pred) const {
            if (!items) return false;
            for (int i = 0; i < size; ++i) {
                if (pred(items->m_Items[i])) return true;
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
            if (!items) return nullptr;
            for (int i = 0; i < size; ++i) {
                if (pred(items->m_Items[i])) return &items->m_Items[i];
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
            if (!items) return -1;
            for (int i = 0; i < size; ++i) {
                if (pred(items->m_Items[i])) return i;
            }
            return -1;
        }

        /**
            @brief Check if all elements satisfy predicate.
            @tparam Predicate Callable bool(const T&).
            @param pred Filter predicate.
            @return True if all match.
        */
        template<typename Predicate>
        [[nodiscard]] bool TrueForAll(Predicate &&pred) const {
            if (!items || size == 0) return true;
            for (int i = 0; i < size; ++i) {
                if (!pred(items->m_Items[i])) return false;
            }
            return true;
        }

        /**
            @brief Execute action for each element in list.
            @tparam Action Callable void(T&).
            @param action Function to run.
        */
        template<typename Action>
        void ForEach(Action &&action) {
            if (!items) return;
            for (int i = 0; i < size; ++i) {
                action(items->m_Items[i]);
            }
        }

        /**
            @brief Reverse list elements in-place.
        */
        void Reverse() {
            if (!items || size <= 1) return;
            std::reverse(begin(), end());
            version++;
        }

        /**
            @brief Sort list elements ascending in-place.
        */
        void Sort() {
            if (!items || size <= 1) return;
            std::sort(begin(), end());
            version++;
        }

        /**
            @brief Sort list elements using custom comparator.
            @tparam Compare Callable bool(const T&, const T&).
            @param comp Comparator function.
        */
        template<typename Compare>
        void Sort(Compare &&comp) {
            if (!items || size <= 1) return;
            std::sort(begin(), end(), std::forward<Compare>(comp));
            version++;
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

        inline constexpr List() : BNM::IL2CPP::Il2CppObject() {
            klass = nullptr;
            monitor = nullptr;
        }
    };

    // Based on https://github.com/royvandam/rtti/tree/cf0dee6fb3999573f45b0726a8d5739022e3dacf
    /// @cond
    namespace PRIVATE_MonoListData {
        template <typename T> constexpr std::string_view WrappedTypeName() { return __PRETTY_FUNCTION__; }
        constexpr std::size_t WrappedTypeNamePrefixLength() { return WrappedTypeName<void>().find("void"); }
        constexpr std::size_t WrappedTypeNameSuffixLength() { return WrappedTypeName<void>().length() - WrappedTypeNamePrefixLength() - 4; }
        constexpr uint32_t FNV1a(const char* str, size_t n, uint32_t hash = 2166136261U) {
            return n == 0 ? hash : FNV1a(str + 1, n - 1, (hash ^ str[0]) * 19777619U);
        }
        constexpr uint32_t FNV1a(const std::string_view &str) { return FNV1a(str.data(), str.size()); }
        template <typename T>
        constexpr uint32_t HashedTypeName() {
            constexpr auto wrappedTypeName = WrappedTypeName<T>();
            constexpr auto prefixLength = WrappedTypeNamePrefixLength();
            constexpr auto suffixLength = WrappedTypeNameSuffixLength();
            constexpr auto typeNameLength = wrappedTypeName.length() - prefixLength - suffixLength;
            constexpr auto typeName = wrappedTypeName.substr(prefixLength, typeNameLength);
            return FNV1a(typeName.data(), typeName.size());
        }
        struct MethodData { std::string_view methodName{}; void *ptr{}; };
        IL2CPP::Il2CppClass *TryGetMonoListClass(uint32_t typeHash, std::array<MethodData, 16> &data);

        template<typename T>
        void InitMonoListVTable(List<T> *list) {
            using namespace PRIVATE_MonoListData;
            using Type = std::conditional_t<std::is_pointer_v<T>, void*, T>;
            constexpr auto RemoveAt = &List<Type>::RemoveAt; constexpr auto GetSize = &List<Type>::GetSize; constexpr auto Clear = &List<Type>::Clear;
            constexpr auto get_Item = &List<Type>::get_Item; constexpr auto set_Item = &List<Type>::set_Item; constexpr auto IndexOf = &List<Type>::IndexOf;
            constexpr auto Insert = &List<Type>::Insert; constexpr auto get_false = &List<Type>::get_false; constexpr auto Add = &List<Type>::Add;
            constexpr auto Contains = &List<Type>::Contains; constexpr auto CopyTo = &List<Type>::CopyTo; constexpr auto Remove = &List<Type>::Remove;
            constexpr auto GetEnumerator = &List<Type>::GetEnumerator; constexpr auto get_SyncRoot = &List<Type>::get_SyncRoot;
            static std::array<MethodData, 16> namesMap = {
                    MethodData{BNM_OBFUSCATE("RemoveAt"), *(void **)&RemoveAt}, MethodData{BNM_OBFUSCATE("get_Count"), *(void **)&GetSize},
                    MethodData{BNM_OBFUSCATE("Clear"), *(void **)&Clear}, MethodData{BNM_OBFUSCATE("get_Item"), *(void **)&get_Item},
                    MethodData{BNM_OBFUSCATE("set_Item"), *(void **)&set_Item}, MethodData{BNM_OBFUSCATE("IndexOf"), *(void **)&IndexOf},
                    MethodData{BNM_OBFUSCATE("Insert"), *(void **)&Insert}, MethodData{BNM_OBFUSCATE("get_IsReadOnly"), *(void **)&get_false},
                    MethodData{BNM_OBFUSCATE("get_IsFixedSize"), *(void **)&get_false}, MethodData{BNM_OBFUSCATE("get_IsSynchronized"), *(void **)&get_false},
                    MethodData{BNM_OBFUSCATE("Add"), *(void **)&Add}, MethodData{BNM_OBFUSCATE("Contains"), *(void **)&Contains},
                    MethodData{BNM_OBFUSCATE("CopyTo"), *(void **)&CopyTo}, MethodData{BNM_OBFUSCATE("Remove"), *(void **)&Remove},
                    MethodData{BNM_OBFUSCATE("GetEnumerator"), *(void **)&GetEnumerator}, MethodData{BNM_OBFUSCATE("get_SyncRoot"), *(void **)&get_SyncRoot}
            };
            list->klass = TryGetMonoListClass(HashedTypeName<Type>(), namesMap);
        }
    }
    /// @endcond

}
// NOLINTEND
