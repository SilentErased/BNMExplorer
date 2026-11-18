#pragma once

#include <vector>
#include <mutex>
#include <functional>
#include <atomic>
#include <algorithm>
#include <utility>

#include "UserSettings/GlobalSettings.hpp"
#include "Delegates.hpp"

namespace BNM {

    template<typename Signature>
    class CustomEvent;

    /**
        @brief Thread-safe and re-entrant C++ event dispatcher.
        Can be listened to by C++ lambdas, std::function, or IL2CPP delegates.
        @tparam Ret Return type of the event invocation.
        @tparam Args Argument types passed to listeners.
    */
    template<typename Ret, typename ...Args>
    class CustomEvent<Ret(Args...)> {
    public:
        using Listener = std::function<Ret(Args...)>;
        using ListenerId = uint64_t;

    private:
        struct Entry {
            ListenerId id{};
            Listener callback{};
            void *rawTarget{nullptr}; // To support removing by raw delegate pointer
        };

        mutable std::mutex _mutex{};
        std::vector<Entry> _entries{};
        std::atomic<ListenerId> _nextId{1};

    public:
        constexpr CustomEvent() = default;

        ~CustomEvent() {
            Clear();
        }

        CustomEvent(const CustomEvent &) = delete;
        CustomEvent &operator=(const CustomEvent &) = delete;

        CustomEvent(CustomEvent &&other) noexcept {
            std::lock_guard<std::mutex> lock(other._mutex);
            _entries = std::move(other._entries);
            _nextId.store(other._nextId.load());
        }

        CustomEvent &operator=(CustomEvent &&other) noexcept {
            if (this != &other) {
                std::scoped_lock lock(_mutex, other._mutex);
                _entries = std::move(other._entries);
                _nextId.store(other._nextId.load());
            }
            return *this;
        }

        /**
            @brief Registers a callable listener to the event.
            @param callback Functor or lambda to invoke.
            @return Unique ListenerId used for unregistering.
        */
        ListenerId Add(Listener callback) {
            if (!callback) return 0;
            ListenerId id = _nextId.fetch_add(1, std::memory_order_relaxed);
            std::lock_guard<std::mutex> lock(_mutex);
            _entries.push_back({id, std::move(callback), nullptr});
            return id;
        }

        /**
            @brief Registers an IL2CPP Delegate as a listener.
            @param del Delegate pointer.
            @return Unique ListenerId used for unregistering.
        */
        ListenerId Add(Delegate<Ret> *del) {
            if (!del || !del->IsValid()) return 0;
            ListenerId id = _nextId.fetch_add(1, std::memory_order_relaxed);
            std::lock_guard<std::mutex> lock(_mutex);
            _entries.push_back({id, [del](Args ...args) -> Ret {
                return del->Invoke(args...);
            }, (void *)del});
            return id;
        }

        /**
            @brief Unregisters a listener by its unique ListenerId.
            @param id ListenerId returned when adding.
            @return True if listener was found and removed.
        */
        bool Remove(ListenerId id) {
            if (id == 0) return false;
            std::lock_guard<std::mutex> lock(_mutex);
            auto it = std::find_if(_entries.begin(), _entries.end(), [id](const Entry &e) {
                return e.id == id;
            });
            if (it != _entries.end()) {
                _entries.erase(it);
                return true;
            }
            return false;
        }

        /**
            @brief Unregisters a listener by raw delegate pointer.
            @param del Delegate pointer.
            @return True if removed.
        */
        bool Remove(Delegate<Ret> *del) {
            if (!del) return false;
            std::lock_guard<std::mutex> lock(_mutex);
            auto it = std::find_if(_entries.begin(), _entries.end(), [del](const Entry &e) {
                return e.rawTarget == (void *)del;
            });
            if (it != _entries.end()) {
                _entries.erase(it);
                return true;
            }
            return false;
        }

        /**
            @brief Operator overload for registering a callable listener.
            @param callback Functor or lambda to invoke.
            @return Unique ListenerId.
        */
        inline ListenerId operator+=(Listener callback) { return Add(std::move(callback)); }

        /**
            @brief Operator overload for registering an IL2CPP Delegate listener.
            @param del Delegate pointer.
            @return Unique ListenerId.
        */
        inline ListenerId operator+=(Delegate<Ret> *del) { return Add(del); }

        /**
            @brief Operator overload for unregistering by ListenerId.
            @param id ListenerId to unregister.
            @return True if successfully removed.
        */
        inline bool operator-=(ListenerId id) { return Remove(id); }

        /**
            @brief Operator overload for unregistering by raw Delegate pointer.
            @param del Delegate pointer to unregister.
            @return True if successfully removed.
        */
        inline bool operator-=(Delegate<Ret> *del) { return Remove(del); }

        /**
            @brief Invokes all registered listeners with thread safety and re-entrancy protection.
            @param args Arguments to pass to listeners.
        */
        void Invoke(Args ...args) const {
            std::vector<Entry> snapshot;
            {
                std::lock_guard<std::mutex> lock(_mutex);
                snapshot = _entries; // Take a local snapshot copy to prevent deadlock and iterator invalidation
            }
            for (const auto &entry : snapshot) {
                if (entry.callback) {
                    entry.callback(args...);
                }
            }
        }

        /**
            @brief Invokes all registered listeners (functor syntax).
            @param args Arguments to pass to listeners.
        */
        inline void operator()(Args ...args) const { Invoke(args...); }

        /**
            @brief Clears all registered listeners.
        */
        void Clear() {
            std::lock_guard<std::mutex> lock(_mutex);
            _entries.clear();
        }

        /**
            @brief Gets the number of registered listeners.
            @return Count of active listeners.
        */
        [[nodiscard]] size_t Size() const {
            std::lock_guard<std::mutex> lock(_mutex);
            return _entries.size();
        }

        /**
            @brief Checks if no listeners are registered.
            @return True if empty.
        */
        [[nodiscard]] bool IsEmpty() const {
            std::lock_guard<std::mutex> lock(_mutex);
            return _entries.empty();
        }
    };

}
