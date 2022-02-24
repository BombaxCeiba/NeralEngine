/***
 * @Author: Dusk
 * @Date: 2022-02-23 11:15:02
 * @FilePath: \NRender\NRender\Test\TimerTest.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include <thread>
#include <iostream>
#include "TestMain.h"
#include "Timer.h"

namespace
{
    void ExpectRange(double value_to_check, double expect_value, double percentage_error)
    {
        EXPECT_GE(value_to_check, expect_value - (expect_value * percentage_error));
        EXPECT_LT(value_to_check, expect_value + (expect_value * percentage_error));
    }
    double MAX_PERCENTAGE_ERROR = 0.2;
}

TEST(Timer, TimerBasicTest)
{
    dusk::Timer timer{};
    timer.Start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    timer.End();

    auto result = timer.GetMillisecondsDuration();
    std::cout << "The thread sleep for about " << result << "ms" << std::endl;
    ExpectRange(result, 1e3, MAX_PERCENTAGE_ERROR);

    result = timer.GetSecondsDuration();
    std::cout << "The thread sleep for about " << result << "s" << std::endl;
    ExpectRange(result, 1, MAX_PERCENTAGE_ERROR);
}

TEST(Timer, TimerGuardTest)
{
    dusk::Timer timer{};
    {
        dusk::TimerGuard guard{timer};
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto result = timer.GetMillisecondsDuration();
    std::cout << "The thread sleep for about " << result << "ms" << std::endl;
    ExpectRange(result, 100, MAX_PERCENTAGE_ERROR);
}

TEST(Timer, TimerFunctionTest)
{
    dusk::Timer timer{};

    timer.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(17));
    timer.Pause();
    auto result = timer.GetMillisecondsDuration();
    auto first_result = result;
    std::cout << "The thread sleep for 17ms in code" << std::endl;
    std::cout << "The timer report that thread sleep for about " << result << "ms" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "The thread sleep for 100ms in code but timer pause" << std::endl;

    timer.Continue();
    std::this_thread::sleep_for(std::chrono::milliseconds(17));
    timer.Pause();
    result = timer.GetMillisecondsDuration();
    std::cout << "The thread sleep for 17ms again in code" << std::endl;
    std::cout << "The timer report that thread sleep for about " << result << "ms" << std::endl;

    ExpectRange(result, first_result * 2, MAX_PERCENTAGE_ERROR);
}