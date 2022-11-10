#include "Bullet.hpp"

namespace rtype {
namespace game {

    ABullet::ABullet(const PAA_ENTITY& e, const BulletType type,
        const double life_time, const double aim_angle, const double damage,
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
    void ABullet::force_kill() const { PAA_ECS.kill_entity(_e); };
    paa::Position& ABullet::get_position() const
    {
        return PAA_GET_COMPONENT(_e, paa::Position);
    }
    double ABullet::get_aim_angle() const { return _aim_angle; }

    void ABullet::on_collision(const paa::CollisionBox& other)
    {
        const int other_type = other.get_id();
        const bool should_kill
            = (other_type == CollisionType::PLAYER && !_from_player)
            | (other_type == CollisionType::ENEMY && _from_player)
            | (other_type == CollisionType::STATIC_WALL)
            | (other_type == CollisionType::TRANSPARENT_WALL && _from_player);

        if (should_kill) {
            PAA_ECS.kill_entity(_e);
            paa::DynamicEntity e = PAA_NEW_ENTITY();
            e.emplaceComponent<BulletExplosion>(e);
            e.attachPosition(get_position());
            spdlog::info("Bullet explosion!");
        }
    }

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

    SkeletonBullet::SkeletonBullet(
        const PAA_ENTITY& e, const double& aim, bool from_player) :
        ABullet(e, BulletType::SKELETON_BULLET, 1500, aim, 1, from_player),
        _dir(paa::Math::angle_to_direction(aim))
    {
    }

    void SkeletonBullet::update()
    {
        const auto dt = PAA_DELTA_TIMER.getDeltaTime();
        paa::Position& posRef = PAA_GET_COMPONENT(_e, paa::Position);

        posRef.x += _dir.x * SKELETON_BULLET_SPEED * dt;
        posRef.y += _dir.y * SKELETON_BULLET_SPEED * dt;
    }

    MissileBullet::MissileBullet(
        const PAA_ENTITY& e, const double& aim, bool from_player) :
        ABullet(e, BulletType::MISSILE_BULLET, 2000, aim, 1, from_player),
        _dir(paa::Math::angle_to_direction(aim))
    {
    }

    void MissileBullet::update()
    {
        const auto dt = PAA_DELTA_TIMER.getDeltaTime();
        paa::Position& posRef = PAA_GET_COMPONENT(_e, paa::Position);

        posRef.x -= _dir.x * MISSILE_BULLET_SPEED * dt;
        posRef.y -= _dir.y * MISSILE_BULLET_SPEED * dt;
    }

    void BulletFactory::make_bullet_by_type(
            const std::string& bullet_type, paa::Position const& ref,
            const bool& from_player, float aim)
    {
        for (auto &iterator : _matching_type) {
            if (iterator.first.compare(bullet_type) == 0)
                iterator.second(aim, ref, from_player);
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

    void BulletFactory::make_skeleton_bullet(
        float aim_angle, paa::Position const& posRef, const bool& from_player)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();

        paa::Sprite& sprite = e.attachSprite("skeleton_bullet");
        auto global_bounds = sprite->getGlobalBounds();
        paa::Position& pos = e.attachPosition(posRef);

        sprite->setRotation(aim_angle, true);
        e.emplaceComponent<paa::SCollisionBox>(
            CollisionFactory::makeBulletCollision(
                paa::IntRect(
                    pos.x, pos.y, global_bounds.width, global_bounds.height),
                e.getEntity(), from_player));

        sprite->useAnimation("skeleton_bullet_animation");
        auto bullet
            = make_bullet<SkeletonBullet>(e.getEntity(), aim_angle, from_player);
        e.insertComponent(std::move(bullet));
    }

    void BulletFactory::make_missile_bullet(
        float aim_angle, paa::Position const& posRef, const bool& from_player)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();
        paa::Sprite& sprite = e.attachSprite("robot_boss");
        auto global_bounds = sprite->getGlobalBounds();
        paa::Position& pos = e.attachPosition(posRef);

        sprite->setRotation(aim_angle, true);
        e.emplaceComponent<paa::SCollisionBox>(
            CollisionFactory::makeBulletCollision(
                paa::IntRect(
                    pos.x, pos.y, global_bounds.width, global_bounds.height),
                e.getEntity(), from_player));

        sprite->useAnimation("missile");
        auto bullet
            = make_bullet<MissileBullet>(e.getEntity(), aim_angle, from_player);
        e.insertComponent(std::move(bullet));
    }
};
}
