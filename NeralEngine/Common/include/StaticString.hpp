/**
 * @author: Dusk
 * @date: 2022-01-10 16:19:32
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:14:58
 * @copyright Copyright (c) 2022 Dusk.
 */
#ifndef NERAL_ENGINE_COMMON_STATICSTRING_H
#define NERAL_ENGINE_COMMON_STATICSTRING_H

#include <iostream>
#include <utility>

namespace Neral
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
    constexpr inline bool operator==(static_string<Chars...>, static_string<Chars...>)
    {
        return true;
    }
    template <char... CharLHS, char... CharRHS>
    inline bool operator==(static_string<CharLHS...>, static_string<CharRHS...>)
    {
        return false;
    }

    template <char... Chars>
    auto AutoEndString(static_string<Chars...>) -> decltype(MakeFixStringOld(static_string<Chars>{}...));
    template <char... Previous, char Current, char... After>
    auto MakeFixStringOld(static_string<Previous...>, static_string<Current>, static_string<After>...)
        -> decltype(MakeFixStringOld(static_string<Previous..., Current>{}, static_string<After>{}...));
    template <char... Previous, char... After>
    auto MakeFixStringOld(static_string<Previous...>, static_string<'\0'>, static_string<After>...)
        -> static_string<Previous...>;

#define NERAL_MAKE_CHAR_INDEX(n, str) ::Neral::GetChar<0x##n##0>(str), ::Neral::GetChar<0x##n##1>(str), \
                                     ::Neral::GetChar<0x##n##2>(str), ::Neral::GetChar<0x##n##3>(str), \
                                     ::Neral::GetChar<0x##n##4>(str), ::Neral::GetChar<0x##n##5>(str), \
                                     ::Neral::GetChar<0x##n##6>(str), ::Neral::GetChar<0x##n##7>(str), \
                                     ::Neral::GetChar<0x##n##8>(str), ::Neral::GetChar<0x##n##9>(str), \
                                     ::Neral::GetChar<0x##n##a>(str), ::Neral::GetChar<0x##n##b>(str), \
                                     ::Neral::GetChar<0x##n##c>(str), ::Neral::GetChar<0x##n##d>(str), \
                                     ::Neral::GetChar<0x##n##e>(str), ::Neral::GetChar<0x##n##f>(str)
#define NERAL_MAKE_CHAR_INDEX_64_LOW(str) NERAL_MAKE_CHAR_INDEX(0, str), NERAL_MAKE_CHAR_INDEX(1, str), \
                                         NERAL_MAKE_CHAR_INDEX(2, str), NERAL_MAKE_CHAR_INDEX(3, str)
#define NERAL_MAKE_CHAR_INDEX_64_HEIGHT(str) NERAL_MAKE_CHAR_INDEX(4, str), NERAL_MAKE_CHAR_INDEX(5, str), \
                                            NERAL_MAKE_CHAR_INDEX(6, str), NERAL_MAKE_CHAR_INDEX(7, str)
#define NERAL_MAKE_CHAR_INDEX_SEQUENCE(str) NERAL_MAKE_CHAR_INDEX_64_LOW(str), NERAL_MAKE_CHAR_INDEX_64_HEIGHT(str)

#define NERAL_MAKE_STATIC_STRING_TYPE(str) decltype(::Neral::AutoEndString(::Neral::static_string<NERAL_MAKE_CHAR_INDEX_SEQUENCE(str)>{}))
#define NERAL_MAKE_STATIC_STRING(str) \
    NERAL_MAKE_STATIC_STRING_TYPE(str) {}
} // namespace Neral
#endif // NERAL_ENGINE_COMMON_STATICSTRING_H
