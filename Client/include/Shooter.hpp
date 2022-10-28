#pragma once

#include "PileAA/Timer.hpp"
#include "PileAA/DynamicEntity.hpp"

namespace rtype {
namespace game {

    class AShooter {
    protected:
        paa::Timer _timer;
        double _aim_angle = 0;
        paa::DynamicEntity _parentEntity;

    public:
        AShooter(const PAA_ENTITY& e,
                double reloadTime);
        virtual ~AShooter() = default;

        bool can_shoot() const;
        bool can_shoot_and_restart();
        double aim_angle() const;
        AShooter& aim(const paa::Vector2f& to_aim);
        AShooter& aim(float const& aim_angle, bool isRadian=false);
        virtual void shoot() = 0;
    };

    using Shooter = std::shared_ptr<AShooter>;

    using ShooterList = std::vector<Shooter>;

    template <typename T, typename... Args>
    static inline Shooter make_shooter(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }


    class BasicShooter : public AShooter {
        public:
            BasicShooter(const PAA_ENTITY& e, double reloadTime=900)
                : AShooter(e, reloadTime)
            {}

            ~BasicShooter() = default;

            void shoot() override final;
    };
}
}
