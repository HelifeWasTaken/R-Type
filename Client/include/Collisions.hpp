#pragma once

#include "PileAA/BaseComponents.hpp"

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

class CollisionFactory {
public:
    static constexpr int WALL_DAMAGE = 1;
    static constexpr int ENEMY_DAMAGE = 1;

    static paa::SCollisionBox make(const paa::IntRect& rect,
        const paa::CollisionBox::callback_t& callback,
        const int64_t& id, const PAA_ENTITY& entity) {
        return paa::SCollisionBox(new paa::CollisionBox(rect, callback, id, entity));
    }

    static paa::SCollisionBox makePlayerCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e);

    static paa::SCollisionBox makeEnemyCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e);

    static paa::SCollisionBox makeBulletCollision(const paa::IntRect& rect,
        const PAA_ENTITY& e, const bool from_player);

    static paa::SCollisionBox makePlayerBulletCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e);

    static paa::SCollisionBox makeEnemyBulletCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e);

    static paa::SCollisionBox makeStaticWallCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e);

    static paa::SCollisionBox makePowerUpCollision(
        const paa::IntRect& rect, const PAA_ENTITY& e);
};

}
}