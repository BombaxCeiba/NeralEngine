/***
 * @Author: Dusk
 * @Date: 2022-01-28 23:05:12
 * @FilePath: \NRender\NRender\Common\include\WinAPIHelper.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <memory>
#include <Windows.h>

namespace dusk
{
    constexpr static DWORD EMPTY_FLAG = 0;

    int EvaluateWideCharToMultiByteSize(UINT code_page, wchar_t* p_wchar_string, DWORD flags = 0, int wchar_string_length = -1);
    auto AllocateWideCharToMultiByteMemory(UINT code_page, wchar_t* p_wchar_string, DWORD flags = 0, int wchar_string_length = -1)
        ->std::unique_ptr<char[]>;
}