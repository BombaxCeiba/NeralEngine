/***
 * @Author: Dusk
 * @Date: 2022-01-10 15:52:44
 * @FilePath: \NRender\Test\StaticStringTest.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include <string>
#include "TestMain.hpp"
#include "StaticString.hpp"

TEST(StaticString, StaticStringTest)
{
    std::string dynamic_string("TestStaticString");
    auto static_string_test = DUSK_MAKE_STATIC_STRING("TestStaticString");
    std::string static_string(static_string_test.data);
    EXPECT_EQ(dynamic_string, static_string);
}