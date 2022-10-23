#pragma once

#include "PileAA/QuadTree.hpp"
#include "PileAA/BaseComponents.hpp"
#include "Bullet.hpp"

namespace rtype {
namespace game {

enum CollisionType {
    PLAYER,
    ENEMY,
    PLAYER_BULLET,
    ENEMY_BULLET,
    STATIC_WALL,
    POWER_UP,
    NONE
};

class CollisionFactory : public paa::QuadTree {
public:
    static constexpr int WALL_DAMAGE = 1;
    static constexpr int ENEMY_DAMAGE = 1;

    static paa::SCollisionBox make(const paa::IntRect& rect,
                                    const paa::CollisionBox::callback_t& callback,
                                    const int64_t& id,
                                    const PAA_ENTITY& entity) {
        return CollisionBox(new paa::CollisionBox(rect, callback, id, entity));
    }

    template<typename T>
    static paa::SCollisionBox makeAnyCollidable(const paa::IntRect& rect,
                                                const int64_t& id,
                                                const PAA_ENTITY& entity) {
        return make(rect,
            [](const paa::CollisionBox& self, const paa::CollisionBox& other) {
                PAA_GET_COMPONENT(self.get_entity(), T)->on_collision(other);
            }, id, entity);
    }

    static paa::SCollisionBox makePlayerCollision(const paa::IntRect& rect,
                                                  const PAA_ENTITY& e)
    { return make_any_collidable<Player>(rect, CollisionType::PLAYER, e); }

    static paa::SCollisionBox makeEnemyCollision(const paa::IntRect& rect,
                                                 const PAA_ENTITY& e)
    { return make_any_collidable<Enemy>(rect, CollisionType::ENEMY, e); }

    static CollisionBox makeBulletCollision(const paa::IntRect& rect,
                                                  const PAA_ENTITY& e,
                                                  const bool from_player)
    { return make_any_collidable<Bullet>(rect,
        from_player ? CollisionType::PLAYER_BULLET : CollisionType::ENEMY_BULLET, e); }

    static paa::SCollisionBox makePlayerBulletCollision(const paa::IntRect& rect,
                                                        const PAA_ENTITY& e)
    { return makeBulletCollision(rect, e, true); }

    static paa::SCollisionBox makeEnemyBulletCollision(const paa::IntRect& rect,
                                                       const PAA_ENTITY& e)
    { return makeBulletCollision(rect, e, false); }

    static paa::SCollisionBox makeStaticWallCollision(const paa::IntRect& rect,
                                                      const PAA_ENTITY& e)
    { return make(rect, [](const paa::CollisionBox&, const paa::CollisionBox&) {},
                CollisionType::STATIC_WALL, e); }

    static paa::SCollisionBox makePowerUpCollision(const paa::IntRect& rect,
                                                   const PAA_ENTITY& e) {
        return make(rect, [](const paa::CollisionBox& self, const paa::CollisionBox& other) {
            if (other.get_id() == CollisionType::PLAYER) {
                PAA_ECS.kill_entity(self.get_entity());
            }
        }, CollisionType::POWER_UP, e);
    }

}

}