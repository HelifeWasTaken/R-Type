#pragma once

#include "Types.hpp"

namespace paa {
using TimeUnit = f64;

class Timer : public Clock {
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
    bool isFinished() const;

    /**
     * @brief Set the Target object
     *
     * @param target
     * @return Timer&
     */
    Timer& setTarget(const TimeUnit& target);

    /**
     * @brief Get the percentage of the target reached by the clock
     * @param capped If true, the percentage will be capped at 1.0
     * @return double
     */
    double getPercentage(bool capped=true) const
    {
        double f = getElapsedTime().asMilliseconds() / _targetTime;

        return capped ? std::min(f, 1.0) : f;
    }

};

class DeltaTimer : public Clock {
private:
    TimeUnit _deltaTime = 0.0;
    TimeUnit _lastTime = 0.0;

public:
    /**
     * @brief Construct a new Delta Timer object
     */
    DeltaTimer() = default;

    /**
     * @brief Destroy the Delta Timer object
     */
    ~DeltaTimer() = default;

    /**
     * @brief Get the Delta Time object
     *
     * @return TimeUnit
     */
    TimeUnit getDeltaTime() const;

    /**
     * @brief Update the Delta Time object
     */
    void update();
};

} // namespace paa
