#include "Bullet.hpp"
#include "Shooter.hpp"

namespace rtype {
namespace game {

    BasicBullet::BasicBullet(
        const PAA_ENTITY& e, const double& aim_angle, bool from_player)
        : ABullet(e, BulletType::BASIC_BULLET, 1500, aim_angle, 1, from_player)
        , _dir(paa::Math::angle_to_direction(aim_angle))
    {
    }

    void BasicBullet::update()
    {
        const auto dt = PAA_DELTA_TIMER.getDeltaTime();
        paa::Position& posRef = PAA_GET_COMPONENT(_e, paa::Position);

        posRef.x += _dir.x * BASIC_BULLET_SPEED * dt;
        posRef.y += _dir.y * BASIC_BULLET_SPEED * dt;
    }

    void BasicShooter::shoot()
    {
        if (can_shoot_and_restart()) {
            paa::Position pos = _parentEntity.getComponent<paa::Position>();
            paa::Sprite& sprite = _parentEntity.getComponent<paa::Sprite>();

            pos.x += (sprite->getGlobalBounds().width / 2);
            pos.y += (sprite->getGlobalBounds().height / 2);
            BulletFactory::make_basic_bullet(
                _aim_angle, pos, is_attached_to_player());
        }
    }

    void BulletFactory::make_basic_bullet(
        float aim_angle, paa::Position const& posRef, const bool& from_player)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();

        paa::Position& pos = e.attachPosition(posRef);
        paa::Sprite& sprite = e.attachSprite("basic_bullet");
        auto global_bounds = sprite->getGlobalBounds();

        sprite->setRotation(aim_angle, true);
        e.emplaceComponent<paa::SCollisionBox>(
            CollisionFactory::makeBulletCollision(
                paa::IntRect(
                    pos.x, pos.y, global_bounds.width, global_bounds.height),
                e.getEntity(), from_player));

        sprite->useAnimation("base_animation");
        auto bullet
            = make_bullet<BasicBullet>(e.getEntity(), aim_angle, from_player);
        e.insertComponent(std::move(bullet));
    }

}
}