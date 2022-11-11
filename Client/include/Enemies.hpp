#pragma once
#include "PileAA/DynamicEntity.hpp"
#include "Shooter.hpp"
#include "Bullet.hpp"

namespace rtype {
namespace game {

    enum EnemyType {
        BASIC_ENEMY,
        KEY_ENEMY,
        MASTODONTE_ENEMY,
        DUMBY_BOY_ENEMY,
        SKELETON_BOSS,
        MATTIS_BOSS,
        CENTIPEDE_BOSS,
        CENTIPEDE_BODY_BOSS
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

        virtual bool is_alive() const;

        EnemyType get_type() const;
        paa::Position& get_position() const;
        paa::Health& get_health() const;

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

    class MastodonteEnemy : public AEnemy {
    public:
        MastodonteEnemy(const PAA_ENTITY& e);
        ~MastodonteEnemy() = default;

        void update() override;
    };

    class DumbyBoy : public AEnemy {
    private:
        paa::DynamicEntity _to_focus;
        float _y_velocity = -50.0f;
        float _shoot_cycle = 0.0f;
        float _last_shoot = 0.0f;

    public:
        DumbyBoy(const PAA_ENTITY& e);
        ~DumbyBoy() = default;

        void on_collision(const paa::CollisionBox& other) override;
        void update() override;
    };

    class SkeletonBoss : public AEnemy {
    public:
        SkeletonBoss(const PAA_ENTITY& e);
        ~SkeletonBoss() = default;

        void update() override;
    };

    class SkeletonBossHead : public AEnemy {
    private:
        const PAA_ENTITY _boss_body;
        const paa::Sprite _head_sprite;
        float _timer = 0.0f;
        float _last_shoot = 0.0f;
        float _shoot_delay = .3f;
        bool _start = false;
        std::size_t _shoot_index = 0;
        const float _shoot_cycle = 2.0f;

    public:
        SkeletonBossHead(const PAA_ENTITY& e, const PAA_ENTITY& body,
                        const paa::Sprite& head_s);
        ~SkeletonBossHead() = default;

        void on_collision(const paa::CollisionBox& other) override;
        void delay_shoot();
        void update() override;
    };

    #define MOUTH_OFFSET_Y 20.0f

    class Mattis : public AEnemy {
    public:
        Mattis(const PAA_ENTITY& e);
        ~Mattis() = default;

        void update() override;
    private:
        const float _path[10][2] = {
           { 891.00f, 117.00f },
           { 1110.00f, 77.00f },
           { 1059.00f, 10.00f },
           { 860.00f, 26.00f },
           { 1005.00f, 29.00f},
           { 1082.00f, 45.00f },
           { 1039.00f, 189.00f },
           { 1152.00f, 241.00f},
           { 987.00f, 110.00f },
           { 982.00f, 140.00f}
        };
        std::size_t _path_index = 0;
        PAA_ENTITY _mouth;
    };

    class MattisMouth : public AEnemy {
    public:
        MattisMouth(
                const PAA_ENTITY& e, const PAA_ENTITY& body);
        ~MattisMouth() = default;

        void update() override;

    private:
        void open_mouth(paa::Position& mouth_pos,
                const float& deltaTime);
        void check_mouth_offset(paa::Position& mouth_pos,
                const float& deltaTime);

    private:
        float _y_offset = 0.0f;
        paa::Position _last_mouth_pos;
        const float _attack_cd = 4.0f;
        float _last_attack = 0.0f;
        const paa::DynamicEntity _body;
        bool _as_attacked = false;
        bool _start_attack = false;
        bool _close_mouth = false;
    };

    #define RTYPE_CENTIPEDE_BODY_COUNT 10

    using CentipedeBack = std::pair<paa::Position, float>;

    class CentipedeBody : public AEnemy {
    private:
        PAA_ENTITY _child;
        PAA_ENTITY _parent;
        int _depth;

        bool _alive = true;
        float _angle = 0.0f;

        paa::Timer _timer;
        CentipedeBack _lastPosition;
        CentipedeBack _target;

        paa::Sprite s;

    public:
        static PAA_ENTITY build_centipede(const PAA_ENTITY& parent,
            int depth=RTYPE_CENTIPEDE_BODY_COUNT);

    public:
        CentipedeBody(const PAA_ENTITY& self, const PAA_ENTITY& parent, int depth);

        void on_collision(const paa::CollisionBox& other);

        void kill();

        bool is_alive() const override;

        bool centipede_part_functional();

        CentipedeBack get_back();

        void update() override;
    };

    class Centipede : public AEnemy {
    protected:
        PAA_ENTITY _body_part;
        paa::Sprite s;

        float _angle = 0.0f;
        paa::Timer _timer;

        std::vector<paa::Vector2f> _path;

        CentipedeBack _lastPosition;

        int _path_index = -1;

        bool _phase_one = true;

        float determine_time_to_next_point();

    public:
        Centipede(const PAA_ENTITY& e);

        ~Centipede() = default;

        void on_collision(const paa::CollisionBox& other) override;

        bool is_alive() const override;

        CentipedeBack get_back();

        void update() override;
    };


    using Enemy = std::shared_ptr<AEnemy>;
    class EnemyFactory {
    public:
        template <typename T, typename... Args>
        static Enemy make_enemy(Args&&... args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        static PAA_ENTITY make_basic_enemy(double const& x, double const& y);

        static PAA_ENTITY make_key_enemy(double const& x, double const& y);

        static PAA_ENTITY make_mastodonte_enemy(
            double const& x, double const& y);

        static PAA_ENTITY make_dumby_boy_enemy(
            double const& x, double const& y);

        static PAA_ENTITY make_enemy_by_type(
            const std::string& enemy_type, const float x, const float y);

        static PAA_ENTITY make_skeleton_boss(
            double const& x, double const& y
        );

        static PAA_ENTITY make_mattis_boss(
                double const& x, double const& y);

        static PAA_ENTITY make_centipede_boss(
            double const& x, double const& y
        );
    };
}
}
