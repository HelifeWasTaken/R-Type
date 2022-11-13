#include "Enemies.hpp"
#include <fstream>

namespace rtype {
namespace game {

    static inline bool shoot = false;

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
        for (std::size_t i = 0; i < 2; i++) {
            auto shooter = make_shooter<BasicShooter>(_e, 50);
            shooter->aim(-180);
            _shooterList.push_back(shooter);
        }
        load_path();
    }

    void Mattis::load_path()
    {
        std::ifstream file("../assets/maps/Space/mattis_path.json");
        nlohmann::json j;

        file >> j;

        for (auto& p : j["path"]) {
            _path.push_back({p[0].get<float>(), p[1].get<float>()});
        }
    }

    void Mattis::shoot_sequence(const float& deltaTime, const paa::Position& pos)
    {
        if (shoot) {
            for (std::size_t i = 0; i < _shooterList.size(); i++) {
                auto fixed_pos = paa::Position(pos.x + _eye_offset[i][0],
                        pos.y + _eye_offset[i][1]);
                _shooterList[i]->shoot_from_pos("laser_beam", fixed_pos);
            }
            _current_shoot_duration += deltaTime;
            if (_current_shoot_duration >= _shoot_duration) {
                shoot = false;
                _current_shoot_duration = 0.0f;
            }
        }
    }

    void Mattis::on_collision(const paa::CollisionBox& other)
    {

        AEnemy::on_collision(other);
        auto& health = PAA_GET_COMPONENT(_e, paa::Health);
        auto& sprite = PAA_GET_COMPONENT(_e, paa::Sprite);
        auto& mouth_s = PAA_GET_COMPONENT(_mouth, paa::Sprite);

        if (other.get_id() == rtype::game::CollisionType::PLAYER_BULLET) {
            sprite->setColor(paa::Color::Red);
            if (health.hp >= 10)
                mouth_s->setColor(paa::Color::Red);
            else
                mouth_s->setColor(paa::Color::Transparent);
        }
        if (health.hp <= 0) {
            PAA_ECS.kill_entity(_mouth);
        }
    }

    void Mattis::update_sprite(const float& deltaTime, const paa::Sprite& sprite)
    {
        auto& mouth_s = PAA_GET_COMPONENT(_mouth, paa::Sprite);
        _red_timer += deltaTime;

        if (_red_timer >= .1f) {
           sprite->setColor(paa::Color::White);
           if (mouth_s->getColor() != paa::Color::Transparent)
               mouth_s->setColor(paa::Color::White);
           _red_timer = 0.0f;
        }
    }

    void Mattis::update()
    {
        const float& deltaTime = PAA_DELTA_TIMER.getDeltaTime() * 2;
        auto& current_pos = PAA_GET_COMPONENT(_e, paa::Position);
        paa::Vector2f dir = paa::Vector2f(_path[_path_index].x - current_pos.x,
                _path[_path_index].y - current_pos.y);
        auto& sprite = PAA_GET_COMPONENT(_e, paa::Sprite);

        if (sprite->getColor() == paa::Color::Red)
            update_sprite(deltaTime, sprite);
        dir.x = (int)dir.x > 0 ? 1.0f : (int)dir.x < 0 ? -1.0f : 0;
        dir.y = (int)dir.y > 0 ? 1.0f : (int)dir.y < 0 ? -1.0f : 0;
        current_pos.x += (int)dir.x * 50.0f * deltaTime;
        current_pos.y += (int)dir.y * 50.0f * deltaTime;
        if (dir.x == 0 && dir.y == 0)
            _path_index += _path_index < _path.size() - 1? 1 : -_path_index;
        shoot_sequence(deltaTime, current_pos);
    }

    MattisMouth::MattisMouth(const PAA_ENTITY& e,
                            const PAA_ENTITY& body) :
                            AEnemy(e, MATTIS_BOSS),
                            _body(body),
                            _last_mouth_pos(0, 0)
    {
        float startingAngle = -230.0f;
        for (std::size_t i = 0; i < 17; i++) {
            auto shooter = make_shooter<BasicShooter>(_e, 100);
            shooter->aim(startingAngle);
            _shooterList.push_back(shooter);
            startingAngle += 25.0f;
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
            shoot = true;
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
        auto& sprite = PAA_GET_COMPONENT(_e, paa::Sprite);
        auto& posRef = PAA_GET_COMPONENT(_e, paa::Position);
        auto& parentPosRef = _body.getComponent<paa::Position>();
        const float& deltaTime = PAA_DELTA_TIMER.getDeltaTime() * 2;

        if (sprite->getColor() == paa::Color::Transparent) {
            shoot = true;
            return;
        }
        if (!_as_attacked)
            _last_attack += deltaTime;
        posRef.x = parentPosRef.x;
        posRef.y = parentPosRef.y + _y_offset;
        open_mouth(posRef, deltaTime);
    }
}
}
