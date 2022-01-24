/***
 * @Author: Dusk
 * @Date: 2022-01-23 10:40:08
 * @FilePath: \NRender\NRender\Common\include\Timer.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <chrono>
#include <profileapi.h> //win32 only

namespace dusk
{
    //使用QueryPerformanceCounter实现
    class Timer
    {
    private:
        class BufferedFrequency
        {
        private:
            LARGE_INTEGER frequency_;
            BufferedFrequency();
            ~BufferedFrequency() = default;

        public:
            static auto GetFrequency() noexcept
                -> const LARGE_INTEGER;
        };

        LARGE_INTEGER start_time_;
        LARGE_INTEGER duration_;

        inline static LARGE_INTEGER GetCurrentTime();

    public:
        Timer();
        void Start();
        void Pause();
        void Continue();
        void End();
        auto GetRawDuration()
            -> decltype(LARGE_INTEGER::QuadPart);
        double GetMillisecondsDuration();
    };

    class TimerGuard
    {
    private:
        Timer& ref_timer_;

        void StartTimer();
        void EndTimer();

    public:
        TimerGuard(Timer& timer);
        TimerGuard(Timer* p_timer);
        TimerGuard(TimerGuard&) = delete;
        TimerGuard& operator=(TimerGuard&) = delete;
        ~TimerGuard();
    };
}
