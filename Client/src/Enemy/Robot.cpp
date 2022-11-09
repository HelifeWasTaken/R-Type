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

        static inline constexpr float OFFSET_BODY        = 100 / 2;
        static inline constexpr float OFFSET_TOP_BOOST_Y = 90;
        static inline constexpr float OFFSET_BOT_BOOST_Y = 70;
        static inline constexpr float OFFSET_BOOST_X     = 15;

        static inline constexpr float SHOOTER_MAX_MOVEMENT = 20;

        static inline constexpr float UP_SHOOTER_MIN_Y = 30;
        float  dir_y_top                               = UP_SHOOTER_MIN_Y;

        static inline constexpr float DOWN_SHOOTER_MIN_Y = 192;
        float dir_y_bot = 0.0f;
        float speed = 20.0f;

        Shooter _up_shooter_comp;
        Shooter _down_shooter_comp;

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

            new_position_direction_shooter(dir_y_top, UP_SHOOTER_MIN_Y);
            new_position_direction_shooter(dir_y_bot, DOWN_SHOOTER_MIN_Y);

            _up_shooter_comp = make_shooter<BasicShooter>(*_up_shooter, 200.f);
            _down_shooter_comp = make_shooter<BasicShooter>(*_down_shooter, 200.f);
        }

        void new_position_direction_shooter(float& dir_y, const float& min_y)
        {
            dir_y = std::fmod((float)paa::Random::rand(), SHOOTER_MAX_MOVEMENT) + min_y;
        }

        void shooter_go_towards(paa::Position& cpos, float& dir_y, const float& min_y)
        {
            const float current_speed = speed * PAA_DELTA_TIMER.getDeltaTime();

            spdlog::info("Current speed: {}, Dir: {}", current_speed, dir_y);

            if (std::abs(current_speed) > std::abs(cpos.y - dir_y)) {
                cpos.y = dir_y;
            }
            if (cpos.y < dir_y) {
                cpos.y += current_speed;
            } else if (cpos.y > dir_y) {
                cpos.y -= current_speed;
            } else {
                new_position_direction_shooter(dir_y, min_y);
            }
        }

        void update()
        {
            const float current_speed = speed * PAA_DELTA_TIMER.getDeltaTime();
            auto& eye_pos         = _eye->getComponent<paa::Position>();
            auto& self_pos        = _self->getComponent<paa::Position>();
            auto& top_pos         = _up_booster->getComponent<paa::Position>();
            auto& bot_pos         = _down_booster->getComponent<paa::Position>();
            auto& top_shooter_pos = _up_shooter->getComponent<paa::Position>();
            auto& bot_shooter_pos = _down_shooter->getComponent<paa::Position>();

            self_pos.x = eye_pos.x;
            self_pos.y = eye_pos.y - OFFSET_BODY;

            top_pos.x = eye_pos.x;
            top_pos.y = eye_pos.y - OFFSET_TOP_BOOST_Y;

            bot_pos.x = eye_pos.x;
            bot_pos.y = eye_pos.y + OFFSET_BOT_BOOST_Y;

            top_shooter_pos.x = eye_pos.x - OFFSET_BOOST_X;
            bot_shooter_pos.x = eye_pos.x - OFFSET_BOOST_X;

            shooter_go_towards(top_shooter_pos, dir_y_top, UP_SHOOTER_MIN_Y);
            shooter_go_towards(bot_shooter_pos, dir_y_bot, DOWN_SHOOTER_MIN_Y);

            auto id = g_game.get_random_player();

            if (id != -1) {
                try {
                    auto& player = PAA_GET_COMPONENT(g_game.players_entities[id], paa::Position);
                    _up_shooter_comp->aim(paa::Vector2f(player.x, player.y));
                    _down_shooter_comp->aim(paa::Vector2f(player.x, player.y));
                } catch (...) {}
            }
            _up_shooter_comp->shoot("missile_bullet");
            _down_shooter_comp->shoot("missile_bullet");
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
