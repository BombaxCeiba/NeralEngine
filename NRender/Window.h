#pragma once
//***************************************************************************************
//Window.h by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
#include "Event.h"
#include <Windows.h>
#include <string>
#include <functional>

class RenderBase
{
public:
    RenderBase() = default;
    virtual ~RenderBase() = default;
};

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
    Window(
#ifdef UNICODE
        const std::wstring& class_name,
        const std::wstring& window_title
#else
        const std::string& class_name,
        const std::string& window_title
#endif
        , const std::wstring& window_name, HINSTANCE app_instance, Event<std::function<EventState(const HWND hwnd)>> on_loaded_event = {});
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    ~Window() = default;
    void Show();
    void Hide();
    int32_t GetWidth() const;
    int32_t GetHeight() const;
    void SetRender(std::unique_ptr<RenderBase> up_render);
    HWND GetHwnd();
    InitializationState state_;
    DWORD error_code_;
    Event<std::function<EventState(const HWND)>> on_loaded_;
    Event<std::function<EventState(const RenderEventArgs&)>> on_render_;
    Event<std::function<EventState()>> on_closing_;
    Event<std::function<EventState()>> on_closed_;
    Event<std::function<EventState(const HWND)>> on_show_;
    Event<std::function<EventState(const HWND)>> on_hide_;
    Event<std::function<EventState(const SizeChangedEventArgs&)>> on_size_changed_;
    const std::wstring name_;
private:
    static LRESULT CALLBACK FunctionForInitialization(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK CustomProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND hWnd_;
    std::unique_ptr<RenderBase> up_render_;//此成员必须晚于事件成员初始化
    uint32_t width_;
    uint32_t height_;
};