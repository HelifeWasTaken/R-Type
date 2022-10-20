#include "Shooter.hpp"

namespace rtype {
namespace game {

    AShooter::AShooter(double reloadTime, const paa::Position& posRef)
        : _positionRef(posRef)
    {
        _timer.setTarget(reloadTime);
    }

    bool AShooter::can_shoot() const {
        return _timer.isFinished();
    }

    bool AShooter::can_shoot_and_restart() {
        if (can_shoot()) {
            _timer.restart();
            return true;
        }
        return false;
    }

    double AShooter::aim_angle() const { return _aim_angle; }

    void aim(const paa::Vector2f& to_aim)
    {
        const paa::Vector2<float> fpos = to_aim - _positionRef;
        _angle = std::atan2(fpos.y, fpos.x) * 180 / M_PI;
    }

}
}