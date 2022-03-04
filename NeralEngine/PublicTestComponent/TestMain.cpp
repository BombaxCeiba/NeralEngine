/**
 * @author: Dusk
 * @date: 2022-02-22 12:15:38
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:21:50
 * @copyright Copyright (c) 2022 Dusk.
 */
#include "TestMain.h"

int main(int argc, char** argv)
{
    ::testing::GTEST_FLAG(catch_exceptions) = false;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}