//***************************************************************************************
//Event.h by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
#pragma once
#include <functional>
#include <cstdint>
#include <list>
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
    SizeChangedEventArgs(uint32_t previous_width,uint32_t previous_height,uint32_t new_width,uint32_t new_height)
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

template <typename T>
class ReversionWrapper
{
public:
    T& raw_data;
};

template <typename T>
ReversionWrapper(T&)->ReversionWrapper<T>;

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

template<typename Func>
class Event;

template<typename... Args>
class Event<std::function<EventState(Args...)>>
{
public:
    using TokenTy = std::uint16_t;
    using FuncTy = std::function<EventState(Args...)>;
    using FunctionList = std::list<std::pair<FuncTy, TokenTy>>;
    Event() :function_list_{}, current_token_(0) {}
    Event(const FuncTy default_function) :function_list_{}
    {
        current_token_ = 0;
        function_list_.push_back(std::make_pair(default_function, current_token_));
    }
    explicit Event(size_t function_count) :function_list_{ function_count }, current_token_(0) {}
    Event(const FuncTy default_function,const size_t function_count) :function_list_{ function_count }
    {
        current_token_ = 0;
        function_list_.push_back(std::make_pair(default_function, current_token_));
    }

    ~Event() = default;
    void TriggerEvent(Args... args)
    {
        for (auto func : function_list_)
        {
            if (func.first)
            {
                if (func.first(std::forward<Args>(args)...) == EventState::End)
                {
                    break;
                }
            }
        }
    }
    void TriggerEventReversely(Args... args)
    {
        for (auto func : ReversionWrapper{ function_list_ })
        {
            if (func.first)
            {
                if (func.first(std::forward<Args>(args)...) == EventState::End)
                {
                    break;
                }
            }
        }
        //auto rit = function_list_.rbegin();
        //auto end = function_list_.rend();
        //while (rit != end)
        //{
        //    if (rit.operator->()->first)
        //    {
        //        if (rit.operator->()->first(std::forward<Args>(args)...) == EventState::End)
        //        {
        //            break;
        //        }
        //    }
        //    rit++;
        //}
    }
    const TokenTy AddFunction(const FuncTy& function)
    {
        current_token_++;
        function_list_.emplace_back(std::make_pair(function, current_token_));
        return current_token_;
    }
    void DeleteFunction(const TokenTy index)
    {
        for (auto it = function_list_.begin(); it != function_list_.end() ; it++)
        {
            if (it->second == index)
            {
                function_list_.erase(it);
                break;
            }
        }
    }
protected:
private:
    FunctionList function_list_;
    TokenTy current_token_;
};

template<typename T>
class EventFunctionGuard
{
public:
    EventFunctionGuard(T& ref_event, const typename T::FuncTy& event_function)
        :ref_related_event_{ ref_event }
    {
        token_ = ref_related_event_.AddFunction(event_function);
    }
    EventFunctionGuard() = delete;
    EventFunctionGuard(const EventFunctionGuard&) = delete;
    EventFunctionGuard& operator=(const EventFunctionGuard&) = delete;
    ~EventFunctionGuard()
    {
        ref_related_event_.DeleteFunction(token_);
    }
private:
    T& ref_related_event_;
    typename T::TokenTy token_;
};