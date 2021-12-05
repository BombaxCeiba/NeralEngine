/***
 * @Author: Dusk
 * @Date: 2021-09-04 14:00:56
 * @FilePath: \NRender\NRender\WindowFramework\include\Window.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <string>
#include <functional>
#include <memory>
#include <Windows.h>
#include "Event.h"
#include "RenderBase.h"

class Window;

extern std::function<void(Window&)> g_window_destructor;

class Window
{
public:
    enum class InitializationState
    {
        Success,
        Failure
    };
    InitializationState state_;
    DWORD error_code_;
    Event<std::function<EventState(const HWND)>> on_loaded_;
    Event<std::function<EventState(const RenderEventArgs&)>> on_rendering_;
    Event<std::function<EventState()>> on_closing_;//事件将被逆序执行
    Event<std::function<EventState()>> on_closed_;//事件将被逆序执行
    Event<std::function<EventState(const HWND)>> on_show_;
    Event<std::function<EventState(const HWND)>> on_hide_;
    Event<std::function<EventState(const SizeChangedEventArgs&)>> on_size_changed_;
    const std::wstring name_;

    Window(
#ifdef UNICODE
        const std::wstring& class_name,
        const std::wstring& window_title
#else
        const std::string& class_name,
        const std::string& window_title
#endif
        , const std::wstring& window_name, HINSTANCE app_instance, std::function<EventState(const HWND hwnd)> on_loaded_event = {});
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    virtual ~Window() = default;
    void Show();
    void Hide();
    int32_t GetWidth() const;
    int32_t GetHeight() const;
    template<typename RenderImpl, typename = typename std::enable_if<
            std::is_base_of<RenderBase, RenderImpl>::value && !std::is_same<RenderBase, RenderImpl>::value>
        ::type>
    auto SetRender(std::shared_ptr<RenderImpl> sp_render, const typename decltype(on_rendering_)::FuncTy& rendering)
        ->typename decltype(on_rendering_)::TokenTy
    {
        sp_render_ = sp_render;
        return on_rendering_.AddFunction(rendering);
    }
    template<typename RenderImpl, typename = typename std::enable_if<
            std::is_base_of<RenderBase, RenderImpl>::value && !std::is_same<RenderBase, RenderImpl>::value>
        ::type>
    auto SetRender1(std::shared_ptr<RenderImpl> sp_render, const typename decltype(on_rendering_)::FuncTy& rendering)
        ->EventFunctionGuard<decltype(on_rendering_)>
    {
        sp_render_ = sp_render;
        return { on_rendering_,on_rendering_.AddFunction(rendering) };
    }
    HWND GetHwnd();
private:
    static LRESULT CALLBACK FunctionForInitialization(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK CustomProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND hWnd_;
    std::shared_ptr<RenderBase> sp_render_;//此成员必须晚于事件成员初始化
    uint32_t width_;
    uint32_t height_;
};