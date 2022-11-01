#pragma once

#define RTYPE_TRANSITION_DURATION 3000

#include "PileAA/Types.hpp"
#include "PileAA/Timer.hpp"
#include "utils.hpp"

class BlackScreenTransition {
private:
    sf::RectangleShape _rect;
    paa::Timer _timer;
    bool _phase_two = true;

public:
    BlackScreenTransition()
    {
        _rect.setPosition(0, 0);

        _rect.setFillColor(sf::Color(0x00, 0x00, 0x00, 0x00));
        _timer.setTarget(0);
    }

    bool phase_two() const
    {
        return _phase_two;
    }

    void update()
    {
        float elapsed = _timer.getElapsedTime().asMilliseconds();
        float alpha = (elapsed / RTYPE_TRANSITION_DURATION) * 0xFF;

        if (phase_two()) {
            _rect.setFillColor(sf::Color(0x00, 0x00, 0x00, 0xFF - alpha));
        } else {
            _rect.setFillColor(sf::Color(0x00, 0x00, 0x00, alpha));
            if (_timer.isFinished()) {
                _timer.restart();
                _phase_two = true;
            }
        }
    }

    void draw() const
    {
        if (!finished())
            PAA_SCREEN.draw(_rect);
    }

    bool finished() const
    {
        return _timer.isFinished() && _phase_two;
    }

    void start()
    {
        _rect.setSize(paa::vecTo<float>(PAA_SCREEN.getSize()));
        _timer.setTarget(RTYPE_TRANSITION_DURATION / 2);
        _phase_two = false;
    }
};
