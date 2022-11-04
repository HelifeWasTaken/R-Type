#include "Enemies.hpp"

namespace rtype {
namespace game {

    SkeletonBoss::SkeletonBoss(const PAA_ENTITY& e) : AEnemy(e, SKELETON_BOSS)
    {
    }

    void SkeletonBoss::update()
    {
    }

    SkeletonBossHead::SkeletonBossHead(const PAA_ENTITY& e, const PAA_ENTITY& body) : AEnemy(e, SKELETON_BOSS),
                                                                                      _boss_body(body)
    {
    }

    void SkeletonBossHead::on_collision(const paa::CollisionBox& other)
    {
        AEnemy::on_collision(other);
        paa::Health head_health = PAA_GET_COMPONENT(_e, paa::Health);
        paa::Health& body_health = PAA_GET_COMPONENT(_boss_body, paa::Health);

        if (head_health.hp <= 0)
            PAA_ECS.kill_entity(_boss_body);
    }

    void SkeletonBossHead::update()
    {
    }
}
}

