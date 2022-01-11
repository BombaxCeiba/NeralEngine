//***************************************************************************************
//Event.h by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
#pragma once
#include <stdint.h>
#include <functional>
#include <cstdint>
#include <vector>
#include <atomic>
#include <memory>
#include <Windows.h>

namespace ceiba
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

    class MouseEventArgs : public EventArgsBase
    {
    public:
        enum class Key
        {
            LeftButton = 1,
            RightButton = 2,
            MiddleButton = 4
        };
        Key pressed_key_;
        ::ceiba::Point2i32 Location;
    };

    template <typename T>
    class ReversionWrapper
    {
    public:
        T& raw_data;
    };

    template <typename T>
    auto begin(ReversionWrapper<T> rw) { return std::rbegin(rw.raw_data); }

    template <typename T>
    auto end(ReversionWrapper<T> rw) { return std::rend(rw.raw_data); }

#if (__cplusplus >= 201402L)
    template <typename T>
    auto cbegin(ReversionWrapper<T> rw)
    {
        return std::crbegin(rw.raw_data);
    }

    template <typename T>
    auto cend(ReversionWrapper<T> rw) { return std::crend(rw.raw_data); }
#endif
    template <typename T>
    ReversionWrapper<T> reverse(T&& raw_data)
    {
        return {raw_data};
    }

    template <typename Func>
    class Event;
    template <typename... Args>
    class Event<std::function<EventState(Args...)>>
    {
    public:
        using TokenType = std::uint16_t;
        using Type = std::function<EventState(Args...)>;
        using FunctionContainer = std::vector<std::pair<Type, TokenType>>;
        class EventProxy
        {
            //下面的inline都是玄学
        public:
            EventProxy() : function_container_{0}, current_token_{0}
            {
                function_container_.reserve(3);
            }
            EventProxy(const Type default_function) : function_container_{0}, current_token_{0}
            {
                function_container_.reserve(3);
                function_container_.push_back(std::make_pair(default_function, current_token_));
            }
            explicit EventProxy(size_t function_count) : function_container_{0}, current_token_(0)
            {
                function_container_.reserve(function_count);
            }
            EventProxy(const Type default_function, const size_t function_count) : function_container_{0}, current_token_{0}
            {
                function_container_.reserve(function_count);
                function_container_.push_back(std::make_pair(default_function, current_token_));
            }
            ~EventProxy() = default;
            inline void TriggerEvent(Args... args)
            {
                for (auto func : function_container_)
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
                for (auto func : reverse(function_container_))
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
            inline const TokenType AddFunction(const Type& function)
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
        template <typename... Ts>
        Event(Ts&&... ts) : proxy_instance_{std::make_shared<EventProxy>(std::forward<Ts>(ts)...)} {}
        ~Event() = default;

        void TriggerEvent(Args... args)
        {
            proxy_instance_->TriggerEvent(std::forward<Args>(args)...);
        }
        void TriggerEventReversely(Args... args)
        {
            proxy_instance_->TriggerEventReversely(std::forward<Args>(args)...);
        }
        const TokenType AddFunction(const Type& function)
        {
            return proxy_instance_->AddFunction(std::forward<const Type&>(function));
        }
        void DeleteFunction(const TokenType index)
        {
            proxy_instance_->DeleteFunction(std::move(index));
        }
        std::weak_ptr<EventProxy> GetWeakPtr() const noexcept
        {
            return proxy_instance_;
        }

    private:
        std::shared_ptr<EventProxy> proxy_instance_;
    };

    template <typename T>
    class EventFunctionGuard
    {
        using TokenTy = typename T::TokenType;

    public:
        EventFunctionGuard(T& ref_event, const typename T::Type& event_function)
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

    private:
        std::weak_ptr<typename T::EventProxy> wp_related_event_;
        TokenTy token_;
    };
#if __cplusplus >= 201703L
    template <typename T>
    EventFunctionGuard(T&, const typename T::FuncTy&) -> EventFunctionGuard<T>;
#endif

template<typename... Args>
using make_event = Event<std::function<EventState(Args...)>>;

}
namespace WindowFramework = ceiba;
