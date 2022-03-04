/**
 * @author: Dusk
 * @date: 2022-01-28 23:05:12
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:13:03
 * @copyright Copyright (c) 2022 Dusk.
 */
#ifndef NERAL_ENGINE_COMMON_WIN32APIHELPER_H
#define NERAL_ENGINE_COMMON_WIN32APIHELPER_H

#include <memory>
#include <Windows.h>

namespace dusk
{
    constexpr static DWORD EMPTY_FLAG = 0;

    int EvaluateWideCharToMultiByteSize(UINT code_page, wchar_t* p_wchar_string, DWORD flags = 0, int wchar_string_length = -1);
    auto AllocateWideCharToMultiByteMemory(UINT code_page, wchar_t* p_wchar_string, DWORD flags = 0, int wchar_string_length = -1)
        -> std::unique_ptr<char[]>;

}
#endif // NERAL_ENGINE_COMMON_WIN32APIHELPER_H
