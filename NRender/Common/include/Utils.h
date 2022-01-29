/***
 * @Author: Dusk
 * @Date: 2021-12-03 17:59:24
 * @LastEditTime: 2021-12-03 17:59:24
 * @LastEditors: Dusk
 * @FilePath: \NRender\NRender\Common\include\Utils.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <type_traits>
#include <utility>

#if defined(_MSC_VER)
#define EXPORT_SYMBOL __declspec(dllexport)
#elif defined(__clang__)
#define EXPORT_SYMBOL __attribute__((visibility("default")))
#elif defined(__INTEL_COMPILER)
static_assert(false, "Unsupport!")
/*Unsupport!*/
#elif defined(__GNUC__)
#define EXPORT_SYMBOL __attribute__((visibility("default")))
#endif

namespace dusk
{
    template <typename T>
    struct function_first_argument_type;
    template <typename C, typename R, typename First, typename... Args>
    struct function_first_argument_type<R (C::*)(First, Args...)>
    {
        using type = First;
    };

    template <typename T, std::size_t Length>
    constexpr inline std::size_t GetLength(const T (&data)[Length])
    {
        return Length;
    }
    template <typename T, typename C, std::size_t Length>
    constexpr inline std::size_t GetLength(const T (C::*(data))[Length])
    {
        return Length;
    }
    template <typename T, typename C, std::size_t Length>
    constexpr inline std::size_t GetLength(T (C::*(data))[Length])
    {
        return Length;
    }

    template <typename MemoryType>
    inline void SetMemoryZero(MemoryType* p_memory_to_set_zero, std::size_t element_length)
    {
        memset(p_memory_to_set_zero, 0, sizeof(MemoryType) * element_length);
    }

}