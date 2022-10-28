#include "Enemies.hpp"
#include "PileAA/Rand.hpp"
#include "Bullet.hpp"

namespace rtype {
    namespace game {

        BasicEnemy::BasicEnemy(const PAA_ENTITY& e) : AEnemy(e, EnemyType::BASIC_ENEMY)
        {
            auto shooter = make_shooter<BasicShooter>(_e);
            shooter->aim(-180);
            _shooterList.push_back(shooter);
            _cycle = paa::Random::rand() % 10;
            _shoot_cycle = 0.1f + static_cast<float>(paa::Random::rand()) * static_cast<float>(2 - 0.1f) / RAND_MAX;
            _rand_ampl = 1 + static_cast<float>(paa::Random::rand()) * static_cast<float>(2 - 1) / RAND_MAX;
        }

        void BasicEnemy::update()
        {
            paa::Position& posRef = get_position();
            const float deltaTime = PAA_DELTA_TIMER.getDeltaTime();
            _last_shoot += deltaTime;
            _cycle += _rand_ampl * deltaTime;
            float value = sin(_cycle);

            posRef.x -= 60 * deltaTime;
            posRef.y += value;
            if (_last_shoot >= _shoot_cycle) {
                _shooterList[0]->shoot();
                _last_shoot = 0.0f;
            }
        }

        PAA_ENTITY EnemyFactory::make_basic_enemy(double const& x, double const& y)
        {
            paa::DynamicEntity e = PAA_NEW_ENTITY();
            auto& s = e.attachSprite("basic_enemy");
            s->setPosition(x, y);
            s->useAnimation("base_animation");

            e.attachHealth(paa::Health(5));
            e.attachPosition(paa::Position(x, y));
            e.attachCollision(
                CollisionFactory::makeEnemyCollision(
                    paa::recTo<int>(s->getGlobalBounds()),
                    e.getEntity()
                )
            );

            Enemy be = EnemyFactory::make_enemy<BasicEnemy>(e.getEntity());
            e.insertComponent(std::move(be));
            return e.getEntity();
        }
    }
}
