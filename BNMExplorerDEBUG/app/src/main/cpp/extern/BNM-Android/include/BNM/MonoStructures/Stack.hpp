#pragma once

#include <vector>
#include <cstring>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../DebugMessages.hpp"
#include "../Class.hpp"
#include "Array.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.Collections.Generic.Stack template implementation in BNM.
        @tparam T Element data type.
    */
    template<typename T>
    struct Stack : BNM::IL2CPP::Il2CppObject {

        Array<T> *_array{};
        int _size{};
        int _version{};
        void *_syncRoot{};

        /**
            @brief Get element count in stack.
            @return Element count.
        */
        [[nodiscard]] inline int GetCount() const { return _size; }

        /**
            @brief Alias for GetCount().
            @return Element count.
        */
        [[nodiscard]] inline int GetSize() const { return _size; }

        /**
            @brief Check if stack is empty.
            @return True if empty.
        */
        [[nodiscard]] inline bool Empty() const { return _size <= 0; }

        /**
            @brief Get top element without removing it.
            @return Top element.
        */
        [[nodiscard]] inline T Peek() const {
            BNM_CHECK_SELF(T{});
            if (_size <= 0) return T{};
            return _array->m_Items[_size - 1];
        }

        /**
            @brief Convert stack to std::vector (top-to-bottom order).
            @return Vector containing elements.
        */
        [[nodiscard]] std::vector<T> ToVector() const {
            std::vector<T> ret{};
            BNM_CHECK_SELF(ret);
            if (!_array || _size <= 0) return ret;
            ret.reserve(_size);
            for (int i = _size - 1; i >= 0; i--) ret.push_back(_array->m_Items[i]);
            return ret;
        }

        /**
            @brief Convert stack to a new Mono Array (top-to-bottom order).
            @return New Mono Array.
        */
        [[nodiscard]] Array<T> *ToArray() const {
            if (!_array || _size <= 0) return Array<T>::Create(0);
            auto arr = Array<T>::Create(_size);
            for (int i = 0; i < _size; i++) arr->m_Items[i] = _array->m_Items[_size - 1 - i];
            return arr;
        }

        /**
            @brief Check if stack contains an element.
            @param item Target element.
            @return True if found.
        */
        [[nodiscard]] bool Contains(T item) const {
            BNM_CHECK_SELF(false);
            if (!_array) return false;
            for (int i = 0; i < _size; i++) if (_array->m_Items[i] == item) return true;
            return false;
        }

        /**
            @brief Push element onto the stack (grows array if needed).
            @param item Element to push.
        */
        void Push(T item) {
            BNM_CHECK_SELF();
            if (!_array || _size == (int)_array->GetCapacity()) {
                int newCap = _array ? (int)_array->GetCapacity() * 2 : 4;
                if (newCap < _size + 1) newCap = _size + 1;
                auto newArr = Array<T>::Create(newCap);
                if (_array) {
                    // Keep the array type metadata of the original buffer so the
                    // new array stays a valid C# T[] for the runtime/GC.
                    newArr->klass = _array->klass;
                    newArr->monitor = _array->monitor;
                    newArr->bounds = _array->bounds;
                    if (_size > 0) memcpy(&newArr->m_Items[0], &_array->m_Items[0], _size * sizeof(T));
                }
                _array = newArr;
            }
            _array->m_Items[_size++] = item;
            ++_version;
        }

        /**
            @brief Pop element from the stack.
            @return Popped element.
        */
        T Pop() {
            BNM_CHECK_SELF(T{});
            if (_size <= 0) return T{};
            T item = _array->m_Items[--_size];
            _array->m_Items[_size] = T{};
            ++_version;
            return item;
        }

        /**
            @brief Clear all elements.
        */
        void Clear() {
            BNM_CHECK_SELF();
            if (_array) {
                for (int i = 0; i < _size; i++) _array->m_Items[i] = T{};
            }
            _size = 0;
            ++_version;
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
