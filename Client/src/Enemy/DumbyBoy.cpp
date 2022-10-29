#include "Enemies.hpp"
#include "ClientScenes.hpp"
#include "Collisions.hpp"

namespace rtype {
namespace game {

    DumbyBoy::DumbyBoy(const PAA_ENTITY& e) :
                        AEnemy(e, EnemyType::DUMBY_BOY_ENEMY),
                        _to_focus(g_game.get_random_player())
    {
        auto shooter = make_shooter<BasicShooter>(_e);
        _shooterList.push_back(shooter);
        _shoot_cycle = 1.5f + static_cast<float>(paa::Random::rand()) * static_cast<float>(3 - 1.5f) / RAND_MAX;
    }

    void DumbyBoy::on_collision(const paa::CollisionBox& other)
    {
        AEnemy::on_collision(other);
        if (other.get_id() == CollisionType::STATIC_WALL) {
            _y_velocity *= -1;
        }
    }

    void DumbyBoy::update()
    {
        paa::Position& posRef = get_position();
        const float deltaTime = PAA_DELTA_TIMER.getDeltaTime();
        _last_shoot += deltaTime;

        posRef.x -= 20 * deltaTime;
        posRef.y += _y_velocity * deltaTime;
        if (_last_shoot >= _shoot_cycle) {
            if (!g_game.is_player_alive_by_entity(_to_focus)) {
                _to_focus.setEntity(g_game.get_random_player());
                if (_to_focus.getId() == -1)
                    return;
            }
            spdlog::error("focus: {}", _to_focus.getId());
            paa::Sprite to_aim = _to_focus.getComponent<paa::Sprite>();
            spdlog::error("focused: {}", _to_focus.getId());
            paa::Vector2f pos = paa::Vector2f(to_aim->getPosition());

            _shooterList[0]->aim(pos);
            _shooterList[0]->shoot();
            _last_shoot = 0.0f;
        }
    }
};
};