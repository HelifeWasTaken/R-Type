#include "PileAA/Timer.hpp"

namespace paa {

    Timer::Timer() = default;

    Timer::~Timer() = default;

    bool Timer::isFinished() const
    {
        return getElapsedTime().asMilliseconds() >= _targetTime;
    }

    Timer& Timer::setTarget(const TimeUnit& target)
    {
        _targetTime = target;
        restart();
        return *this;
    }
}
}