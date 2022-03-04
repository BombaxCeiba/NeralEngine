/**
 * @author: Dusk
 * @date: 2022-01-04 20:21:41
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:12:00
 * @copyright Copyright (c) 2022 Dusk.
 */
#include "../include/Win32APIHelper.h"

int Neral::EvaluateWideCharToMultiByteSize(UINT code_page, wchar_t* p_wchar_string, DWORD flags, int wchar_string_length)
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

auto Neral::AllocateWideCharToMultiByteMemory(UINT code_page, wchar_t* p_wchar_string, DWORD flags, int wchar_string_length)
    -> std::unique_ptr<char[]>
{
    std::size_t memory_size = ::Neral::EvaluateWideCharToMultiByteSize(code_page, p_wchar_string, flags, wchar_string_length);
    auto result = std::make_unique<char[]>(memory_size);
    memset(result.get(), 0, sizeof(char) * memory_size);
    return result;
}
