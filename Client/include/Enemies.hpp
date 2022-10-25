#pragma once

#include "Shooter.hpp"

namespace rtype {
namespace game {

    class AEnemy {
    protected:
        paa::Position& _positionRef;

    public:
        AEnemy(paa::Position& positionRef)
            : _positionRef(positionRef)
        {
        }

        virtual ~AEnemy() = default;

        virtual void update() = 0;
        virtual void on_collision(const paa::CollisionBox& other) = 0;
    };

    using Enemy = std::shared_ptr<AEnemy>;

    class EnemyFactory {
        template <typename T, typename... Args>
        static Enemy make_enemy(Args&&... args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }
    };

}
}