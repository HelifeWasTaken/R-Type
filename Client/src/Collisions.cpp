#include "Collisions.hpp"
#include "Bullet.hpp"
#include "Enemies.hpp"
#include "Player.hpp"

namespace rtype {
namespace game {

    template <typename T>
    paa::SCollisionBox make_any_collidable(
        const paa::IntRect& rect, const int64_t& id, const PAA_ENTITY& entity)
    {
        return CollisionFactory::make(
            rect,
            [](const paa::CollisionBox& self, const paa::CollisionBox& other) {
                PAA_GET_COMPONENT(self.get_entity(), T)->on_collision(other);
            },
            id, entity);
    }

    paa::SCollisionBox CollisionFactory::makePlayerCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e)
    {
        return make_any_collidable<Player>(rect, CollisionType::PLAYER, e);
    }

    paa::SCollisionBox CollisionFactory::makeEnemyCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e)
    {
        return make_any_collidable<Enemy>(rect, CollisionType::ENEMY, e);
    }

    paa::SCollisionBox CollisionFactory::makeBulletCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e, const bool from_player)
    {
        return make_any_collidable<Bullet>(rect,
            from_player ? CollisionType::PLAYER_BULLET
                        : CollisionType::ENEMY_BULLET,
            e);
    }

    paa::SCollisionBox CollisionFactory::makePlayerBulletCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e)
    {
        return makeBulletCollision(rect, e, true);
    }

    paa::SCollisionBox CollisionFactory::makeEnemyBulletCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e)
    {
        return makeBulletCollision(rect, e, false);
    }

    paa::SCollisionBox CollisionFactory::makeStaticWallCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e)
    {
        return make(
            rect,
            [](const paa::CollisionBox& self, const paa::CollisionBox& other) {
            },
            CollisionType::STATIC_WALL, e);
    }

    paa::SCollisionBox CollisionFactory::makePowerUpCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e)
    {
        return make(
            rect,
            [](const paa::CollisionBox& self, const paa::CollisionBox& other) {
                if (other.get_id() == CollisionType::PLAYER) {
                    PAA_ECS.kill_entity(self.get_entity());
                }
            },
            CollisionType::POWER_UP, e);
    }
}
}
