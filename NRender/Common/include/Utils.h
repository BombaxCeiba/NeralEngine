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
    template <typename R, typename First, typename... Args>
    struct function_first_argument_type<R(First, Args...)>
    {
        using type = First;
    };

    template <typename T, std::size_t Length>
    std::size_t GetLength(const T (&data)[Length])
    {
        return Length;
    }

    template <typename Result, typename StaticString, typename Tag>
    struct Asserter;
    template <typename Result, typename StaticString>
    struct Asserter<Result, StaticString, typename std::enable_if<std::is_same<decltype(Result::value), bool>::value>::type>
    {
        constexpr static bool value = Result::value;
        constexpr operator bool() const { return value; }
        static_assert(Result::value);
    };
}