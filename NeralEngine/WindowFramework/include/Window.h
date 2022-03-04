/**
 * @author: Dusk
 * @date: 2021-09-04 14:00:56
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:32:49
 * @copyright Copyright (c) 2022 Dusk.
 */
#ifndef CEIBA_WINDOW_H
#define CEIBA_WINDOW_H

#include <string>
#include <functional>
#include <memory>
#include <Windows.h>
#include "Common/include/Event.hpp"
#include "Common/include/RenderBase.h"

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

    ceiba::DefaultEvent<const HWND> on_loaded_;
    ceiba::DefaultEvent<const ::ceiba::RenderEventArgs&> on_rendering_;
    ceiba::DefaultEvent<> on_closing_; //事件将被逆序执行
    ceiba::DefaultEvent<> on_closed_; //事件将被逆序执行
    ceiba::DefaultEvent<const HWND> on_show_;
    ceiba::DefaultEvent<const HWND> on_hide_;
    ceiba::DefaultEvent<const ::ceiba::MouseEventArgs&> on_mouse_move_;
    ceiba::DefaultEvent<> on_key_up_;
    ceiba::DefaultEvent<const ::ceiba::KeyEventArgs&> on_key_down_;
    ceiba::DefaultEvent<const ::ceiba::SizeChangedEventArgs&> on_size_changed_;

    const std::wstring name_;

    Window(
#ifdef UNICODE
        const std::wstring& class_name,
        const std::wstring& window_title
#else
        const std::string& class_name,
        const std::string& window_title
#endif
        ,
        const std::wstring& window_name, HINSTANCE app_instance, std::function<ceiba::EventState(const HWND hwnd)> on_loaded_event = {});
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    virtual ~Window() = default;
    void Show();
    void Hide();
    int32_t GetWidth() const;
    int32_t GetHeight() const;
    void Update();
    template <typename RenderImpl, typename = typename std::enable_if<
                                       std::is_base_of<RenderBase, RenderImpl>::value && !std::is_same<RenderBase, RenderImpl>::value>::type>
    auto SetRender(std::shared_ptr<RenderImpl> sp_render, const ceiba::event_function_type_t<decltype(on_rendering_)>& rendering)
        -> typename decltype(on_rendering_)::TokenType
    {
        sp_render_ = sp_render;
        return on_rendering_.AddFunction(rendering);
    }
    template <typename RenderImpl, typename = typename std::enable_if<
                                       std::is_base_of<RenderBase, RenderImpl>::value && !std::is_same<RenderBase, RenderImpl>::value>::type>
    auto SetRender1(std::shared_ptr<RenderImpl> sp_render, const typename ceiba::event_function_type_t<decltype(on_rendering_)>& rendering)
        -> ceiba::EventFunctionGuard<decltype(on_rendering_)>
    {
        sp_render_ = sp_render;
        return {on_rendering_, on_rendering_.AddFunction(rendering)};
    }
    HWND GetHwnd();

private:
    static LRESULT CALLBACK FunctionForInitialization(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK CustomProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND hWnd_;
    std::shared_ptr<RenderBase> sp_render_; //此成员必须晚于事件成员初始化
    uint32_t width_;
    uint32_t height_;
};
#endif // CEIBA_WINDOW_H
