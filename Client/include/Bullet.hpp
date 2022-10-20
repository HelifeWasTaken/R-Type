#pragma once

#include <PileAA/Types.hpp>
#include "ClientScenes.hpp"
#include <cmath>

namespace rtype {
namespace game {

using bullet_type_t = uint8_t;

enum BulletType : bullet_type_t {
    BASIC_BULLET
};

struct BulletQuery : public net::Serializable {
    bullet_type_t type;
    net::PlayerID id;

    BulletQuery() = default;
    ~BulletQuery() override = default;

    BulletQuery(bullet_type_t type, net::PlayerID id);
    BulletQuery(const std::vector<net::Byte>& bytes);

    std::vector<net::Byte> serialize() const override;
    void from(const net::Byte *data, const net::BufferSizeType size) override;
};

// Base impl

class ABullet {
private:
    paa::Timer _timer;
    BulletType _type;
    paa::Position& _posRef;
    double _aim_angle;
    // TODO Have collider there

public:
    ABullet(const double lifeTime,
            const BulletType type,
            const double aim_angle,
            paa::Position& posRef);
    virtual ~ABullet() = default;

    bool is_alive() const;
    BulletType get_type() const;
    paa::Position& get_position() const;
    double get_aim_angle() const;

    virtual void update() = 0;
};

using Bullet = std::shared_ptr<ABullet>;

template<typename B, typename ...Args>
static inline Bullet make_bullet(Args&& ...args) {
    return std::make_shared<B>(std::forward<Args>(args)...);
}

}
}

/// Bullets