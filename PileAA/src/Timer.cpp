#include "PileAA/Timer.hpp"

namespace paa {

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

TimeUnit DeltaTimer::getDeltaTime() const { return _deltaTime; }

void DeltaTimer::setFpsTarget(const unsigned int& fpsTarget)
{
    _fpsTarget = 1000.f / static_cast<float>(fpsTarget);
}

unsigned int DeltaTimer::getFpsTarget() const { return _fpsTarget; }

void DeltaTimer::update()
{
    const TimeUnit currentTime = getElapsedTime().asMilliseconds();
    _deltaTime = _fpsTarget * (currentTime - _lastTime);
    _lastTime = currentTime;
}

}