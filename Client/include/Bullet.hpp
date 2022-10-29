#pragma once

#include "Bullet.hpp"
#include "ClientScenes.hpp"
#include "Collisions.hpp"
#include "PileAA/DynamicEntity.hpp"
#include "PileAA/Math.hpp"
#include <PileAA/Types.hpp>
#include <cmath>

namespace paa {
class CollisionBox;
}

#define BASIC_BULLET_SPEED 800

namespace rtype {
namespace game {

    using bullet_type_t = uint8_t;

    enum BulletType : bullet_type_t { BASIC_BULLET };

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

    class BulletFactory {
    public:
        template <typename B, typename... Args>
        static inline Bullet make_bullet(Args&&... args)
        {
            return std::make_shared<B>(std::forward<Args>(args)...);
        }

        static void make_basic_bullet(float aim_angle,
            paa::Position const& posRef, const bool& from_player);
    };
}
}

/// Bullets
