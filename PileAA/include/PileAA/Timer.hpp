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
