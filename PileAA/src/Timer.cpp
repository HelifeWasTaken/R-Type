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

void DeltaTimer::update()
{
    sf::Time t = restart();

    _deltaTime = t.asSeconds();
}

}
