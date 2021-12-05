//***************************************************************************************
//Event.h by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
#pragma once
#include <functional>
#include <cstdint>
#include <vector>
#include <atomic>
#include <memory>
#include <Windows.h>

enum class EventState
{
    Continue,
    End
};

class EventArgsBase
{
public:
    EventArgsBase() = default;
    virtual ~EventArgsBase() = default;
};

class SizeChangedEventArgs : public EventArgsBase
{
public:
    SizeChangedEventArgs(uint32_t previous_width, uint32_t previous_height, uint32_t new_width, uint32_t new_height)
        :previous_width_{ previous_width }, previous_height_{ previous_height },
        new_width_{ new_width }, new_height_{ new_height } {}
    ~SizeChangedEventArgs() = default;
    uint32_t previous_width_;
    uint32_t previous_height_;
    uint32_t new_width_;
    uint32_t new_height_;
};

class RenderEventArgs : public EventArgsBase
{
public:
    RenderEventArgs(const PAINTSTRUCT& paint_struct) :paint_struct_{ paint_struct } {}
    ~RenderEventArgs() = default;
    const PAINTSTRUCT& paint_struct_;
};

namespace dusk
{
    namespace tools
    {
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

        //template <typename T>
        //auto cbegin(ReversionWrapper<T> rw) { return std::crbegin(rw.raw_data); }
        //
        //template <typename T>
        //auto cend(ReversionWrapper<T> rw) { return std::crend(rw.raw_data); }

        template <typename T>
        ReversionWrapper<T> reverse(T&& raw_data) { return { raw_data }; }
    }
}
template<typename Func>
class Event;

template<typename... Args>
class Event<std::function<EventState(Args...)>>
{
public:
    using TokenTy = std::uint16_t;
    using FuncTy = std::function<EventState(Args...)>;
    using FunctionContainer = std::vector<std::pair<FuncTy, TokenTy>>;
    class EventProxy
    {
        //下面的inline都是玄学
    public:
        EventProxy() :function_container_{ 0 }, current_token_{ 0 }
        {
            function_container_.reserve(3);
        }
        EventProxy(const FuncTy default_function) :function_container_{ 0 }, current_token_{ 0 }
        {
            function_container_.reserve(3);
            function_container_.push_back(std::make_pair(default_function, current_token_));
        }
        explicit EventProxy(size_t function_count) :function_container_{ 0 }, current_token_(0)
        {
            function_container_.reserve(function_count);
        }
        EventProxy(const FuncTy default_function, const size_t function_count) :function_container_{ 0 }, current_token_{ 0 }
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
            for (auto func : dusk::tools::reverse(function_container_))
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
        inline const TokenTy AddFunction(const FuncTy& function)
        {
            ++current_token_;
            function_container_.emplace_back(std::make_pair(function, current_token_));
            return current_token_;
        }
        inline void DeleteFunction(const TokenTy index)
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
        TokenTy current_token_;
    };
    template<typename... Ts>
    Event(Ts&&... ts) :proxy_instance_{ std::make_shared<EventProxy>(std::forward<Ts>(ts)...) } {}
    ~Event() = default;

    void TriggerEvent(Args... args)
    {
        proxy_instance_->TriggerEvent(std::forward<Args>(args)...);
    }
    void TriggerEventReversely(Args... args)
    {
        proxy_instance_->TriggerEventReversely(std::forward<Args>(args)...);
    }
    const TokenTy AddFunction(const FuncTy & function)
    {
        return proxy_instance_->AddFunction(std::forward<const FuncTy&>(function));
    }
    void DeleteFunction(const TokenTy index)
    {
        proxy_instance_->DeleteFunction(std::move(index));
    }
    std::weak_ptr<EventProxy> GetWeakPtr()const noexcept
    {
        return proxy_instance_;
    }
private:
    std::shared_ptr<EventProxy> proxy_instance_;
};

template<typename T>
class EventFunctionGuard
{
    using TokenTy = typename T::TokenTy;
public:
    EventFunctionGuard(T& ref_event, const typename T::FuncTy& event_function)
        :wp_related_event_{ std::move(ref_event.GetWeakPtr()) }
    {
        token_ = ref_event.AddFunction(event_function);
    }
    EventFunctionGuard(T& ref_event, const TokenTy token) :token_{ token }, wp_related_event_{ std::move(ref_event.GetWeakPtr()) }{}
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
template<typename T>
EventFunctionGuard(T&, const typename T::FuncTy&)->EventFunctionGuard<T>;
#endif