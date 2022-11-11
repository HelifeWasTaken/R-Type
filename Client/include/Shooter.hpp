#pragma once

#include "PileAA/DynamicEntity.hpp"
#include "PileAA/Timer.hpp"

namespace rtype {
namespace game {

    class AShooter {
    protected:
        paa::Timer _timer;
        double _aim_angle = 0;
        paa::DynamicEntity _parentEntity;

    public:
        AShooter(const PAA_ENTITY& e, double reloadTime);
        virtual ~AShooter() = default;

        bool can_shoot() const;
        bool can_shoot_and_restart();
        double aim_angle() const;
        AShooter& aim(const paa::Vector2f& to_aim);
        AShooter& aim(float const& aim_angle, bool isRadian = false);
        bool angle_is_set(void);
        bool is_attached_to_player() const;
        void shoot_from_pos(std::string const& bullet_type,
                paa::Position const& position);
        virtual void shoot(std::string const& bullet_type) = 0;
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
        BasicShooter(const PAA_ENTITY& e, double reloadTime = 900)
            : AShooter(e, reloadTime)
        {
        }

        ~BasicShooter() = default;

        void shoot(std::string const& bullet_type) override final;
    };

    class ConeShooter : public AShooter {
    public:
        ConeShooter(const PAA_ENTITY& e, double reloadTime=1500);

        ~ConeShooter() = default;

        void shoot(std::string const& bullet_type) override final;

    private:
        std::vector<Shooter> _shooterList;
    };
}
}
