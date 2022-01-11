/***
 * @Author: Dusk
 * @Date: 2022-01-10 16:19:32
 * @FilePath: \NRender\NRender\Common\include\StaticString.hpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <iostream>
#include <utility>

namespace dusk
{
    template <std::size_t Index, std::size_t Length>
    constexpr char GetChar(const char (&s)[Length])
    {
        return s[Index < Length ? Index : Length - 1];
    }

    template <char... Chars>
    struct static_string
    {
        constexpr static const std::size_t size = sizeof...(Chars) + 1;
        constexpr static const char data[size] = {Chars..., '\0'};
        friend std::ostream& operator<<(std::ostream& os, const static_string& rhs)
        {
            os << rhs.data;
            return os;
        }
    };

    template <typename String, typename T>
    struct make_static_string;
    template <typename String, typename T, T... Indexs>
    struct make_static_string<String, std::integer_sequence<T, Indexs...>>
    {
        using type = static_string<(String{}.data[Indexs])...>;
    };

    template <char... CharLHS, char... CharRHS>
    inline static_string<CharLHS..., CharRHS...> operator+(static_string<CharLHS...>, static_string<CharRHS...>)
    {
        return {};
    }

    template <char... Chars>
    auto AutoEndString(static_string<Chars...>) -> decltype(MakeFixStringOld(static_string<Chars>{}...));
    template <char... Previous, char Current, char... After>
    auto MakeFixStringOld(static_string<Previous...>, static_string<Current>, static_string<After>...)
        -> decltype(MakeFixStringOld(static_string<Previous..., Current>{}, static_string<After>{}...));
    template <char... Previous, char... After>
    auto MakeFixStringOld(static_string<Previous...>, static_string<'\0'>, static_string<After>...)
        -> static_string<Previous...>;

#define DUSK_MAKE_CHAR_INDEX(n, str) ::dusk::GetChar<0x##n##0>(str), ::dusk::GetChar<0x##n##1>(str), \
                                     ::dusk::GetChar<0x##n##2>(str), ::dusk::GetChar<0x##n##3>(str), \
                                     ::dusk::GetChar<0x##n##4>(str), ::dusk::GetChar<0x##n##5>(str), \
                                     ::dusk::GetChar<0x##n##6>(str), ::dusk::GetChar<0x##n##7>(str), \
                                     ::dusk::GetChar<0x##n##8>(str), ::dusk::GetChar<0x##n##9>(str), \
                                     ::dusk::GetChar<0x##n##a>(str), ::dusk::GetChar<0x##n##b>(str), \
                                     ::dusk::GetChar<0x##n##c>(str), ::dusk::GetChar<0x##n##d>(str), \
                                     ::dusk::GetChar<0x##n##e>(str), ::dusk::GetChar<0x##n##f>(str)
#define DUSK_MAKE_CHAR_INDEX_64_LOW(str) DUSK_MAKE_CHAR_INDEX(0, str), DUSK_MAKE_CHAR_INDEX(1, str), \
                                         DUSK_MAKE_CHAR_INDEX(2, str), DUSK_MAKE_CHAR_INDEX(3, str)
#define DUSK_MAKE_CHAR_INDEX_64_HEIGHT(str) DUSK_MAKE_CHAR_INDEX(4, str), DUSK_MAKE_CHAR_INDEX(5, str), \
                                            DUSK_MAKE_CHAR_INDEX(6, str), DUSK_MAKE_CHAR_INDEX(7, str)
#define DUSK_MAKE_CHAR_INDEX_SEQUENCE(str) DUSK_MAKE_CHAR_INDEX_64_LOW(str), DUSK_MAKE_CHAR_INDEX_64_HEIGHT(str)

#define DUSK_MAKE_STATIC_STRING_TYPE(str) decltype(::dusk::AutoEndString(::dusk::static_string<DUSK_MAKE_CHAR_INDEX_SEQUENCE(str)>{}))
#define DUSK_MAKE_STATIC_STRING(str) \
    DUSK_MAKE_STATIC_STRING_TYPE(str) {}
} // namespace dusk
