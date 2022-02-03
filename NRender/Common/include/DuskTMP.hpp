/***
 * @Author: Dusk
 * @Date: 2022-01-31 12:19:00
 * @FilePath: \NRender\NRender\Common\include\DuskTMP.hpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <type_traits>

namespace dusk
{
    template <class T>
    struct function_first_argument_type;
    template <class C, class R, class First, class... Args>
    struct function_first_argument_type<R (C::*)(First, Args...)>
    {
        using type = First;
    };

    template <class Container, class... Ts>
    struct rebind_container;
    template <template <class...> class Container, class NewType, class... NewArgs, class... OldArgs>
    struct rebind_container<Container<OldArgs...>, NewType, NewArgs...>
    {
        using type = Container<NewType, NewArgs...>;
    };
    template <class Container, class NewType, class... NewArgs>
    using rebind_container_t = typename rebind_container<Container, NewType, NewArgs...>::type;

    template <class T>
    struct get_fist_template_argument;
    template <template <class...> class T, class TFistArg, class... Others>
    struct get_fist_template_argument<T<TFistArg, Others...>>
    {
        using type = TFistArg;
    };
    template <class T>
    using get_fist_template_argument_t = typename get_fist_template_argument<T>::type;

    template <typename... Ts>
    struct make_void
    {
        using type = void;
    };
    template <typename... Ts>
    using void_t = typename make_void<Ts...>::type;

} // namespace dusk
