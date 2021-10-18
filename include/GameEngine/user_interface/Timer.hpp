#pragma once
#include <chrono>

namespace GameEngine
{
    class Timer
    {
    public:
        Timer();
        ~Timer();
        //!< Time in seconds since last call to dt with reset=true (or since creation time)
        double dt(bool reset=true);
        //!< Time in seconds since creation of the timer.
        double t() const;
        //!< Restarts the timer as if it was just created.
        void restart();
    private:
        std::chrono::time_point<std::chrono::steady_clock> _start;
        std::chrono::time_point<std::chrono::steady_clock> _last;
    };
}
