#pragma once

#include "Bullet.hpp"
#include "ClientScenes.hpp"
#include "Collisions.hpp"
#include "PileAA/DynamicEntity.hpp"
#include "PileAA/Math.hpp"
#include <PileAA/Types.hpp>
#include <cmath>
#include <functional>

namespace paa {
class CollisionBox;
}

#define BASIC_BULLET_SPEED 800
#define SKELETON_BULLET_SPEED 750
#define MISSILE_BULLET_SPEED 400

namespace rtype {
namespace game {

    using bullet_type_t = uint8_t;

    enum BulletType : bullet_type_t {
        BASIC_BULLET,
        SKELETON_BULLET,
        MISSILE_BULLET
    };

    // Base impl
    class ABullet {
    private:
        paa::Timer _timer;
        BulletType _type;

    protected:
        double _aim_angle;
        double _damage;
        bool _from_player;

        const PAA_ENTITY _e;

    public:
        ABullet(const PAA_ENTITY& e, const BulletType type,
            const double life_time, const double aim_angle, const double damage,
            const bool from_player);
        virtual ~ABullet() = default;

        bool is_alive() const;
        void force_kill() const;
        BulletType get_type() const;
        paa::Position& get_position() const;
        double get_aim_angle() const;
        double get_damage() const;

        virtual void update() = 0;
        virtual void on_collision(const paa::CollisionBox& other);
    };

    using Bullet = std::shared_ptr<ABullet>;

    class BasicBullet : public ABullet {
    private:
        paa::Vector2f _dir;
    public:
        BasicBullet(
            const PAA_ENTITY& e, const double& aim_angle, bool from_player);
        void update() override;
    };

    class SkeletonBullet : public ABullet {
    private:
        paa::Vector2f _dir;
    public:
        SkeletonBullet(
            const PAA_ENTITY&e, const double &aim_angle, bool from_player);

        void update() override;
    };

    class MissileBullet : public ABullet {
        private:
            paa::Vector2f _dir;
        public:
            MissileBullet(
                const PAA_ENTITY&e, const double &aim_angle, bool from_player);

            void update() override;
    };

    class BulletFactory {
    public:
        template <typename B, typename... Args>
        static inline Bullet make_bullet(Args&&... args)
        {
            return std::make_shared<B>(std::forward<Args>(args)...);
        }

        static void make_bullet_by_type(
            const std::string& bullet_type, paa::Position const& ref,
            const bool& from_player, float aim);

        static void make_skeleton_bullet(
            float aim_angle, paa::Position const& posRef, const bool& from_player);

        static void make_basic_bullet(float aim_angle,
            paa::Position const& posRef, const bool& from_player);

        static void make_missile_bullet(float aim_angle,
            paa::Position const& posRef, const bool& from_player);
    private:
        static inline std::unordered_map<std::string, std::function<void(float, paa::Position const&, const bool&)>> _matching_type = {
            {"basic_bullet", &make_basic_bullet},
            {"skeleton_bullet", &make_skeleton_bullet},
            {"missile_bullet", &make_missile_bullet}
        };
    };
}
}

/// Bullets
