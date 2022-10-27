#include "Shooter.hpp"
#include "Bullet.hpp"
#include "PileAA/Math.hpp"

namespace rtype {
namespace game {

    AShooter::AShooter(const PAA_ENTITY& e,
                       double reloadTime)
        : _parentEntity(e)
    {
        _timer.setTarget(reloadTime);
    }

    bool AShooter::can_shoot() const { return _timer.isFinished(); }

    bool AShooter::can_shoot_and_restart()
    {
        if (can_shoot()) {
            _timer.restart();
            return true;
        }
        return false;
    }

    double AShooter::aim_angle() const { return _aim_angle; }

    AShooter& AShooter::aim(const paa::Vector2f& to_aim)
    {
        _aim_angle = paa::Math::direction_to_angle(
            _parentEntity.getComponent<paa::Position>(), to_aim);
        return *this;
    }

    AShooter& AShooter::aim(float const& aim_angle, bool isRadian)
    {
        if (!isRadian)
            _aim_angle = aim_angle * (M_PI / 180);
        else
            _aim_angle = aim_angle;
        return *this;
    }

    void BasicShooter::shoot()
    {
        if (can_shoot_and_restart()) {
            paa::Position pos = _parentEntity.getComponent<paa::Position>();
            paa::Sprite& sprite = _parentEntity.getComponent<paa::Sprite>();

            pos.x += (sprite->getGlobalBounds().width / 2);
            pos.y += (sprite->getGlobalBounds().height / 2);
            BulletFatory::make_basic_bullet(_aim_angle, pos);
        }
    }

}
}
