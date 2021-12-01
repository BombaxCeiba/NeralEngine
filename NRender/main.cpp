//***************************************************************************************
//main.cpp by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
#include <Windows.h>
#include "WindowFramework/include/Window.h"
#include "ObjLoader.hpp"
#include "WindowFramework/include/App.h"
#include "SoftRender/SoftRender.h"

int main()
{
#if _DEBUG
#if (MSC_VER)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
#endif
    auto hInstance = GetModuleHandle(NULL);
    auto up_main_window = std::make_unique<Window>(L"MainWindow", L"NRener", L"MainWindow", hInstance);
    auto up_soft_render = std::make_unique<SoftRender>(*(up_main_window.get()), Vector4fAlignas16{ 3, 4, 3 }, Vector4fAlignas16{ -2,-3,-3 }, Vector4fAlignas16{ 1,0,0 });
    auto& ref_soft_render = *(up_soft_render.get());
    up_main_window->SetRender(std::move(up_soft_render));
    ObjLoader obj_loader{"D:\\Sources\\CPP\\GAMES101\\Assignment3\\Code\\out\\build\\models\\spot\\spot_triangulated_good.obj"};
    auto obj_content = obj_loader.ReadOBJ();
    ref_soft_render.SetObjContent(obj_content);
    App::GetInstance().AddAsMainWindow(std::move(up_main_window));
    App::GetInstance().GetWindow(L"MainWindow").Show();
    return App::GetInstance().Run();
}