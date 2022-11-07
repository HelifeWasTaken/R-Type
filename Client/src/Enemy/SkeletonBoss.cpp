#include "Enemies.hpp"

namespace rtype {
namespace game {

    SkeletonBoss::SkeletonBoss(const PAA_ENTITY& e) : AEnemy(e, SKELETON_BOSS)
    {
    }

    void SkeletonBoss::update()
    {
    }

    SkeletonBossHead::SkeletonBossHead(const PAA_ENTITY& e,
        const PAA_ENTITY& body, const paa::Sprite& head_sprite) : AEnemy(e, SKELETON_BOSS),
                                                                _boss_body(body),
                                                                _head_sprite(head_sprite)
    {
    }

    void SkeletonBossHead::on_collision(const paa::CollisionBox& other)
    {
        AEnemy::on_collision(other);
        paa::Health head_health = PAA_GET_COMPONENT(_e, paa::Health);
        paa::Health& body_health = PAA_GET_COMPONENT(_boss_body, paa::Health);

        if (head_health.hp <= 0 && _start)
            PAA_ECS.kill_entity(_boss_body);
    }

    void SkeletonBossHead::update()
    {
        const float deltaTime = PAA_DELTA_TIMER.getDeltaTime();
        _timer += deltaTime;

        if (!_start) {
            _head_sprite->useAnimation("skeleton_boss_head_start_animation", false);
            _start = true;
            _timer = 0.0f;
        }
        if (_start && _timer > .7f) {
            _head_sprite->useAnimation("skeleton_boss_head_animation");
            _timer = 0.0f;
        }
    }
}
}

