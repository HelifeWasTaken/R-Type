#include "Shooter.hpp"
#include "Bullet.hpp"
#include "PileAA/Math.hpp"
#include "Player.hpp"

namespace rtype {
namespace game {

    AShooter::AShooter(const PAA_ENTITY& e, double reloadTime)
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
        _aim_angle = isRadian ? aim_angle : paa::Math::toRadians(aim_angle);
        return *this;
    }

    bool AShooter::angle_is_set(void)
    {
        return _aim_angle != 0;
    }

    bool AShooter::is_attached_to_player() const
    {
        return _parentEntity.hasComponent<rtype::game::Player>();
    }

    void BasicShooter::shoot(std::string const& bullet_type)
    {
        if (can_shoot_and_restart()) {
            paa::Position pos = _parentEntity.getComponent<paa::Position>();
            paa::Sprite& sprite = _parentEntity.getComponent<paa::Sprite>();

            pos.x += (sprite->getGlobalBounds().width / 2);
            pos.y += (sprite->getGlobalBounds().height / 2);
            BulletFactory::make_bullet_by_type(bullet_type, pos,
                        is_attached_to_player(), _aim_angle);
        }
    }

    ConeShooter::ConeShooter(const PAA_ENTITY& e, double reloadTime) :
                                            AShooter(e, reloadTime)
    {
        static const float angles[3] = {2.96706f, 3.14159f, 3.31613f};

        _aim_angle = 1;
        for (std::size_t i = 0; i < 3; i++) {
            auto shooter = make_shooter<BasicShooter>(e);
            shooter->aim(angles[i], true);
            _shooterList.push_back(shooter);
        }
    }

    void ConeShooter::shoot(std::string const& bullet_type)
    {
        if (can_shoot_and_restart()) {
            paa::Position pos = _parentEntity.getComponent<paa::Position>();
            paa::Sprite& sprite = _parentEntity.getComponent<paa::Sprite>();

            pos.x += (sprite->getGlobalBounds().width / 2);
            pos.y += (sprite->getGlobalBounds().height / 2);
            for (auto &shooter : _shooterList)
                shooter->shoot(bullet_type);
        }
    }
}
}
