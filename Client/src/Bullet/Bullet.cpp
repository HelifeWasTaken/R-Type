#include "Bullet.hpp"

namespace rtype {
namespace game {

    ABullet::ABullet(const PAA_ENTITY& e,
                     const BulletType type,
                     const double life_time,
                     const double aim_angle,
                     const double damage,
                     const bool from_player)
        : _type(type)
        , _aim_angle(aim_angle)
        , _damage(damage)
        , _from_player(from_player)
        , _e(e)
    {
        _timer.setTarget(life_time);
    }

    bool ABullet::is_alive() const { return !_timer.isFinished(); }
    BulletType ABullet::get_type() const { return _type; }
    paa::Position& ABullet::get_position() const { return PAA_GET_COMPONENT(_e, paa::Position); }
    double ABullet::get_aim_angle() const { return _aim_angle; }

    void ABullet::on_collision(const paa::CollisionBox& other)
    {
        if (other.get_id() == CollisionType::PLAYER_BULLET) {
            if (!_from_player) {
                PAA_ECS.kill_entity(_e);
            }
        } else if (other.get_id() == CollisionType::ENEMY_BULLET) {
            if (_from_player) {
                PAA_ECS.kill_entity(_e);
            }
        }
    }
};
}
