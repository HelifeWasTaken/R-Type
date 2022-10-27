#pragma once

#include "ClientScenes.hpp"
#include "PileAA/DynamicEntity.hpp"
#include "PileAA/Math.hpp"
#include "Bullet.hpp"
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

    struct BulletQuery : public net::Serializable {
        bullet_type_t type;
        net::PlayerID id;

        BulletQuery() = default;
        ~BulletQuery() override = default;

        BulletQuery(bullet_type_t type, net::PlayerID id);
        BulletQuery(const std::vector<net::Byte>& bytes);

        std::vector<net::Byte> serialize() const override;
        void from(
            const net::Byte* data, const net::BufferSizeType size) override;
    };

    // Base impl

    class ABullet {
    private:
        paa::Timer _timer;
        BulletType _type;

    protected:
        double _aim_angle;
        double _damage;
        bool _destroyed_on_collision;

        const PAA_ENTITY _e;

    public:
        ABullet(const PAA_ENTITY& e,
                const BulletType type, const double life_time,
                const double aim_angle, const double damage);
        virtual ~ABullet() = default;

        bool is_alive() const;
        BulletType get_type() const;
        paa::Position& get_position() const;
        double get_aim_angle() const;
        double get_damage() const;

        virtual void update() = 0;
        virtual void on_collision(const paa::CollisionBox& other) = 0;
    };

    using Bullet = std::shared_ptr<ABullet>;

    template <typename B, typename... Args>
    static inline Bullet make_bullet(Args&&... args)
    {
        return std::make_shared<B>(std::forward<Args>(args)...);
    }

    class BasicBullet : public ABullet {
        private:
            paa::Vector2f _dir;

        public:
            BasicBullet(const PAA_ENTITY& e, const double &aim_angle)
                : ABullet(e, BulletType::BASIC_BULLET,
                        1500, aim_angle, 1)
                , _dir(paa::Math::angle_to_direction(aim_angle))
            {
            }

            void update() override
            {
                const auto dt = PAA_DELTA_TIMER.getDeltaTime();
                paa::Position& posRef = PAA_GET_COMPONENT(_e, paa::Position);

                posRef.x += _dir.x * BASIC_BULLET_SPEED * dt;
                posRef.y += _dir.y * BASIC_BULLET_SPEED * dt;
            }

            void on_collision(const paa::CollisionBox& other) override
            {
            }
    };

    class BulletFatory {
        public:
            static void make_basic_bullet(float aim_angle,
                                    paa::Position const& posRef)
            {
                paa::DynamicEntity e = PAA_NEW_ENTITY();

                paa::Position& pos = e.attachPosition(posRef);
                paa::Sprite& sprite = e.attachSprite("basic_bullet");
                sprite->useAnimation("base_animation");
                auto bullet = make_bullet<BasicBullet>(e.getEntity(), aim_angle);
                e.insertComponent(std::move(bullet));
            }
    };
}
}

/// Bullets
