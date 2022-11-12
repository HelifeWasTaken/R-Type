#pragma once

#define RTYPE_SHORT_TRANSITION_DURATION 3000
#define RTYPE_LONG_TRANSITION_DURATION 16000
#define RTYPE_MUSIC_STAGE_CLEAR "../assets/stage_clear.ogg"

#include "PileAA/Types.hpp"
#include "PileAA/Timer.hpp"
#include "PileAA/MusicPlayer.hpp"
#include "utils.hpp"

class BlackScreenTransition {
private:
    sf::RectangleShape _rect;
    paa::Timer _timer;
    bool _phase_two = true;
    float _duration = 0.f;

public:
    BlackScreenTransition()
    {
        _rect.setPosition(0, 0);

        _rect.setFillColor(sf::Color(0x00, 0x00, 0x00, 0x00));
        _timer.setTarget(_duration);
    }

    bool phase_two() const
    {
        return _phase_two;
    }

    void update()
    {
        float elapsed = _timer.getElapsedTime().asMilliseconds();
        float alpha = (elapsed / _duration) * 0xFF;

        if (phase_two()) {
            _rect.setFillColor(sf::Color(0x00, 0x00, 0x00, 0xFF - alpha));
        } else {
            _rect.setFillColor(sf::Color(0x00, 0x00, 0x00, alpha));
            if (_timer.isFinished()) {
                _duration = RTYPE_SHORT_TRANSITION_DURATION;
                _timer.setTarget(RTYPE_SHORT_TRANSITION_DURATION / 2);
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

    void start(bool long_transition)
    {
        _rect.setSize(paa::vecTo<float>(PAA_SCREEN.getSize()));
        _duration = long_transition ? RTYPE_LONG_TRANSITION_DURATION : RTYPE_SHORT_TRANSITION_DURATION;
        if (long_transition) {
            paa::GMusicPlayer::play(RTYPE_MUSIC_STAGE_CLEAR, false);
        }
        _timer.setTarget(_duration / 2);
        _phase_two = false;
    }

    void start_halfway() {
        start(false);
        _duration = RTYPE_SHORT_TRANSITION_DURATION;
        _timer.setTarget(RTYPE_SHORT_TRANSITION_DURATION / 2);
        _phase_two = true;
    }
};
