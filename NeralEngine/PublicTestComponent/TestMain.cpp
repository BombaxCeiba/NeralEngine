/***
 * @Author: Dusk
 * @Date: 2022-02-22 12:15:38
 * @FilePath: \NRender\Test\TestMain.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include "TestMain.h"

int main(int argc, char** argv)
{
    ::testing::GTEST_FLAG(catch_exceptions) = false;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}