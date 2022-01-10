/***
 * @Author: Dusk
 * @Date: 2021-09-04 14:00:56
 * @FilePath: \NRender\NRender\main.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include <Windows.h>
#include "WindowFramework/include/Window.h"
#include "ObjLoader.hpp"
#include "WindowFramework/include/App.h"
#include "SoftRender/SoftRender.h"
#include "D3D12Render/D3D12Render.h"

#include "Logger.h"
#include "Utils.h"

int main()
{
#if (defined(DEBUG) || defined(_DEBUG))
#ifdef _MSC_VER
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

#endif
    auto hInstance = App::GetHandle();
    App::GetInstance().EnableGDIPlus();
#if !(defined(DEBUG) || defined(_DEBUG))
    App::HideConsoleWindow();
#endif
    auto up_main_window = std::make_unique<Window>(L"MainWindow", L"NRener", L"MainWindow", hInstance);
    auto sp_soft_render = std::make_shared<SoftRender>(*(up_main_window.get()), Vector4fAlignas16{3, 4, 3}, Vector4fAlignas16{-2, -3, -3}, Vector4fAlignas16{1, 0, 0});
    auto sp_d3d12_render = std::make_shared<D3D12Render>(*up_main_window);
    {
        auto* const p_render = sp_soft_render.get();
        sp_soft_render->on_render_event_guard_.Reset(up_main_window->on_rendering_, up_main_window->SetRender(sp_soft_render, [p_render](const RenderEventArgs& e) -> EventState
                                                                                                              { return p_render->Rendering(e); }));
    }
    auto& ref_soft_render = *(sp_soft_render.get());
    ObjLoader obj_loader{"D:\\Sources\\CPP\\GAMES101\\Assignment3\\Code\\out\\build\\models\\spot\\spot_triangulated_good.obj"};
    auto obj_content = obj_loader.ReadOBJ();
    ref_soft_render.SetObjContent(obj_content);
    App::GetInstance().AddAsMainWindow(std::move(up_main_window));
    App::GetInstance().GetWindow(L"MainWindow").Show();
    return App::GetInstance().Run();
}