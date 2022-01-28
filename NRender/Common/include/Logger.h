/***
 * @Author: Dusk
 * @Date: 2021-12-31 00:21:18
 * @FilePath: \NRender\NRender\Common\include\PhoenixLogger.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <iostream>
#include <type_traits>
#include "Utils.h"

#define DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(log_type, string_type, literal_string) \
    inline string_type log_type(string_type tag)                                           \
    {                                                                                      \
        static string_type log_type_tip_string{literal_string};                            \
        return log_type_tip_string;                                                        \
    }

#define DUSK_LOG_TYPE(log_type)

namespace dusk
{
    namespace LogType
    {
        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Error, std::string, "Error :");
        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Warning, std::string, "Warning :");
        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Debug, std::string, "Debug :")

        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Error, std::wstring, L"Error :");
        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Warning, std::wstring, L"Warning :");
        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Debug, std::wstring, L"Debug :")
    }
    namespace Outputer
    {
        class Console
        {
        public:
            using StringType = std::string;

            template <class LogType, class StringType1 = StringType, class StringType2 = StringType>
            static void Print(LogType log_type_function_pointer, StringType1&& content, StringType2&& end_string = StringType{'\n'})
            {
                std::cout << log_type_function_pointer(StringType{}) << content << end_string;
            }

            static void DisableCoutSyncWithStdio()
            {
                std::ios::sync_with_stdio(false);
            }
        };
    }
    template <class Outputer, class String = typename Outputer::StringType, class String1 = String, class LogType = String (*)(String)>
    inline void Log(LogType log_type_function_pointer, String1&& content)
    {
        Outputer::template Print(
            std::forward<LogType>(log_type_function_pointer),
            std::forward<String>(content));
    }
    template <class Outputer, class String = typename Outputer::StringType, class String1 = String, class String2 = String, class LogType = String (*)(String)>
    inline void Log(LogType log_type_function_pointer, String1&& content, String2&& end_string)
    {
        Outputer::template Print(
            std::forward<LogType>(log_type_function_pointer),
            std::forward<String1>(content),
            std::forward<String2>(end_string));
    }
    template <class Outputer, class String = typename Outputer::StringType, class String1 = String, class LogType = String (*)(String)>
    inline void LogError(String1&& content)
    {
        Log(
            std::forward<LogType>(&LogType::Error),
            std::forward<String1>(content));
    }
    template <class Outputer, class String = typename Outputer::StringType, class String1 = String, class String2 = String, class LogType = String (*)(String)>
    inline void LogError(String1&& content, String2&& end_string)
    {
        Log(
            std::forward<LogType>(&LogType::Error),
            std::forward<String1>(content),
            std::forward<String2>(end_string));
    }
    template <class Outputer, class String = typename Outputer::StringType, class String1 = String, class LogType = String (*)(String)>
    inline void LogWarning(String1&& content)
    {
        Log(
            std::forward<LogType>(&LogType::Warning),
            std::forward<String1>(content));
    }
    template <class Outputer, class String = typename Outputer::StringType, class String1 = String, class String2 = String, class LogType = String (*)(String)>
    inline void LogWarning(String1&& content, String2&& end_string)
    {
        Log(
            std::forward<LogType>(&LogType::Warning),
            std::forward<String1>(content),
            std::forward<String2>(end_string));
    }
    template <class Outputer, class String = typename Outputer::StringType, class String1 = String, class LogType = String (*)(String)>
    inline void LogDebug(String1&& content)
    {
        Log(
            std::forward<LogType>(&LogType::Debug),
            std::forward<String1>(content));
    }
    template <class Outputer, class String = typename Outputer::StringType, class String1 = String, class String2 = String, class LogType = String (*)(String)>
    inline void LogDebug(String1&& content, String2&& end_string)
    {
        Log(
            std::forward<LogType>(&LogType::Debug),
            std::forward<String1>(content),
            std::forward<String2>(end_string));
    }
}