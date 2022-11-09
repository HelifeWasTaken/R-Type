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

            _up_booster = paa::make_dynamic_entity(PAA_NEW_ENTITY());
            _up_booster->attachSprite("robot_boss")->useAnimation("up_booster");
            _up_booster->attachPosition(_eye->getComponent<paa::Position>());

            _down_booster = paa::make_dynamic_entity(PAA_NEW_ENTITY());
            _down_booster->attachSprite("robot_boss")->useAnimation("down_booster");
            _down_booster->attachPosition(_eye->getComponent<paa::Position>());

            _up_shooter = paa::make_dynamic_entity(PAA_NEW_ENTITY());
            _up_shooter->attachSprite("robot_boss")->useAnimation("weapon");
            _up_shooter->attachPosition(_eye->getComponent<paa::Position>());

            _down_shooter = paa::make_dynamic_entity(PAA_NEW_ENTITY());
            _down_shooter->attachSprite("robot_boss")->useAnimation("weapon");
            _down_shooter->attachPosition(_eye->getComponent<paa::Position>());
        }

        void update()
        {
            auto& eye_pos = _eye->getComponent<paa::Position>();
            auto& self_pos = _self->getComponent<paa::Position>();
            auto& top_pos = _up_booster->getComponent<paa::Position>();
            auto& bot_pos = _down_booster->getComponent<paa::Position>();
            auto& top_shooter_pos = _up_shooter->getComponent<paa::Position>();
            auto& bot_shooter_pos = _down_shooter->getComponent<paa::Position>();

            self_pos.x = eye_pos.x;
            self_pos.y = eye_pos.y - 100 / 2;

            top_pos.x = eye_pos.x;
            top_pos.y = eye_pos.y - 90;

            bot_pos.x = eye_pos.x;
            bot_pos.y = eye_pos.y + 70;

            top_shooter_pos.x = eye_pos.x - 15;
            top_shooter_pos.y = eye_pos.y - 50;

            bot_shooter_pos.x = eye_pos.x - 15;
            bot_shooter_pos.y = eye_pos.y + 60;
        }
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
