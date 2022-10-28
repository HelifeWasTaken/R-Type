#include "Enemies.hpp"
#include "PileAA/Rand.hpp"
#include "Bullet.hpp"

namespace rtype {
    namespace game {

        KeyEnemy::KeyEnemy(const PAA_ENTITY& e) : AEnemy(e, EnemyType::KEY_ENEMY)
        {
            auto shooter = make_shooter<BasicShooter>(_e);
            shooter->aim(-180);
            _shooterList.push_back(shooter);
            _cycle = std::rand() % 10;
            _shoot_cycle = 0.1f + static_cast<float>(std::rand()) * static_cast<float>(2 - 0.1f) / RAND_MAX;
            _rand_ampl = .6f + static_cast<float>(std::rand()) * static_cast<float>(2 - .6f) / RAND_MAX;
        }

        void KeyEnemy::update()
        {
            paa::Position& posRef = get_position();
            const float deltaTime = PAA_DELTA_TIMER.getDeltaTime();
            _last_shoot += deltaTime;
            _cycle += _rand_ampl * deltaTime;
            float value = sin(_cycle);

            posRef.x -= 80 * deltaTime;
            posRef.y += value;
            if (_last_shoot >= _shoot_cycle) {
                _shooterList[0]->shoot();
                _last_shoot = 0.0f;
            }
        }

        PAA_ENTITY EnemyFactory::make_key_enemy(double const& x, double const& y)
        {
            paa::DynamicEntity e = PAA_NEW_ENTITY();

            auto& s = e.attachSprite("key_enemy")->setPosition(x, y)
                .useAnimation("key_animation");

            e.attachHealth(paa::Health(3));
            e.attachPosition(paa::Position(x, y));
            e.attachCollision(
                CollisionFactory::makeEnemyCollision(
                    paa::recTo<int>(s.getGlobalBounds()),
                    e.getEntity()
                )
            );
            Enemy ke = EnemyFactory::make_enemy<KeyEnemy>(e.getEntity());
            e.insertComponent(std::move(ke));
            return e.getEntity();

        }
    }
}