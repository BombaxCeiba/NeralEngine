#pragma once
//***************************************************************************************
//App.h by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
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
    void DeleteWindow(Window& p_window);
    bool AddWindow(UniqueWindowPointer up_window);
    bool AddAsMainWindow(UniqueWindowPointer up_window);
    auto GetWindow(const std::wstring& window_name)->WindowHashMap::iterator;
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