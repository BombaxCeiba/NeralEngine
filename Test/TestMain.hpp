/***
 * @Author: Dusk
 * @Date: 2022-01-10 15:50:23
 * @FilePath: \NRender\Test\TestMain.hpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <gtest/gtest.h>
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}