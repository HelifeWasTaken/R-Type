#include "Enemies.hpp"
#include "Collisions.hpp"

namespace rtype {
namespace game {

    AEnemy::AEnemy(const PAA_ENTITY e, EnemyType type)
        : _type(type)
        , _e(e)
    {
    }

    bool AEnemy::is_alive() const { return true; };
    EnemyType AEnemy::get_type() const { return _type; };
    paa::Position& AEnemy::get_position() const
    {
        return PAA_GET_COMPONENT(_e, paa::Position);
    };

    void AEnemy::on_collision(const paa::CollisionBox& other)
    {
        spdlog::warn("Enemy collision");
        if (other.get_id() == rtype::game::CollisionType::PLAYER_BULLET) {
            spdlog::warn("Enemy collision with bullet");
            PAA_GET_COMPONENT(_e, paa::Health).hp -= 1;
        }
    }
}
}