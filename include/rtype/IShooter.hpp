#pragma once

#include "BulletPool.hpp"

namespace rtype {
class IShooter {
public:
    virtual void aim(const float& angle) = 0;
    virtual void aim(const float& x, const float& y) = 0;

    virtual void shoot() = 0;

    virtual bool can_shoot() const = 0;

    virtual void set_bullet(IBullet *bullet) = 0;

    IShooter() = default;
    virtual ~IShooter() = default;
};
}
