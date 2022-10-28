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
        AEnemy(const PAA_ENTITY e, EnemyType type);

        virtual ~AEnemy() = default;

        bool is_alive() const;
        EnemyType get_type() const;
        paa::Position& get_position() const;

        virtual void update() = 0;
        virtual void on_collision(const paa::CollisionBox& other);
    };

    class BasicEnemy : public AEnemy {
    private:
        float _cycle = 0.0f;
        float _shoot_cycle = 0.0f;
        float _last_shoot = 0.0f;
        float _rand_ampl = 0.0f;

    public:
        BasicEnemy(const PAA_ENTITY& e);
        ~BasicEnemy() = default;

        void update() override;
    };

    class KeyEnemy : public AEnemy {
    private:
        float _cycle = 0.0f;
        float _shoot_cycle = 0.0f;
        float _last_shoot = 0.0f;
        float _rand_ampl = 0.0f;

    public:
        KeyEnemy(const PAA_ENTITY& e);
        ~KeyEnemy() = default;

        void update() override;
    };

    using Enemy = std::shared_ptr<AEnemy>;
    class EnemyFactory {
    public:
        template <typename T, typename... Args>
        static Enemy make_enemy(Args&&... args)
        {
            return std::make_shared<T>(std::forward<Args>(args) ...);
        }

        static PAA_ENTITY make_basic_enemy(double const& x, double const& y);

        static PAA_ENTITY make_key_enemy(double const& x, double const& y);

        static PAA_ENTITY make_enemy_by_type(const std::string& enemy_type,
            const float x, const float y);
    };
}
}