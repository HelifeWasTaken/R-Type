#include "Collisions.hpp"
#include "Enemies.hpp"

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
    }

    paa::Health& AEnemy::get_health() const
    {
        return PAA_GET_COMPONENT(_e, paa::Health);
    }

    void AEnemy::on_collision(const paa::CollisionBox& other)
    {
        if (other.get_id() == rtype::game::CollisionType::PLAYER_BULLET) {
            PAA_GET_COMPONENT(_e, paa::Health).hp -= 1;
        }
    }

    PAA_ENTITY EnemyFactory::make_key_enemy(double const& x, double const& y)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();

        auto& s = e.attachSprite("key_enemy")
                      ->setPosition(x, y)
                      .useAnimation("key_animation");

        e.attachHealth(paa::Health(3));
        e.attachPosition(paa::Position(x, y));
        e.attachCollision(CollisionFactory::makeEnemyCollision(
            paa::recTo<int>(s.getGlobalBounds()), e.getEntity()));
        Enemy ke = EnemyFactory::make_enemy<KeyEnemy>(e.getEntity());
        e.insertComponent(std::move(ke));
        return e.getEntity();
    }

    PAA_ENTITY EnemyFactory::make_mastodonte_enemy(
        double const& x, double const& y)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();

        auto& s = e.attachSprite("mastodonte_enemy")
                      ->setPosition(x, y)
                      .useAnimation("mastodonte_animation")
                      .setScale(-1, 1);
        e.attachHealth(paa::Health(6));
        e.attachPosition(paa::Position(x, y));
        e.attachCollision(CollisionFactory::makeEnemyCollision(
            paa::recTo<int>(s.getGlobalBounds()), e.getEntity()));
        Enemy masto = EnemyFactory::make_enemy<MastodonteEnemy>(e.getEntity());
        e.insertComponent(std::move(masto));
        return e.getEntity();
    }

    PAA_ENTITY EnemyFactory::make_dumby_boy_enemy(
        double const& x, double const& y)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();

        auto& s = e.attachSprite("dumby_boy_enemy")
                      ->setPosition(x, y)
                      .useAnimation("dumby_boy_enemy_animation");
        e.attachHealth(paa::Health(4));
        e.attachPosition(paa::Position(x, y));
        e.attachCollision(CollisionFactory::makeEnemyCollision(
            paa::recTo<int>(s.getGlobalBounds()), e.getEntity()));
        Enemy dmb = EnemyFactory::make_enemy<DumbyBoy>(e.getEntity());
        e.insertComponent(std::move(dmb));
        return e.getEntity();
    }

    PAA_ENTITY EnemyFactory::make_skeleton_boss(double const &x,
                                                double const &y)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();
        paa::DynamicEntity head = PAA_NEW_ENTITY();
        paa::Position head_position = paa::Position(
            x + (159.0f/*Boss sprite width*/ / 2), y + (190.0f/*Boss sprite height*/ / 2));
        auto& s = e.attachSprite("skeleton_boss")
                      ->setPosition(x, y)
                      .useAnimation("skeleton_boss_animation");
        auto& head_s = head.attachSprite("skeleton_boss_head");
        auto& head_s_animated = head_s->setPosition(
                        head_position.x, head_position.y)
                        .useAnimation("skeleton_boss_head_animation");
        e.attachHealth(paa::Health(10000));
        e.attachPosition(paa::Position(x, y));
        head.attachHealth(paa::Health(100));
        head.attachPosition(head_position);
        head.attachCollision(CollisionFactory::makeEnemyCollision(
            paa::recTo<int>(head_s_animated.getGlobalBounds()), head.getEntity()));
        Enemy skeleton = EnemyFactory::make_enemy<SkeletonBoss>(e.getEntity());
        Enemy skeletonHead = EnemyFactory::make_enemy<SkeletonBossHead>(
            head.getEntity(), e.getEntity(), head_s);
        e.insertComponent(std::move(skeleton));
        head.insertComponent(std::move(skeletonHead));
        return head.getEntity();
    }

    PAA_ENTITY EnemyFactory::make_mattis_boss(double const& x, double const& y)
    {
        paa::DynamicEntity body = PAA_NEW_ENTITY();

        auto& body_s = body.attachSprite("mattis_boss_face")
            ->setPosition(x, y)
            .useAnimation("mattis_boss_head_basic");
        body.attachHealth(paa::Health(10));
        body.attachPosition(paa::Position(x, y));
        body.attachCollision(CollisionFactory::makeEnemyCollision(
            paa::recTo<int>(body_s.getGlobalBounds()), body.getEntity()));
        Enemy body_top = EnemyFactory::make_enemy<Mattis>(body.getEntity());
        body.insertComponent(std::move(body_top));
        return body.getEntity();
    }
}
}
