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

        static inline constexpr float SHOOTER_MAX_MOVEMENT = 80;

        static inline constexpr float UP_SHOOTER_MIN_Y = 30;
        float  dir_y_top                               = UP_SHOOTER_MIN_Y;

        static inline constexpr float DOWN_SHOOTER_MIN_Y = 120;
        float dir_y_bot = 0.0f;
        float speed = 50.0f;

        static inline constexpr float SHOOTER_RELOAD_TIME = 750.f;
        Shooter _up_shooter_comp;
        Shooter _down_shooter_comp;

    public:
        RobotBody(const PAA_ENTITY& self, const PAA_ENTITY& eye)
            : _self(paa::make_dynamic_entity(self))
            , _eye(paa::make_dynamic_entity(eye))
        {
            static const char *const animations[] = {
                "up_booster", "down_booster", "weapon", "weapon"
            };
            std::reference_wrapper<paa::SDynamicEntity> entities_ref[] = {
                _up_booster, _down_booster, _up_shooter, _down_shooter
            };

            _self->attachSprite("robot_boss")->useAnimation("boss");
            _self->attachPosition(_eye->getComponent<paa::Position>());

            for (size_t i = 0; i < 4; ++i) {
                entities_ref[i].get() = paa::make_dynamic_entity(PAA_NEW_ENTITY());
                entities_ref[i].get()->attachSprite("robot_boss")->useAnimation(animations[i]);
                entities_ref[i].get()->attachPosition(_eye->getComponent<paa::Position>());

                entities_ref[i].get()->attachCollision(CollisionFactory::makeT(
                    paa::recTo<int>(entities_ref[i].get()->getComponent<paa::Sprite>()->getGlobalBounds()),
                    entities_ref[i].get()->getEntity()));
            }

            new_position_direction_shooter(dir_y_top, UP_SHOOTER_MIN_Y);
            new_position_direction_shooter(dir_y_bot, DOWN_SHOOTER_MIN_Y);

            _up_shooter_comp = make_shooter<BasicShooter>(*_up_shooter, SHOOTER_RELOAD_TIME);
            _down_shooter_comp = make_shooter<BasicShooter>(*_down_shooter, SHOOTER_RELOAD_TIME);
        }

        void new_position_direction_shooter(float& dir_y, const float& min_y)
        {
            dir_y = std::fmod((float)paa::Random::rand(), SHOOTER_MAX_MOVEMENT) + min_y;
        }

        void shooter_go_towards(paa::Position& cpos, float& dir_y, const float& min_y)
        {
            const float current_speed = speed * PAA_DELTA_TIMER.getDeltaTime();

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
            if (!_eye->hasComponent<paa::Position>()) {
                _self->kill();
                _up_booster->kill();
                _down_booster->kill();
                _up_shooter->kill();
                _down_shooter->kill();
                return;
            }
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

    void RobotBossEye::on_collision(const paa::CollisionBox& other)
    {
        paa::Health& health = PAA_GET_COMPONENT(_e, paa::Health);

        if (_state == State::VULNERABLE || _state == State::TRANSITION) {
            AEnemy::on_collision(other);
        }
    }

    RobotBossEye::RobotBossEye(const PAA_ENTITY& e)
        : AEnemy(e, ROBOT_BOSS_EYE)
    {
        _body = PAA_NEW_ENTITY();
        _body.emplaceComponent<RobotBody>(_body, _e);

        _vulnerable_timer.setTarget(VULNERABLE_TIME);
    }

    void RobotBossEye::update()
    {
        constexpr int OFFSET_MAX = 50;
        const float min = g_game.scroll + RTYPE_PLAYFIELD_WIDTH - OFFSET_MAX;
        auto& cpos = get_position();

        if (min > cpos.x) {
            cpos.x = min;
        }

        auto& s = PAA_GET_COMPONENT(_e, paa::Sprite);

        switch (_state) {
        case State::VULNERABLE:
            if (_vulnerable_timer.isFinished()) {
                s->useAnimation("eye_close_animation", false);
                _state = State::TRANSITION;
            }
            break;
        case State::TRANSITION:
            if (s->isLastFrame()) {
                _state = State::INVULNERABLE;
                _vulnerable_timer.setTarget(INVULNERABLE_TIME);
            }
            break;
        case State::INVULNERABLE:
            if (_vulnerable_timer.isFinished()) {
                s->useAnimation("eye_open_animation", false);
                _state = State::VULNERABLE;
                _vulnerable_timer.setTarget(VULNERABLE_TIME);
            }
            break;
        }
    }
}
}
