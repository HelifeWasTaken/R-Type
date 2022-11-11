#include "Enemies.hpp"

namespace rtype {
namespace game {
    Mattis::Mattis(const PAA_ENTITY& e) : AEnemy(e, MATTIS_BOSS)
    {
        paa::DynamicEntity mouth = PAA_NEW_ENTITY();
        auto& position = PAA_GET_COMPONENT(e, paa::Position);
        auto& health = PAA_GET_COMPONENT(e, paa::Health);
        auto& s = mouth.attachSprite("mattis_boss_mouth")
                          ->setPosition(position.x, position.y)
                          .useAnimation("mattis_boss_mouth_basic");

        mouth.attachId(paa::Id(-1));
        mouth.attachHealth(health);
        mouth.attachPosition(position);
        Enemy entity = EnemyFactory::make_enemy<MattisMouth>(mouth.getEntity(), _e);
        mouth.insertComponent(std::move(entity));
        _mouth = mouth;
    }

    void Mattis::update()
    {
        const float& deltaTime = PAA_DELTA_TIMER.getDeltaTime();
        auto& current_pos = PAA_GET_COMPONENT(_e, paa::Position);
        paa::Vector2f dir = paa::Vector2f(_path[_path_index][0] - current_pos.x,
                _path[_path_index][1] - current_pos.y);

        dir.x = (int)dir.x > 0 ? 1.0f : (int)dir.x < 0 ? -1.0f : 0;
        dir.y = (int)dir.y > 0 ? 1.0f : (int)dir.y < 0 ? -1.0f : 0;
        current_pos.x += (int)dir.x * 50.0f * deltaTime;
        current_pos.y += (int)dir.y * 50.0f * deltaTime;
        if (dir.x == 0 && dir.y == 0)
            _path_index += _path_index < 9 ? 1 : -_path_index;
    }

    MattisMouth::MattisMouth(const PAA_ENTITY& e,
                            const PAA_ENTITY& body) :
                            AEnemy(e, MATTIS_BOSS),
                            _body(body),
                            _last_mouth_pos(0, 0)
    {
        float startingAngle = -230.0f;
        for (std::size_t i = 0; i < 10; i++) {
            auto shooter = make_shooter<BasicShooter>(_e, 100);
            shooter->aim(startingAngle);
            _shooterList.push_back(shooter);
            startingAngle += 14.0f;
        }
    }

    void MattisMouth::handle_shoot(float const& deltaTime,
            paa::Position const& pos)
    {
        _last_shoot += deltaTime;
        const paa::Position right_position(pos.x + 28, pos.y + 100);
        if (_last_shoot >= _shooting_speed) {
            _shooterList[_shoot_index++]
                ->shoot_from_pos("mattis_bullet", right_position);
            _last_shoot = 0.0f;
        }
        if (_shoot_index >= _shooterList.size()) {
            _shoot_index = 0;
            _last_attack = 0.0f;
            _as_attacked = false;
            _close_mouth = true;
            _start_attack = false;
        }
    }

    void MattisMouth::check_mouth_offset(paa::Position& mouth_pos,
                                        const float& deltaTime)
    {
        if (_start_attack) {
            if (_y_offset < MOUTH_OFFSET_Y && !_close_mouth) {
                _y_offset += 25.0f * deltaTime;
            } else {
                handle_shoot(deltaTime, mouth_pos);
            }
        }

        if (_close_mouth)
            _y_offset -= 25.0f * deltaTime;
        if (_y_offset <= 0.0f) {
            _y_offset = 0;
            _close_mouth = false;
        }
    }

    void MattisMouth::open_mouth(paa::Position& mouth_pos,
                                float const& deltaTime)
    {
        if (_last_attack >= _attack_cd && !_start_attack) {
            _start_attack = true;
            _last_mouth_pos = mouth_pos;
        }
        check_mouth_offset(mouth_pos, deltaTime);
    }

    void MattisMouth::update()
    {
        auto& posRef = PAA_GET_COMPONENT(_e, paa::Position);
        auto& parentPosRef = _body.getComponent<paa::Position>();
        const float& deltaTime = PAA_DELTA_TIMER.getDeltaTime();

        if (!_as_attacked)
            _last_attack += deltaTime;
        posRef.x = parentPosRef.x;
        posRef.y = parentPosRef.y + _y_offset;
        open_mouth(posRef, deltaTime);
    }
}
}
