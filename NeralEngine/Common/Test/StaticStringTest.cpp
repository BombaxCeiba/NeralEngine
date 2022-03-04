/**
 * @author: Dusk
 * @date: 2022-01-10 15:52:44
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:09:18
 * @copyright Copyright (c) 2022 Dusk.
 */
#include <string>
#include <iostream>
#include "TestMain.h"
#include "../include/StaticString.hpp"

NERAL_ENGINE_TEST_NAMESPACE_START
TEST(StaticString, StaticStringContentTest)
{
    std::cout << "__cplusplus: " << __cplusplus << std::endl;
    std::string dynamic_string_1("TestStaticString");
    auto static_string_test_1 = NERAL_MAKE_STATIC_STRING("TestStaticString");
    std::cout << typeid(decltype(static_string_test_1)).name() << std::endl;
    std::string static_string_1(static_string_test_1.data);
    EXPECT_EQ(dynamic_string_1, static_string_1);

    std::string dynamic_string_2("TestStaticString,TestStaticString,TestStaticString");
    auto static_string_test_2 = NERAL_MAKE_STATIC_STRING("TestStaticString,TestStaticString,TestStaticString");
    std::cout << typeid(decltype(static_string_test_2)).name() << std::endl;
    std::string static_string_2(static_string_test_2.data);
    EXPECT_EQ(dynamic_string_2, static_string_2);
}

TEST(StaticString, StaticStringEqualityTest)
{
    auto static_string_test_1 = NERAL_MAKE_STATIC_STRING("TestStaticString");
    auto static_string_test_2 = NERAL_MAKE_STATIC_STRING("TestStaticString");
    EXPECT_EQ(static_string_test_1, static_string_test_2);
}

TEST(StaticString, StaticStringInequalityTest)
{
    auto static_string_test_1 = NERAL_MAKE_STATIC_STRING("TestStaticString");
    auto static_string_test_2 = NERAL_MAKE_STATIC_STRING("InequalityTestStaticString");
    EXPECT_NE(static_string_test_1, static_string_test_2);
}
NERAL_ENGINE_TEST_NAMESPACE_END