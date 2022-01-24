/***
 * @Author: Dusk
 * @Date: 2022-01-23 11:03:30
 * @FilePath: \NRender\NRender\Common\source\Timer.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#include "Timer.h"

namespace dusk
{
    Timer::BufferedFrequency::BufferedFrequency()
    {
        QueryPerformanceFrequency(&frequency_);
    }

    auto Timer::BufferedFrequency::GetFrequency() noexcept
        -> const LARGE_INTEGER
    {
        static BufferedFrequency instance{};
        return instance.frequency_;
    }

    LARGE_INTEGER Timer::GetCurrentTime()
    {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        return currentTime;
    }

    Timer::Timer()
    {
        duration_.QuadPart = 0;
    }

    void Timer::Start()
    {
        start_time_ = GetCurrentTime();
    }

    void Timer::Pause()
    {
        auto end_time = GetCurrentTime();
        duration_.QuadPart += end_time.QuadPart - start_time_.QuadPart;
        start_time_ = end_time;
    }

    void Timer::End()
    {
        auto end_time = GetCurrentTime();
        duration_.QuadPart += end_time.QuadPart - start_time_.QuadPart;
    }

    auto Timer::GetRawDuration()
        -> decltype(LARGE_INTEGER::QuadPart)
    {
        return duration_.QuadPart;
    }

    void TimerGuard::StartTimer()
    {
        ref_timer_.Start();
    }

    void TimerGuard::EndTimer()
    {
        ref_timer_.End();
    }

    double Timer::GetMillisecondsDuration()
    {
        return static_cast<double>(duration_.QuadPart * 1000000) / static_cast<double>(BufferedFrequency::GetFrequency().QuadPart);
    }

    TimerGuard::TimerGuard(Timer& timer) : ref_timer_{timer}
    {
        StartTimer();
    }

    TimerGuard::TimerGuard(Timer* p_timer) : ref_timer_{*p_timer}
    {
        StartTimer();
    }

    TimerGuard::~TimerGuard()
    {
        EndTimer();
    }
}
