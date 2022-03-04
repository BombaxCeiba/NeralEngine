/**
 * @author: Dusk
 * @date: 2021-09-06 13:59:48
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:31:14
 * @copyright Copyright (c) 2022 Dusk.
 */
#ifndef CEIBA_APP_H
#define CEIBA_APP_H

#include <memory>
#include <unordered_map>
#include <Windows.h>
#include <Gdiplus.h>
#include "Window.h"

class App
{
public:
    using UniqueWindowPointer = std::unique_ptr<Window>;
    using WindowHashMap = std::unordered_map<std::wstring, UniqueWindowPointer>;
    enum class QuitMode
    {
        WhenAllWindowClosed,
        WhenMainWindowClosed,
        WhenExpelicitQuit
    };
    static App& GetInstance();
    static HINSTANCE GetHandle();
    static void HideConsoleWindow();
    void EnableGDIPlus();
    void DeleteWindow(Window& p_window);
    bool AddWindow(UniqueWindowPointer up_window);
    bool AddAsMainWindow(UniqueWindowPointer up_window);
    auto GetWindow(const std::wstring& window_name) -> Window&;
    auto GetWindow(const std::wstring& window_name) const -> const Window&;
    int32_t Run();
    HWND main_window_;
    QuitMode quit_mode_ = QuitMode::WhenAllWindowClosed;

private:
    App();
    ~App();
    constexpr static int empty_gdi_plus_token_ = 0;
    ULONG_PTR gdiplus_token_;
    WindowHashMap windows_;

    static App instance;
};

inline std::function<void(Window&)> g_window_destructor = [](Window& window_to_delete)
{
    App::GetInstance().DeleteWindow(window_to_delete);
};
#endif // CEIBA_APP_H
