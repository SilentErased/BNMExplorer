#pragma once

#include <vector>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../DebugMessages.hpp"
#include "../Class.hpp"
#include "Array.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.Collections.Generic.Queue template implementation in BNM.
        @tparam T Element data type.
    */
    template<typename T>
    struct Queue : BNM::IL2CPP::Il2CppObject {

        Array<T> *_array{};
        int _head{};
        int _tail{};
        int _size{};
        int _version{};
        void *_syncRoot{};

        /**
            @brief Get element count in queue.
            @return Element count.
        */
        [[nodiscard]] inline int GetCount() const { return _size; }

        /**
            @brief Alias for GetCount().
            @return Element count.
        */
        [[nodiscard]] inline int GetSize() const { return _size; }

        /**
            @brief Check if queue is empty.
            @return True if empty.
        */
        [[nodiscard]] inline bool Empty() const { return _size <= 0; }

        /**
            @brief Get front element without removing it.
            @return Front element.
        */
        [[nodiscard]] inline T Peek() const {
            BNM_CHECK_SELF(T{});
            if (_size <= 0 || !_array) return T{};
            return _array->m_Items[_head];
        }

        /**
            @brief Convert queue to std::vector (front-to-back order).
            @return Vector containing elements.
        */
        [[nodiscard]] std::vector<T> ToVector() const {
            std::vector<T> ret{};
            BNM_CHECK_SELF(ret);
            if (!_array || _size <= 0) return ret;
            ret.reserve(_size);
            int cap = (int)_array->GetCapacity();
            for (int i = 0; i < _size; i++) {
                ret.push_back(_array->m_Items[(_head + i) % cap]);
            }
            return ret;
        }

        /**
            @brief Convert queue to a new Mono Array (front-to-back order).
            @return New Mono Array.
        */
        [[nodiscard]] Array<T> *ToArray() const {
            if (!_array || _size <= 0) return Array<T>::Create(0);
            auto arr = Array<T>::Create(_size);
            int cap = (int)_array->GetCapacity();
            for (int i = 0; i < _size; i++) arr->m_Items[i] = _array->m_Items[(_head + i) % cap];
            return arr;
        }

        /**
            @brief Check if queue contains an element.
            @param item Target element.
            @return True if found.
        */
        [[nodiscard]] bool Contains(T item) const {
            BNM_CHECK_SELF(false);
            if (!_array) return false;
            int cap = (int)_array->GetCapacity();
            for (int i = 0; i < _size; i++) {
                if (_array->m_Items[(_head + i) % cap] == item) return true;
            }
            return false;
        }

        /**
            @brief Enqueue element at the back (grows array if needed).
            @param item Element to enqueue.
        */
        void Enqueue(T item) {
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
                    int oldCap = (int)_array->GetCapacity();
                    for (int i = 0; i < _size; i++) {
                        newArr->m_Items[i] = _array->m_Items[(_head + i) % oldCap];
                    }
                }
                _array = newArr;
                _head = 0;
                _tail = _size;
            }
            _array->m_Items[_tail] = item;
            _tail = (_tail + 1) % (int)_array->GetCapacity();
            ++_size;
            ++_version;
        }

        /**
            @brief Dequeue element from the front.
            @return Dequeued element.
        */
        T Dequeue() {
            BNM_CHECK_SELF(T{});
            if (_size <= 0 || !_array) return T{};
            T item = _array->m_Items[_head];
            _array->m_Items[_head] = T{};
            _head = (_head + 1) % (int)_array->GetCapacity();
            --_size;
            ++_version;
            return item;
        }

        /**
            @brief Clear all elements.
        */
        void Clear() {
            BNM_CHECK_SELF();
            if (_array) {
                int cap = (int)_array->GetCapacity();
                for (int i = 0; i < _size; i++) {
                    _array->m_Items[(_head + i) % cap] = T{};
                }
            }
            _head = 0;
            _tail = 0;
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
