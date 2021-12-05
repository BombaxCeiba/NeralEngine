/***
 * @Author: Dusk
 * @Date: 2021-12-03 17:59:24
 * @LastEditTime: 2021-12-03 17:59:24
 * @LastEditors: Dusk
 * @FilePath: \NRender\NRender\Common\include\Utils.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#if defined(_MSC_VER)
#define __DLL_EXPORT__ __declspec(dllexport)
#elif defined(__clang__)
#define __DLL_EXPORT__ __attribute__((visibility(default)))
#elif defined(__INTEL_COMPILER)
/*Unsupport!*/
#elif defined(__GNUC__)
#define __DLL_EXPORT__ __attribute__((visibility(default)))