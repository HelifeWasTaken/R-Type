#pragma once

#include "Shooter.hpp"

namespace rtype {
namespace game {

class AEnemy {
private:
    // TODO Collision Ref

public:
    AEnemy() {}
    virtual ~AEnemy() = default;

    virtual void update() = 0;
};

using Enemy = std::shared_ptr<AEnemy>

template<typename T, typenmae ...Args>
static inline Enemy make_enemy(Args&& ...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

}
}