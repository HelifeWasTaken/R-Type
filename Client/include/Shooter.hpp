#pragma once

#include "Bullet.hpp"

namespace rtype {
namespace game {

class AShooter {
private:
    Timer _timer;
    double _aim_angle = -90;
    const paa::Position& _positionRef;

public:
    AShooter(double reloadTime, const paa::Position& posRef);
    virtual ~AShooter() = default;

    bool can_shoot() const;
    bool can_shoot_and_restart();
    double aim_angle() const;
    void aim(const paa::Vector2f& to_aim);
    virtual void shoot() = 0;
};

using Shooter = std::shared_ptr<AShooter>;

using ShooterList = std::vector<Shooter>;

template<typename T, typename ...Args>
static inline Shooter make_shooter(Args&& ...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

}
}