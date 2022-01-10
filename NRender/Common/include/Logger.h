/***
 * @Author: Dusk
 * @Date: 2021-12-31 00:21:18
 * @FilePath: \NRender\NRender\Common\include\PhoenixLogger.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include <type_traits>
#include "Utils.h"

namespace dusk
{
    struct OutputerBase
    {
        template <typename StringType>
        static void Print(StringType&& type, StringType&& content, StringType&& end_string);
        template <typename Derived>
        struct CheckOutputMethodExist
        {
            constexpr static bool value =
                (sizeof(&Derived::Print) == sizeof(&::dusk::OutputerBase::template Print<::dusk::function_first_argument_type<decltype(Derived::Print)>::type>)) &&
                std::is_same<
                    decltype(&OutputerBase::template Print<function_first_argument_type<decltype(&Derived::Print)>::type>),
                    decltype(&Derived::Print)>::value;
            operator bool() const
            {
                return value;
            }
        };

    protected:
        ~OutputerBase() = default;
    };
    struct LogTypeBase
    {
        template <typename T, typename S = void>
        struct CheckLogTypeStringExist;
        template <typename Derived>
        struct CheckLogTypeStringExist<Derived, decltype(Derived::type_)>
        {
            constexpr static bool value = true;
            constexpr auto GetTypeString() const -> decltype(Derived::type_)
            {
                return Derived::type_;
            }
        };
        template <typename Derived>
        struct CheckLogTypeStringExist<Derived, decltype(Derived::type)>
        {
            constexpr static bool value = true;
            constexpr auto GetTypeString() const -> decltype(Derived::type)
            {
                return Derived::type;
            }
        };
        template <typename Derived>
        struct CheckLogTypeStringExist<Derived, decltype(Derived::Type)>
        {
            constexpr static bool value = true;
            constexpr auto GetTypeString() const -> decltype(Derived::Type)
            {
                return Derived::Type;
            }
        };
        template <typename Derived>
        struct CheckLogTypeStringExist<Derived, void>
        {
            constexpr static bool value = false;
        };

    protected:
        ~LogTypeBase() = default;
    };
    namespace LogType
    {
        struct Error : public LogTypeBase
        {
        };
    }
    namespace OutputerType
    {
        struct Console : public OutputerBase
        {
        };
    }
    template <typename Type, typename Outputer, typename StringType1, typename StringType2 = StringType1,
              typename = typename std::enable_if<
                  std::is_base_of<dusk::OutputerBase, Type>::value && std::is_base_of<dusk::LogTypeBase, Outputer>::value>::type>
    inline void Log(StringType1&& content, StringType2&& end_string = '\n')
    {
        static_assert(dusk::OutputerBase::CheckOutputMethodExist<Outputer>::value, "You should override function Print(...) which in class OutputerBase!");
        static_assert(dusk::LogTypeBase::CheckLogTypeStringExist<Type>::value, "Need static data member Type::type_ or Type::type or Type::Type_ in class Type!");
        Outputer::Print(
            std::forward<StringType1>(dusk::LogTypeBase::CheckLogTypeStringExist<Type>::GetTypeString()),
            std::forward<StringType1>(content),
            std::forward<StringType2>(end_string));
    }
}