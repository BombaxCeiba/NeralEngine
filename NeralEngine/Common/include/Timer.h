/**
 * @author: Dusk
 * @date: 2022-01-23 10:40:08
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:14:30
 * @copyright Copyright (c) 2022 Dusk.
 */
#ifndef NERAL_ENGINE_COMMON_TIMER_H
#define NERAL_ENGINE_COMMON_TIMER_H

#include <Windows.h> //win32 only

namespace Neral
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
        double GetSecondsDuration();
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
#endif // NERAL_ENGINE_COMMON_TIMER_H
