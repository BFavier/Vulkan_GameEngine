#include <GameEngine/user_interface/Timer.hpp>
using namespace GameEngine;

Timer::Timer()
{
    restart();
}

Timer::~Timer()
{
}

double Timer::dt(bool reset)
{
    std::chrono::time_point<std::chrono::steady_clock> now;
    now = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = now - _last;
    if (reset)
    {
        _last = now;
    }
    return duration.count();
}

double Timer::t() const
{
    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - _start;
    return duration.count();
}

void Timer::restart()
{
    _start = std::chrono::steady_clock::now();
    _last = _start;
}
