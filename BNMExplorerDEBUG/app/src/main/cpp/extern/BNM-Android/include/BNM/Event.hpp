#pragma once

#include <type_traits>
#include <functional>
#include <utility>

#include "UserSettings/GlobalSettings.hpp"
#include "EventBase.hpp"
#include "Utils.hpp"
#include "Delegates.hpp"

// NOLINTBEGIN
namespace BNM {

#pragma pack(push, 1)

    /**
        @brief Typed class for working with il2cpp events.

        This class provides API for adding, removing and rising event.

        @tparam Ret Return type of event
        @tparam Parameters Parameters of event
    */
    template<typename Ret = void, typename ...Parameters>
    struct Event : EventBase {

        /**
            @brief Create empty event base.
        */
        inline constexpr Event() = default;

        /**
            @brief Copy event.
            @param other Other event
            @tparam OtherType Type of other event
        */
        template<typename OtherType>
        Event(const Event<OtherType> &other) : EventBase(other) {}

        /**
            @brief Create event from il2cpp event.
            @param info Il2cpp event
        */
        Event(const IL2CPP::EventInfo *info) : EventBase(info) {}

        /**
            @brief Convert base event to typed event.
            @param other Base event
        */
        Event(const EventBase &other) : EventBase(other) {}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Event
        */
        inline Event<Ret, Parameters...> &operator[](void *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Event
        */
        inline Event<Ret, Parameters...> &operator[](IL2CPP::Il2CppObject *instance) { SetInstance(instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current Event
        */
        inline Event<Ret, Parameters...> &operator[](UnityEngine::Object *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Add delegate to event.
            @param delegate Delegate to add
        */
        inline void Add(Delegate<Ret> *delegate) {
            if (_hasAdd) return _add.cast<void>()(delegate);
            BNM_LOG_ERR(DBG_BNM_MSG_Event_Add_Error, str().c_str());
        }

        /**
            @brief Add callable lambda/functor directly to event.
            @param callable Functor or lambda to invoke.
            @return Pointer to newly created Delegate<Ret>.
        */
        template<typename F, typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, Delegate<Ret> *>>>
        inline Delegate<Ret> *Add(F &&callable) {
            if constexpr (std::is_void_v<Ret>) {
                auto del = (Delegate<Ret> *) CreateAction<Parameters...>(std::forward<F>(callable));
                Add(del);
                return del;
            } else {
                auto del = CreateFunc<Ret, Parameters...>(std::forward<F>(callable));
                Add(del);
                return del;
            }
        }

        /**
            @brief Operator for adding delegate to event.
            @param delegate Delegate to add
        */
        inline Event<Ret, Parameters...> &operator+=(Delegate<Ret> *delegate) { Add(delegate); return *this; }

        /**
            @brief Operator for adding callable lambda/functor to event.
            @param callable Functor or lambda to invoke.
        */
        template<typename F, typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, Delegate<Ret> *>>>
        inline Event<Ret, Parameters...> &operator+=(F &&callable) { Add(std::forward<F>(callable)); return *this; }

        /**
            @brief Remove delegate from event.
            @param delegate Delegate to remove
        */
        inline void Remove(Delegate<Ret> *delegate) {
            if (_hasRemove) return _remove.cast<void>()(delegate);
            BNM_LOG_ERR(DBG_BNM_MSG_Event_Remove_Error, str().c_str());
        }

        /**
            @brief Operator for removing delegate from event.
            @param delegate Delegate to remove
        */
        inline Event<Ret, Parameters...> &operator-=(Delegate<Ret> *v) { Remove(v); return *this; }

        /**
            @brief Raise (call) event.
            @param parameters Parameters of event
            @return Value of Ret type
        */
        inline Ret Raise(Parameters ...parameters) const {
            if (_hasRaise) return _raise.cast<Ret>()(parameters...);
            BNM_LOG_ERR(DBG_BNM_MSG_Event_Raise_Error, str().c_str());
            return BNM::PRIVATE_INTERNAL::ReturnEmpty<Ret>();
        }

        /**
            @brief Raise (call) event.
            @param parameters Parameters of event
            @return Value of Ret type
        */
        inline Ret operator()(Parameters ...parameters) const { return Raise(parameters...); }

        /**
            @brief Convert base event to typed event.
            @param other Base event
        */
        Event<Ret> &operator =(const EventBase &other) {
            _data = other._data;
            _add = other._add;
            _remove = other._remove;
            _raise = other._raise;
            _hasAdd = other._hasAdd;
            _hasRemove = other._hasRemove;
            _hasRaise = other._hasRaise;
            return *this;
        }
    };

#pragma pack(pop)

    /**
        @brief RAII scoped event listener guard.
        Automatically unregisters / removes listener when the guard goes out of scope.
    */
    class ScopedEventListener {
    private:
        std::function<void()> _cleanup{};

    public:
        constexpr ScopedEventListener() = default;

        /**
            @brief Constructs a scoped listener from a custom cleanup callable.
            @param cleanup Functor to invoke upon destruction.
        */
        explicit ScopedEventListener(std::function<void()> cleanup) : _cleanup(std::move(cleanup)) {}

        /**
            @brief Constructs a scoped listener bound to an IL2CPP Event and Delegate pointer.
            @param event Reference to the BNM::Event.
            @param del Delegate pointer to register and automatically unregister on scope exit.
        */
        template<typename Ret, typename ...Parameters>
        ScopedEventListener(Event<Ret, Parameters...> &event, Delegate<Ret> *del) {
            if (del) {
                event += del;
                _cleanup = [&event, del]() {
                    event -= del;
                };
            }
        }

        ScopedEventListener(ScopedEventListener &&other) noexcept : _cleanup(std::move(other._cleanup)) {
            other._cleanup = nullptr;
        }

        ScopedEventListener &operator=(ScopedEventListener &&other) noexcept {
            if (this != &other) {
                Reset();
                _cleanup = std::move(other._cleanup);
                other._cleanup = nullptr;
            }
            return *this;
        }

        ScopedEventListener(const ScopedEventListener &) = delete;
        ScopedEventListener &operator=(const ScopedEventListener &) = delete;

        ~ScopedEventListener() {
            Reset();
        }

        /**
            @brief Manually unregisters the listener before the scope ends.
        */
        inline void Reset() {
            if (_cleanup) {
                auto cleanup = std::move(_cleanup);
                _cleanup = nullptr;
                cleanup();
            }
        }

        /**
            @brief Releases ownership without unregistering the listener.
        */
        inline void Release() {
            _cleanup = nullptr;
        }

        /**
            @brief Checks if the guard is actively tracking a listener.
            @return True if listener is active.
        */
        [[nodiscard]] inline bool IsActive() const noexcept {
            return _cleanup != nullptr;
        }
    };

    /**
        @brief Convenience function to attach a scoped listener to a BNM::Event using a callable lambda.
        @tparam Ret Return type.
        @tparam Parameters Event parameters.
        @tparam F Callable functor/lambda type.
        @param event Target BNM::Event.
        @param callable Callable to invoke.
        @return ScopedEventListener guard managing the subscription lifecycle.
    */
    template<typename Ret = void, typename ...Parameters, typename F>
    inline ScopedEventListener Listen(Event<Ret, Parameters...> &event, F &&callable) {
        auto del = event.Add(std::forward<F>(callable));
        return ScopedEventListener([&event, del]() {
            event -= del;
        });
    }

}
// NOLINTEND
