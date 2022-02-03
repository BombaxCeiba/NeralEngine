/***
 * @Author: Dusk
 * @Date: 2022-01-10 15:52:44
 * @FilePath: \NRender\Test\StaticStringTest.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include <string>
#include <iostream>
#include "TestMain.hpp"
#include "StaticString.hpp"

TEST(StaticString, StaticStringTest)
{
    std::string dynamic_string_1("TestStaticString");
    auto static_string_test_1 = DUSK_MAKE_STATIC_STRING("TestStaticString");
    std::cout << typeid(decltype(static_string_test_1)).name() << std::endl;
    std::string static_string_1(static_string_test_1.data);
    EXPECT_EQ(dynamic_string_1, static_string_1);

    std::string dynamic_string_2("TestStaticString,TestStaticString,TestStaticString");
    auto static_string_test_2 = DUSK_MAKE_STATIC_STRING("TestStaticString,TestStaticString,TestStaticString");
    std::cout << typeid(decltype(static_string_test_2)).name() << std::endl;
    std::string static_string_2(static_string_test_2.data);
    EXPECT_EQ(dynamic_string_2, static_string_2);
}