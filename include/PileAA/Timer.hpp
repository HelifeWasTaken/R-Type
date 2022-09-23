#include <SFML/Graphics.hpp>
#include "Types.hpp"

namespace paa {
class Timer : public sf::Clock {
public:
    using TimeUnit = f64;

protected:
    TimeUnit _targetTime = 0.0;

public:
    /**
     * @brief Construct a new Timer object
     */
    Timer() = default;

    /**
     * @brief Destroy the Timer object
     */
    ~Timer() = default;

    /**
     * @brief Checks whether the target has been reached by the clock
     */
    bool isFinished() const
    {
        return getElapsedTime().asMilliseconds() >= _targetTime;
    }

    /**
     * @brief Set the Target object
     *
     * @param target
     * @return Timer&
     */
    Timer& setTarget(const TimeUnit& target)
    {
        _targetTime = target;
        restart();
        return *this;
    }
};
}