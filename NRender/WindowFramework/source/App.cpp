#include "../include/App.h"

App App::instance{};

App& App::GetInstance()
{
    return instance;
}

void App::HideConsoleWindow()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

HINSTANCE App::GetHandle()
{
    return ::GetModuleHandle(NULL);
}

void App::EnableGDIPlus()
{
    if (gdiplus_token_ == empty_gdi_plus_token_)
    {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput{};
        Gdiplus::GdiplusStartup(&gdiplus_token_, &gdiplusStartupInput, NULL);
    }
}

void App::DeleteWindow(Window& window)
{
    bool is_deleting_main_window = window.GetHwnd() == main_window_;

    for (auto it = windows_.begin(); it != windows_.end(); it++)
    {
        if (it->second.get() == &window)
        {
            it->second.reset();
            windows_.erase(it);
            break;
        }
    }

    switch (quit_mode_)
    {
    case App::QuitMode::WhenMainWindowClosed:
        if (is_deleting_main_window)
        {
            PostQuitMessage(0);
            return;
        }
        //当main_window_无可以匹配时（即没有正确配置main_window_的时候），为防止无法退出，检查是否已经是最后一个窗口
        [[fallthrough]];
    case App::QuitMode::WhenAllWindowClosed:
        if (windows_.empty())
        {
            PostQuitMessage(0);
            return;
        }
        break;
    default:
        break;
    }
}

bool App::AddWindow(UniqueWindowPointer up_window)
{
    if (windows_.find(up_window->name_) == windows_.end() || up_window->state_ == Window::InitializationState::Failure)
    {
        return false;
    }
    windows_.insert({up_window->name_, std::move(up_window)});
    return true;
}

bool App::AddAsMainWindow(UniqueWindowPointer up_window)
{
    if (windows_.find(up_window->name_) != windows_.end() || up_window->state_ == Window::InitializationState::Failure)
    {
        return false;
    }
    main_window_ = up_window->GetHwnd();
    windows_.insert({up_window->name_, std::move(up_window)});
    return true;
}

auto App::GetWindow(const std::wstring& window_name) -> Window&
{
    return *(windows_.find(window_name)->second.get());
}

auto App::GetWindow(const std::wstring& window_name) const -> const Window&
{
    return *(windows_.find(window_name)->second.get());
}

int32_t App::Run()
{
    if (!windows_.empty())
    {
        MSG msg{}; //消息机制
        MSG& ref_msg = msg;
        while (GetMessage(&ref_msg, NULL, 0, 0)) //消息循环
        {
            TranslateMessage(&ref_msg); //将传来的消息翻译
            DispatchMessage(&ref_msg);  //
        }
        return static_cast<int32_t>(msg.wParam);
    }
    return -1;
}

App::App() : main_window_{NULL}, gdiplus_token_{empty_gdi_plus_token_}
{
}

App::~App()
{
    Gdiplus::GdiplusShutdown(gdiplus_token_);
}
