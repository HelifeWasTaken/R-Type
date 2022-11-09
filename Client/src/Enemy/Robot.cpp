#include "Enemies.hpp"
#include "utils.hpp"

namespace rtype {
namespace game {

    class RobotBody {
    private:
        paa::SDynamicEntity _up_booster;
        paa::SDynamicEntity _down_booster;
        paa::SDynamicEntity _up_shooter;
        paa::SDynamicEntity _down_shooter;
        paa::SDynamicEntity _eye;
        paa::SDynamicEntity _self;

    public:
        RobotBody(const PAA_ENTITY& self, const PAA_ENTITY& eye)
            : _self(paa::make_dynamic_entity(self))
            , _eye(paa::make_dynamic_entity(eye))
        {
            _self->attachSprite("robot_boss")->useAnimation("boss");
            _self->attachPosition(_eye->getComponent<paa::Position>());
        }

        void update()
        {
            auto& eye_pos = _eye->getComponent<paa::Position>();
            auto& self_pos = _self->getComponent<paa::Position>();

            self_pos.x = eye_pos.x;
            self_pos.y = eye_pos.y - 100 / 2;
        }

        HL_AUTO_COMPLETE_CANONICAL_FORM(RobotBody);
    };

    void RobotBossEye::register_robot_components()
    {
        PAA_REGISTER_COMPONENTS(RobotBody);

        PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
            PAA_ITERATE_VIEW(PAA_VIEW_COMPONENTS(RobotBody), body) {
                body.update();
            }
        });
    }

    RobotBossEye::RobotBossEye(const PAA_ENTITY& e)
        : AEnemy(e, ROBOT_BOSS_EYE)
    {
        _body = PAA_NEW_ENTITY();
        _body.emplaceComponent<RobotBody>(_body, _e);
    }

    void RobotBossEye::update()
    {
        spdlog::info("updating");
        constexpr int OFFSET_MAX = 50;
        const float min = g_game.scroll + RTYPE_PLAYFIELD_WIDTH - OFFSET_MAX;
        auto& cpos = get_position();

        if (min > cpos.x) {
            cpos.x = min;
            spdlog::critical("cpos: {}", cpos.x);
        }
    }
}
}
