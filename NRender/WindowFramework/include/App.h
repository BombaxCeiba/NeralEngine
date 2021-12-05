/***
 * @Author: Dusk
 * @Date: 2021-09-06 13:59:48
 * @LastEditTime: 2021-12-03 11:11:32
 * @LastEditors: Dusk
 * @FilePath: \NRender\NRender\WindowFramework\include\App.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
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
    auto GetWindow(const std::wstring& window_name)->Window&;
    auto GetWindow(const std::wstring& window_name)const->const Window&;
    int32_t Run();
    HWND main_window_;
    QuitMode quit_mode_ = QuitMode::WhenAllWindowClosed;
private:
    App();
    ~App();
    ULONG_PTR gdiplus_token_;
    WindowHashMap windows_;

    static App instance;
};

inline std::function<void(Window&)> g_window_destructor = [](Window& window_to_delete) {
    App::GetInstance().DeleteWindow(window_to_delete);
};