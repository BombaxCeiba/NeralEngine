/***
 * @Author: Dusk
 * @Date: 2022-01-04 20:21:41
 * @FilePath: \NRender\NRender\Common\source\Win32APIHelper.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include "Win32APIHelper.h"

int dusk::EvaluateWideCharToMultiByteSize(UINT code_page, wchar_t* p_wchar_string, DWORD flags, int wchar_string_length)
{
    return ::WideCharToMultiByte(
        code_page,
        flags,
        p_wchar_string,
        -1,
        nullptr,
        0,
        nullptr,
        nullptr);
}

auto dusk::AllocateWideCharToMultiByteMemory(UINT code_page, wchar_t* p_wchar_string, DWORD flags, int wchar_string_length)
    -> std::unique_ptr<char[]>
{
    std::size_t memory_size = ::dusk::EvaluateWideCharToMultiByteSize(code_page, p_wchar_string, flags, wchar_string_length);
    auto result = std::make_unique<char[]>(memory_size);
    memset(result.get(), 0, sizeof(char) * memory_size);
    return result;
}
