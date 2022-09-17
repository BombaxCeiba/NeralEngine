/**
 * @author: Dusk
 * @date: 2022-03-04 19:19:37
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:19:37
 * @copyright Copyright (c) 2022 Dusk.
*/
#ifndef NERAL_ENGINE_COMMON_EVENT_HPP
#define NERAL_ENGINE_COMMON_EVENT_HPP

#include <stdint.h>
#include <type_traits>
#include <functional>
#include <cstdint>
#include <vector>
#include <mutex>
#include <memory>
#include <Windows.h>
#include "Common/include/DuskTMP.hpp"

namespace Ceiba
{
    enum class EventState
    {
        Continue,
        End
    };
    struct Point2i32
    {
        std::int32_t x_;
        std::int32_t y_;
    };

    class EventArgsBase
    {
    public:
        EventArgsBase() = default;
        virtual ~EventArgsBase() = default;
    };

    class SizeChangedEventArgs final : public EventArgsBase
    {
    public:
        SizeChangedEventArgs(uint32_t previous_width, uint32_t previous_height, uint32_t new_width, uint32_t new_height)
            : previous_width_{previous_width}, previous_height_{previous_height},
              new_width_{new_width}, new_height_{new_height} {}
        ~SizeChangedEventArgs() = default;
        uint32_t previous_width_;
        uint32_t previous_height_;
        uint32_t new_width_;
        uint32_t new_height_;
    };

    class RenderEventArgs final : public EventArgsBase
    {
    public:
        RenderEventArgs(const PAINTSTRUCT& paint_struct) : paint_struct_{paint_struct} {}
        ~RenderEventArgs() = default;
        const PAINTSTRUCT& paint_struct_;
    };

    class KeyEventArgs final : public EventArgsBase
    {
    public:
        KeyEventArgs(WPARAM wParam, LPARAM lParam)
            : virtual_key_code_{LOWORD(wParam)}, repat_count_{LOWORD(lParam)},
              is_alt_down_{(HIWORD(lParam) & KF_ALTDOWN) == KF_ALTDOWN} {}
        WORD virtual_key_code_;
        WORD repat_count_;
        bool is_alt_down_;
    };

    enum class MouseButton
    {
        LeftButton = 1,
        RightButton = 2,
        MiddleButton = 4
    };
    class MouseEventArgs : public EventArgsBase
    {
    public:
        MouseButton pressed_button_;
        Point2i32 Location;
    };

    template <typename T>
    class ReversionWrapper
    {
    public:
        T& raw_data;
    };

    template <typename T>
    auto begin(ReversionWrapper<T> rw) noexcept(noexcept(rw.raw_data.rbegin()))
        -> decltype(rw.raw_data.rbegin())
    {
        return rw.raw_data.rbegin();
    }

    template <typename T>
    auto end(ReversionWrapper<T> rw) noexcept(noexcept(rw.raw_data.rend()))
        -> decltype(rw.raw_data.rend())
    {
        return rw.raw_data.rend();
    }

#if (__cplusplus >= 201402L)
    template <typename T>
    auto cbegin(ReversionWrapper<T> rw) noexcept(noexcept(rw.raw_data.crbegin()))
        -> decltype(rw.raw_data.crbegin())
    {
        return rw.raw_data.crbegin();
    }

    template <typename T>
    auto cend(ReversionWrapper<T> rw) noexcept(noexcept(rw.raw_data.crend()))
        -> decltype(rw.raw_data.cend())
    {
        return rw.raw_data.crend();
    }
#endif

    template <typename T>
    ReversionWrapper<T> reverse(T&& raw_data)
    {
        return {raw_data};
    }

    template <class T, typename = void>
    struct check_if_allocator_type_exist
        : public std::false_type
    {
    };
    template <class T>
    struct check_if_allocator_type_exist<T, Neral::void_t<typename T::value_type>>
        : public std::true_type
    {
    };

    template <class Function, class TokenType>
    using EventFunctionStorageType = std::pair<Function, TokenType>;

    template <class FunctionType, class FunctionContainer, class TokenType, class... Args>
    class EventImplProxy
    {
    public:
        static_assert(check_if_allocator_type_exist<FunctionContainer>{},
                      "FunctionContainer does not have type(or alias) allocator_type!");
        using Allocator = typename FunctionContainer::allocator_type;
        EventImplProxy(const Allocator& allocator = {}) : function_container_{allocator}, current_token_{0}
        {
        }
        EventImplProxy(const FunctionType default_function, const Allocator& allocator = {})
            : function_container_{allocator}, current_token_{0}
        {
            function_container_.push_back(std::make_pair(default_function, current_token_));
        }

        ~EventImplProxy() = default;
        inline void TriggerEvent(Args... args)
        {
            for (auto&& func : function_container_)
            {
                if (func.first)
                {
                    if (func.first(std::forward<Args>(args)...) == EventState::End)
                        [[unlikely]]
                    {
                        break;
                    }
                }
            }
        }
        inline void TriggerEventReversely(Args... args)
        {
            for (auto&& func : reverse(function_container_))
            {
                if (func.first)
                {
                    if (func.first(std::forward<Args>(args)...) == EventState::End)
                        [[unlikely]]
                    {
                        break;
                    }
                }
            }
        }
        inline const TokenType AddFunction(const FunctionType& function)
        {
            ++current_token_;
            function_container_.emplace_back(std::make_pair(function, current_token_));
            return current_token_;
        }
        inline void DeleteFunction(const TokenType index)
        {
            for (auto it = function_container_.begin(); it != function_container_.end(); ++it)
            {
                if (it->second == index)
                {
                    function_container_.erase(it);
                    break;
                }
            }
        }

    private:
        FunctionContainer function_container_;
        TokenType current_token_;
    };
    template <class FunctionTy, class Mutex, class TokenTy, class StorageType, class Container, class... Args>
    class EventImpl
    {
    public:
        using FunctionType = FunctionTy;
        static_assert(std::is_same<StorageType, EventFunctionStorageType<FunctionType, TokenTy>>::value,
                      "User provieded first template argument of FunctionContainer in class Event<... ,FunctionContainer ,... > does not match expected type!");
        using TokenType = TokenTy;
        using EventProxy = EventImplProxy<FunctionType, Container, TokenType, Args...>;
        template <typename... Ts>
        EventImpl(Ts&&... ts) : proxy_instance_{std::make_shared<EventProxy>(std::forward<Ts>(ts)...)}
        {
        }
        ~EventImpl() = default;

        void TriggerEvent(Args... args)
        {
            std::lock_guard<Mutex> lock_guard{mutex_};
            proxy_instance_->TriggerEvent(std::forward<Args>(args)...);
        }
        void TriggerEventReversely(Args... args)
        {
            std::lock_guard<Mutex> lock_guard{mutex_};
            proxy_instance_->TriggerEventReversely(std::forward<Args>(args)...);
        }
        template <class Function = FunctionType>
        const TokenType AddFunction(Function&& function)
        {
            std::lock_guard<Mutex> lock_guard{mutex_};
            return proxy_instance_->AddFunction(std::forward<Function>(function));
        }
        void DeleteFunction(const TokenType index)
        {
            std::lock_guard<Mutex> lock_guard{mutex_};
            proxy_instance_->DeleteFunction(std::move(index));
        }
        std::weak_ptr<EventProxy> GetWeakPtr() const noexcept
        {
            return proxy_instance_;
        }

    private:
        Mutex mutex_;
        std::shared_ptr<EventProxy> proxy_instance_;
    };

    template <class Function,
              class Mutex,
              class TokenType,
              class FunctionContainer>
    class Event;
    template <
        template <class...>
        class Function,
        class Mutex,
        class TokenTy,
        template <class...>
        class FunctionContainer,
        class StorageType,
        class... CunstomFunctionContainerArgs,
        class... Args>
    class Event<Function<EventState(Args...)>,
                Mutex,
                TokenTy,
                FunctionContainer<StorageType, CunstomFunctionContainerArgs...>>
        : public EventImpl<Function<EventState(Args...)>, Mutex, TokenTy, StorageType, FunctionContainer<StorageType, CunstomFunctionContainerArgs...>, Args...>
    {
        using EventBase = EventImpl<Function<EventState(Args...)>, Mutex, TokenTy, StorageType, FunctionContainer<StorageType, CunstomFunctionContainerArgs...>, Args...>;
        using EventBase::EventBase;
    };
    template <
        class Mutex,
        class TokenTy,
        template <class...>
        class FunctionContainer,
        class StorageType,
        class... CunstomFunctionContainerArgs,
        class... Args>
    class Event<EventState (*)(Args...),
                Mutex,
                TokenTy,
                FunctionContainer<StorageType, CunstomFunctionContainerArgs...>>
        : public EventImpl<EventState (*)(Args...), Mutex, TokenTy, StorageType, FunctionContainer<StorageType, CunstomFunctionContainerArgs...>, Args...>
    {
        using EventBase = EventImpl<EventState (*)(Args...), Mutex, TokenTy, StorageType, FunctionContainer<StorageType, CunstomFunctionContainerArgs...>, Args...>;
        using EventBase::EventBase;
    };

    template <class T>
    struct event_function_type
    {
        using type = typename T::FunctionType;
    };
    template <class T>
    using event_function_type_t = typename event_function_type<T>::type;

    template <typename T>
    class EventFunctionGuard
    {
        using TokenTy = typename T::TokenType;

    public:
        EventFunctionGuard(T& ref_event, const typename T::FunctionType& event_function)
            : wp_related_event_{std::move(ref_event.GetWeakPtr())}
        {
            token_ = ref_event.AddFunction(event_function);
        }
        EventFunctionGuard(T& ref_event, const TokenTy token) : token_{token}, wp_related_event_{std::move(ref_event.GetWeakPtr())} {}
        EventFunctionGuard() = default;
        EventFunctionGuard(const EventFunctionGuard&) = delete;
        EventFunctionGuard& operator=(const EventFunctionGuard&) = delete;
        ~EventFunctionGuard()
        {
            if (!wp_related_event_.expired())
            {
                wp_related_event_.lock()->DeleteFunction(token_);
            }
        }
        void Reset(T& ref_event, const TokenTy token)
        {
            wp_related_event_ = ref_event.GetWeakPtr();
            token_ = token;
        }
        void Reset(T& ref_event, const typename T::FunctionType& event_function)
        {
            wp_related_event_ = ref_event.GetWeakPtr();
            token_ = ref_event.AddFunction(event_function);
        }

    private:
        std::weak_ptr<typename T::EventProxy> wp_related_event_;
        TokenTy token_;
    };
#if __cplusplus >= 201703L
    template <typename T>
    EventFunctionGuard(T&, const typename T::FuncTy&) -> EventFunctionGuard<T>;
#endif

    template <typename... Args>
    using DefaultEvent = Event<std::function<EventState(Args...)>, std::mutex, std::uint16_t, std::vector<EventFunctionStorageType<std::function<EventState(Args...)>, std::uint16_t>>>;
    template <typename... Args>
    using DefaultFunctionPointerEvent = Event<EventState (*)(Args...), std::mutex, std::uint16_t, std::vector<EventFunctionStorageType<EventState (*)(Args...), std::uint16_t>>>;
}
namespace WindowFramework = Ceiba;

#endif // NERAL_ENGINE_COMMON_EVENT_HPP
