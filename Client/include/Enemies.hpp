#pragma once
#include "PileAA/DynamicEntity.hpp"
#include "Shooter.hpp"

namespace rtype {
namespace game {

    enum EnemyType
    {
        BASIC_ENEMY,
        KEY_ENEMY
    };

    class AEnemy {
    protected:
        EnemyType _type;
        ShooterList _shooterList;
        const PAA_ENTITY _e;
        bool is_hurt = false;

    public:
        AEnemy(const PAA_ENTITY e, EnemyType type) : _type(type),
                                                     _e(e)
        {
        }

        virtual ~AEnemy() = default;

        bool is_alive() const { return true; };
        EnemyType get_type() const { return _type; };
        paa::Position& get_position() const { return PAA_GET_COMPONENT(_e, paa::Position); };

        virtual void update() = 0;
        virtual void on_collision(const paa::CollisionBox& other) = 0;
    };

    class BasicEnemy : public AEnemy {
    private:
        float _cycle = 0.0f;
        float _shoot_cycle = 0.0f;
        float _last_shoot = 0.0f;
        float _rand_ampl = 0.0f;

    public:
        BasicEnemy(const PAA_ENTITY& e) : AEnemy(e, EnemyType::BASIC_ENEMY)
        {
            auto shooter = make_shooter<BasicShooter>(_e);
            shooter->aim(-180);
            _shooterList.push_back(shooter);
            _cycle = std::rand() % 10;
            _shoot_cycle = 0.1f + static_cast<float>(std::rand()) * static_cast<float>(2 - 0.1f) / RAND_MAX;
            _rand_ampl = 1 + static_cast<float>(std::rand()) * static_cast<float>(2 - 1) / RAND_MAX;
        }

        ~BasicEnemy() = default;

        void on_collision(const paa::CollisionBox& other) override {};

        void update() override
        {
            paa::Position& posRef = get_position();
            const float deltaTime = PAA_DELTA_TIMER.getDeltaTime();
            _last_shoot += deltaTime;
            _cycle += _rand_ampl * deltaTime;
            float value = sin(_cycle);

            posRef.x -= 120 * deltaTime;
            posRef.y += value;
            if (_last_shoot >= _shoot_cycle) {
                _shooterList[0]->shoot();
                _last_shoot = 0.0f;
            }
        }
    };

    class KeyEnemy : public AEnemy {
    private:
        float _cycle = 0.0f;
        float _shoot_cycle = 0.0f;
        float _last_shoot = 0.0f;
        float _rand_ampl = 0.0f;

    public:
        KeyEnemy(const PAA_ENTITY& e) : AEnemy(e, EnemyType::KEY_ENEMY)
        {
            auto shooter = make_shooter<BasicShooter>(_e);
            shooter->aim(-180);
            _shooterList.push_back(shooter);
            _cycle = std::rand() % 10;
            _shoot_cycle = 0.1f + static_cast<float>(std::rand()) * static_cast<float>(2 - 0.1f) / RAND_MAX;
            _rand_ampl = .6f + static_cast<float>(std::rand()) * static_cast<float>(2 - .6f) / RAND_MAX;
        }

        ~KeyEnemy() = default;

        void on_collision(const paa::CollisionBox& other) override {};

        void update() override
        {
            paa::Position& posRef = get_position();
            const float deltaTime = PAA_DELTA_TIMER.getDeltaTime();
            _last_shoot += deltaTime;
            _cycle += _rand_ampl * deltaTime;
            float value = sin(_cycle);

            posRef.x -= 120 * deltaTime;
            posRef.y += value;
            if (_last_shoot >= _shoot_cycle) {
                _shooterList[0]->shoot();
                _last_shoot = 0.0f;
            }
        }
    };

    using Enemy = std::shared_ptr<AEnemy>;
    class EnemyFactory {
    public:
        template <typename T, typename... Args>
        static Enemy make_enemy(Args&&... args)
        {
            return std::make_shared<T>(std::forward<Args>(args) ...);
        }

        static PAA_ENTITY make_basic_enemy(double const& x, double const& y)
        {
            paa::DynamicEntity e = PAA_NEW_ENTITY();
            e.attachSprite("basic_enemy")->useAnimation("base_animation");
            e.attachHealth(paa::Health(5));
            e.attachPosition(paa::Position(x, y));
            Enemy be = EnemyFactory::make_enemy<BasicEnemy>(e.getEntity());
            e.insertComponent(std::move(be));
            return e.getEntity();
        }

        static PAA_ENTITY make_key_enemy(double const& x, double const& y)
        {
            paa::DynamicEntity e = PAA_NEW_ENTITY();

            e.attachSprite("key_enemy")->useAnimation("key_animation");
            e.attachHealth(paa::Health(3));
            e.attachPosition(paa::Position(x, y));
            Enemy ke = EnemyFactory::make_enemy<KeyEnemy>(e.getEntity());
            e.insertComponent(std::move(ke));
            return e.getEntity();
        }
    };
}
}