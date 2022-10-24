#pragma once

#include "ClientScenes.hpp"
#include <PileAA/Types.hpp>
#include <cmath>

namespace paa {
class CollisionBox;
}

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
        bool _from_player;

    protected:
        double _aim_angle;
        double _damage;
        bool _destroyed_on_collision;
        paa::Position& _posRef;

    public:
        ABullet(const BulletType type, const double life_time,
            const double aim_angle, const double damage, const bool from_player,
            paa::Position& posRef);
        virtual ~ABullet() = default;

        bool is_alive() const;
        BulletType get_type() const;
        paa::Position& get_position() const;
        double get_aim_angle() const;
        double get_damage() const;
        bool is_from_player() const;

        virtual void update() = 0;
        virtual void on_collision(const paa::CollisionBox& other) = 0;
    };

    using Bullet = std::shared_ptr<ABullet>;

    template <typename B, typename... Args>
    static inline Bullet make_bullet(Args&&... args)
    {
        return std::make_shared<B>(std::forward<Args>(args)...);
    }

}
}

/// Bullets