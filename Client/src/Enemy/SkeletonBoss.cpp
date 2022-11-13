#include "Enemies.hpp"
#include <iostream>
#include "PileAA/MusicPlayer.hpp"

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
        isDeathTriggered = false;
        for (std::size_t i = 0; i < 12; i++) {
            auto shooter = make_shooter<ConeShooter>(_e);
            _shooterList.push_back(shooter);
        }
    }

    void SkeletonBossHead::on_collision(const paa::CollisionBox& other)
    {
        AEnemy::on_collision(other);
        auto& body_sprite = PAA_GET_COMPONENT(_boss_body, paa::Sprite);
        auto& head_sprite = PAA_GET_COMPONENT(_e, paa::Sprite);

        head_sprite->setColor(paa::Color::Red);
        body_sprite->setColor(paa::Color::Red);
    }

    void SkeletonBossHead::delay_shoot()
    {
        auto size = _shooterList.size();
        if (_last_shoot >= _shoot_cycle) {
            if (_shoot_delay >= .5f && _shoot_index < size) {
                _shooterList[_shoot_index++]->shoot("skeleton_bullet");
                _head_sprite->useAnimation("skeleton_boss_head_shoot_animation");
            }
            if (_shoot_index >= size) {
                _last_shoot = 0.0f;
                _shoot_index = 0;
                _shoot_delay = 0.0f;
            }
        }
    }

    void SkeletonBossHead::update()
    {
        const float deltaTime = PAA_DELTA_TIMER.getDeltaTime();
        auto& head_sprite = PAA_GET_COMPONENT(_e, paa::Sprite);

        _timer += deltaTime;

        if (!_start) {
            _head_sprite->useAnimation("skeleton_boss_head_start_animation", false);
            _start = true;
            _timer = 0.0f;
        }
        paa::Health head_health = PAA_GET_COMPONENT(_e, paa::Health);
        paa::Health& body_health = PAA_GET_COMPONENT(_boss_body, paa::Health);
        if (!isDeathTriggered && head_health.hp <= 0 && _start) {
            isDeathTriggered = true;
            PAA_ECS.kill_entity(_boss_body);
            paa::GMusicPlayer::play(MUSIC_COMBAT_IS_OVER, false);
        }
        if (_start) {
            _last_shoot += deltaTime;
            _shoot_delay += deltaTime;
            if (_timer > .7f) {
                _head_sprite->useAnimation("skeleton_boss_head_animation");
                _timer = 0.0f;
            }
            delay_shoot();
        }
        if (head_sprite->getColor() == paa::Color::Red) {
            _red_timer += deltaTime;
            if (_red_timer >= .1f) {
                auto& body = PAA_GET_COMPONENT(_boss_body, paa::Sprite);
                head_sprite->setColor(paa::Color::White);
                body->setColor(paa::Color::White);
                _red_timer = 0.0f;
            }
        }
    }
}
}

