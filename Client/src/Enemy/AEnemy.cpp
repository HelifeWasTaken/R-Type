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

    PAA_ENTITY EnemyFactory::make_key_enemy(double const& x, double const& y)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();

        auto& s = e.attachSprite("key_enemy")->setPosition(x, y)
            .useAnimation("key_animation");

        e.attachHealth(paa::Health(3));
        e.attachPosition(paa::Position(x, y));
        e.attachCollision(
            CollisionFactory::makeEnemyCollision(
                paa::recTo<int>(s.getGlobalBounds()),
                e.getEntity()
            )
        );
        Enemy ke = EnemyFactory::make_enemy<KeyEnemy>(e.getEntity());
        e.insertComponent(std::move(ke));
        return e.getEntity();

    }

    PAA_ENTITY EnemyFactory::make_mastodonte_enemy(double const& x, double const& y)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();

        auto& s = e.attachSprite("mastodonte_enemy")->setPosition(x, y)
            .useAnimation("mastodonte_animation").setScale(-1, 1);
        e.attachHealth(paa::Health(6));
        e.attachPosition(paa::Position(x, y));
        e.attachCollision(
            CollisionFactory::makeEnemyCollision(
                paa::recTo<int>(s.getGlobalBounds()),
                e.getEntity()
            )
        );
        Enemy masto = EnemyFactory::make_enemy<MastodonteEnemy>(e.getEntity());
        e.insertComponent(std::move(masto));
        return e.getEntity();
    }

    PAA_ENTITY EnemyFactory::make_dumby_boy_enemy(double const& x, double const& y)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();

        auto& s = e.attachSprite("dumby_boy_enemy")->setPosition(x, y)
            .useAnimation("dumby_boy_enemy_animation");
        e.attachHealth(paa::Health(4));
        e.attachPosition(paa::Position(x, y));
        e.attachCollision(
            CollisionFactory::makeEnemyCollision(
                paa::recTo<int>(s.getGlobalBounds()),
                e.getEntity()
            )
        );
        Enemy dmb = EnemyFactory::make_enemy<DumbyBoy>(e.getEntity());
        e.insertComponent(std::move(dmb));
        return e.getEntity();
    }
}
}