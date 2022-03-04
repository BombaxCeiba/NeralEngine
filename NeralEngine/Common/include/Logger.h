/**
 * @author: Dusk
 * @date: 2021-12-31 00:21:18
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:16:30
 * @copyright Copyright (c) 2022 Dusk.
 */
#ifndef NERAL_ENGINE_COMMON_LOGGER_H
#define NERAL_ENGINE_COMMON_LOGGER_H

#include <iostream>
#include <type_traits>
#include "Utils.h"

#define DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(log_type, string_type, literal_string) \
    inline string_type log_type(string_type tag)                                           \
    {                                                                                      \
        static string_type log_type_tip_string{literal_string};                            \
        return log_type_tip_string;                                                        \
    }

namespace Neral
{
    namespace LogType
    {
        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Error, std::string, "[Error] ");
        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Warning, std::string, "[Warning] ");
        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Debug, std::string, "[Debug] ")

        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Error, std::wstring, L"[Error] ");
        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Warning, std::wstring, L"[Warning] ");
        DUSK_MAKE_LOG_TYPE_AND_LITERAL_STRING_OF_IT(Debug, std::wstring, L"[Debug] ")
    }
    namespace Outputer
    {
        class Console
        {
        public:
            using StringType = std::string;

            template <class LogType, class... StringTypes>
            static void Print(LogType log_type_function_pointer, StringTypes&&... contents)
            {
                std::cout << log_type_function_pointer(StringType{});
                (std::cout << ... << contents) << '\n';
            }

            static void DisableCoutSyncWithStdio()
            {
                std::ios::sync_with_stdio(false);
            }
        };
        class WConsole
        {
        public:
            using StringType = std::wstring;

            template <class LogType, class... StringTypes>
            static void Print(LogType log_type_function_pointer, StringTypes&&... contents)
            {
                std::wcout << log_type_function_pointer(StringType{});
                (std::wcout << ... << contents) << L'\n';
            }

            static void DisableWCoutSyncWithStdio()
            {
                std::ios::sync_with_stdio(false);
            }
        };
    }
    template <class Outputer, class... OutputTypes, class String = typename Outputer::StringType, class LogType = String (*)(String)>
    inline void Log(LogType log_type_function_pointer, OutputTypes&&... contents)
    {
        Outputer::template Print(
            std::forward<LogType>(log_type_function_pointer),
            std::forward<OutputTypes>(contents)...);
    }
    template <class Outputer, class... OutputTypes, class String = typename Outputer::StringType, class LogType = String (*)(String)>
    inline void LogError(OutputTypes&&... contents)
    {
        Log<Outputer>(
            std::forward<LogType>(&::Neral::LogType::Error),
            std::forward<OutputTypes>(contents)...);
    }

    template <class Outputer, class... OutputTypes, class String = typename Outputer::StringType, class LogType = String (*)(String)>
    inline void LogWarning(OutputTypes&&... contents)
    {
        Log<Outputer>(
            std::forward<LogType>(&::Neral::LogType::Warning),
            std::forward<OutputTypes>(contents)...);
    }
    template <class Outputer, class... OutputTypes, class String = typename Outputer::StringType, class LogType = String (*)(String)>
    inline void LogDebug(OutputTypes&&... contents)
    {
        Log<Outputer>(
            std::forward<LogType>(&::Neral::LogType::Debug),
            std::forward<OutputTypes>(contents)...);
    }
}
#endif // NERAL_ENGINE_COMMON_LOGGER_H
