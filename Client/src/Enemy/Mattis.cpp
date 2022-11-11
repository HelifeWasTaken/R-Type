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
        current_pos.x += (int)dir.x * 100.0f * deltaTime;
        current_pos.y += (int)dir.y * 100.0f * deltaTime;
        if (dir.x == 0 && dir.y == 0)
            _path_index += _path_index < 9 ? 1 : -_path_index;
    }

    MattisMouth::MattisMouth(const PAA_ENTITY& e,
                            const PAA_ENTITY& body) :
                            AEnemy(e, MATTIS_BOSS),
                            _body(body),
                            _last_mouth_pos(0, 0)
    {
    }

    void MattisMouth::check_mouth_offset(paa::Position& mouth_pos,
                                        const float& deltaTime)
    {
        if (_start_attack) {
            if (mouth_pos.y <
                    _last_mouth_pos.y + MOUTH_OFFSET_Y && !_close_mouth) {
                _y_offset += 50.0f * deltaTime;
            } else {
                _close_mouth = true;
                if (_y_offset <= 0) {
                    _y_offset = 0.0f;
                    _start_attack = false;
                    _last_attack = 0.0f;
                    _as_attacked = false;
                }
                _y_offset -= 50.0f * deltaTime;
            }
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
        open_mouth(posRef, deltaTime);
        posRef.x = parentPosRef.x;
        posRef.y = parentPosRef.y + _y_offset;
    }
}
}
