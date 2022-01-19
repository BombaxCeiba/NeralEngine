/***
 * @Author: Dusk
 * @Date: 2021-09-04 14:00:56
 * @FilePath: \NRender\NRender\WindowFramework\source\Window.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include "../include/Window.h"

constexpr INT DefaultWidth = 1024;
constexpr INT DefaultHeight = 768;

Window::Window(
#ifdef UNICODE
    const std::wstring& class_name,
    const std::wstring& window_title
#else
    const std::string& class_name,
    const std::string& window_title
#endif
    ,
    const std::wstring& window_name, HINSTANCE app_instance, typename decltype(on_loaded_)::Type on_loaded)
    : error_code_(0), hWnd_(nullptr), width_(DefaultWidth), height_(DefaultHeight), state_(InitializationState::Success), name_(window_name),
      on_loaded_{on_loaded},
      on_size_changed_{[this](const ceiba::SizeChangedEventArgs& size_changed_event_args) -> ceiba::EventState
                       {
                           this->width_ = size_changed_event_args.new_width_;
                           this->height_ = size_changed_event_args.new_height_;
                           return ceiba::EventState::Continue;
                       }},
      on_closed_{},
      on_closing_{[this]() -> ceiba::EventState
                  {
                      //MessageBox(hWnd_,L"on_closing",L"",MB_OKCANCEL);
                      ::DestroyWindow(hWnd_);
                      hWnd_ = nullptr;
                      return ceiba::EventState::Continue;
                  }},
      on_show_{[](const HWND hWnd) -> ceiba::EventState
               {
                   ::ShowWindow(hWnd, SW_SHOW);
                   ::UpdateWindow(hWnd);
                   return ceiba::EventState::Continue;
               }},
      on_hide_{[](const HWND hWnd) -> ceiba::EventState
               {
                   ::ShowWindow(hWnd, SW_HIDE);
                   return ceiba::EventState::Continue;
               }},
      on_rendering_{}
{
    WNDCLASS window_class;
    //window_class.cbSize = sizeof(window_class);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = &FunctionForInitialization;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = app_instance;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    //window_class.hIconSm = LoadIcon(app_instance, MAKEINTRESOURCE(IDI_APPLICATION));
    window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    window_class.lpszMenuName = NULL;
    window_class.lpszClassName = class_name.c_str();
    ATOM regiset_result = ::RegisterClass(&window_class);
    if (regiset_result == false)
    {
        error_code_ = GetLastError();
        state_ = InitializationState::Failure;
        return;
    }
    hWnd_ = ::CreateWindow(
        class_name.c_str(),       // name of window class
        window_title.c_str(),     // title-bar string
        WS_OVERLAPPEDWINDOW,      // top-level window
        CW_USEDEFAULT,            // default horizontal position
        CW_USEDEFAULT,            // default vertical position
        DefaultWidth,             // default width
        DefaultHeight,            // default height
        static_cast<HWND>(NULL),  // no owner window
        static_cast<HMENU>(NULL), // use class menu
        app_instance,             // handle to application instance
        this);                    // no window-creation data
    if (!hWnd_)
    {
        error_code_ = GetLastError();
        state_ = InitializationState::Failure;
    }
}

void Window::Show()
{
    on_show_.TriggerEvent(hWnd_);
}

void Window::Hide()
{
    on_hide_.TriggerEvent(hWnd_);
}

int32_t Window::GetWidth() const
{
    return width_;
}

int32_t Window::GetHeight() const
{
    return height_;
}

HWND Window::GetHwnd()
{
    return hWnd_;
}

void Window::Update()
{
    ::RedrawWindow(hWnd_, nullptr, nullptr, RDW_INVALIDATE | RDW_INVALIDATE);
}

LRESULT Window::FunctionForInitialization(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_NCCREATE)
    {
        auto* p_this = reinterpret_cast<Window*>((reinterpret_cast<LPCREATESTRUCT>(lParam))->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(p_this));
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&CustomProcess));
        p_this->on_loaded_.TriggerEvent(hwnd);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT Window::CustomProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto* const p_this = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        BeginPaint(hwnd, &ps);
        ceiba::RenderEventArgs render_event_args{ps};
        p_this->on_rendering_.TriggerEvent(render_event_args);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_KEYDOWN:
    {
        ceiba::KeyEventArgs key_event_args{wParam, lParam};
        p_this->on_key_down_.TriggerEvent(key_event_args);
        break;
    }
    case WM_SIZE:
    {
        ceiba::SizeChangedEventArgs size_changed_event_args{
            p_this->width_, p_this->height_,
            LOWORD(lParam), HIWORD(lParam)};
        p_this->on_size_changed_.TriggerEvent(size_changed_event_args);
        break;
    }
    case WM_CLOSE:
        p_this->on_closing_.TriggerEventReversely();
        if (p_this->hWnd_ == nullptr)
        {
            g_window_destructor(*p_this);
        }
        break;
    case WM_NCDESTROY:
        p_this->on_closed_.TriggerEventReversely();
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}