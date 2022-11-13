#include "Collisions.hpp"
#include "Enemies.hpp"
#include "PileAA/Math.hpp"
#include "PileAA/BaseComponents.hpp"
#include "PileAA/external/nlohmann/json.hpp"
#include "ClientScenes.hpp"

#include <fstream>
#include <PileAA/MusicPlayer.hpp>

#define RTYPE_CENTIPEDE_FRAME_OFFSET 100
#define RTYPE_CENTIPEDE_SPEED 100
#define RTYPE_CENTIPEDE_HP 10

namespace rtype {
namespace game {

    static CentipedeBody *get_centipede_body(Enemy& e)
    {
        return dynamic_cast<CentipedeBody *>(e.get());
    }

    static Centipede *get_centipede(Enemy& e)
    {
        return dynamic_cast<Centipede *>(e.get());
    }

    static Centipede *get_centipede(PAA_ENTITY e)
    {
        try {
            return get_centipede(PAA_GET_COMPONENT(e, Enemy));
        } catch (...) {
            return nullptr;
        }
    }

    static CentipedeBody *get_centipede_body(PAA_ENTITY e)
    {
        try {
            return get_centipede_body(PAA_GET_COMPONENT(e, Enemy));
         } catch (...) {
             return nullptr;
         }
    }

    PAA_ENTITY CentipedeBody::build_centipede(const PAA_ENTITY& parent, int depth)
    {
        paa::DynamicEntity dp = parent;
        paa::DynamicEntity e = PAA_NEW_ENTITY();
        auto s = e.attachSprite("centiped_boss");

        depth ? s->useAnimation("body") : s->useAnimation("head");
        auto ppos = dp.getComponent<paa::Position>();

        e.attachPosition(ppos);
        e.attachHealth(paa::Health(RTYPE_CENTIPEDE_HP));
        s->setPosition(ppos.x, ppos.y);
        s->setOrigin(s->getGlobalBounds().width / 2, s->getGlobalBounds().height / 2);
        e.attachCollision(CollisionFactory::makeEnemyCollision(
            paa::recTo<int>(s->getGlobalBounds()), e.getEntity()));

        e.attachId(paa::Id(-1));

        Enemy enemy = EnemyFactory::make_enemy<CentipedeBody>(e.getEntity(), parent, depth);
        e.insertComponent(std::move(enemy));
        return e.getEntity();
    }

    CentipedeBody::CentipedeBody(const PAA_ENTITY& self, const PAA_ENTITY& parent, int depth)
        : AEnemy(self, EnemyType::CENTIPEDE_BODY_BOSS)
        , _parent(parent)
        , _depth(depth)
        , _lastPosition({{0, 0}, 0})
        , _target({{0, 0}, 0})
    {
        _timer.setTarget(RTYPE_CENTIPEDE_FRAME_OFFSET);

        if (depth > 0) {
            _child = build_centipede(self, depth - 1);
        }
        s = PAA_GET_COMPONENT(self, paa::Sprite);
        auto& sp = PAA_GET_COMPONENT(_parent, paa::Sprite);

        s->setPosition(sp->getPosition().x, sp->getPosition().y);
        _lastPosition = {{sp->getPosition().x, sp->getPosition().y}, 0};
        _target = _lastPosition;
    }

    void CentipedeBody::on_collision(const paa::CollisionBox& other)
    {
        AEnemy::on_collision(other);
        auto& hp = get_health().hp;
        auto& s = PAA_GET_COMPONENT(_e, paa::Sprite);

        if (other.get_id() == rtype::game::CollisionType::PLAYER_BULLET)
            s->setColor(paa::Color::Red);
        if (hp <= 1) {
            hp = 1;
            if (_depth > 0)
                s->useAnimation("damage");
        }
    }

    void CentipedeBody::kill()
    {
        _alive = false;
        if (_depth != 0) {
            auto c = get_centipede_body(_child);
            if (c) {
                c->kill();
            }
        }
    }

    bool CentipedeBody::is_alive() { return _alive; }

    bool CentipedeBody::centipede_part_functional()
    {
        if (_depth == 0)
            return false;
        auto c = get_centipede_body(_child);
        if (!c) {
            _alive = false;
            return false;
        }
        if (get_health().hp > 1)
            return true;
        return c->centipede_part_functional();
    }

    CentipedeBack CentipedeBody::get_back()
    {
        return _lastPosition;
    }

    void CentipedeBody::update()
    {
        auto& cpos = get_position();
        const float& deltaTime = PAA_DELTA_TIMER.getDeltaTime();
        //cpos.x -= g_game.scroll;
        auto& s = PAA_GET_COMPONENT(_e, paa::Sprite);
        auto target_angle = paa::Math::direction_to_angle(cpos, _target.first);
        const double angle_speed = 0.05;

        if (s->getColor() == paa::Color::Red) {
            _red_timer += deltaTime;
            if (_red_timer >= .1f) {
                _red_timer = 0.0f;
                s->setColor(paa::Color::White);
            }
        }
        if (_timer.isFinished()) {
            _lastPosition = {cpos, _angle};
            cpos = _target.first;
            _timer.setTarget(RTYPE_CENTIPEDE_FRAME_OFFSET);

            if (_depth != RTYPE_CENTIPEDE_BODY_COUNT) {
                auto c = get_centipede_body(_parent);
                if (!c) {
                    _alive = false;
                    return;
                }
                _target = c->get_back();
            } else {
                auto c = get_centipede(_parent);
                if (!c) {
                    _alive = false;
                    return;
                }
                _target = c->get_back();
            }
            _angle = _target.second;
            s->setRotation(_angle + 180);
        } else {
            paa::Vector2f dir = {(float)_target.first.x - (float)cpos.x, (float)_target.first.y - (float)cpos.y};
            const double speed = RTYPE_CENTIPEDE_SPEED * PAA_DELTA_TIMER.getDeltaTime();

            dir.x = dir.x > 0 ? 1 : dir.x < 0 ? -1 : 0;
            dir.y = dir.y > 0 ? 1 : dir.y < 0 ? -1 : 0;

            cpos.x += speed * dir.x;
            cpos.y += speed * dir.y;
        }

        //cpos.x += g_game.scroll;
    }

    void CentipedeBody::heal_self_and_child()
    {
        if (_depth == 0) {
            return;
        }
        s->useAnimation("body");
        PAA_GET_COMPONENT(_e, paa::Health).hp = RTYPE_CENTIPEDE_HP;
        auto c = get_centipede_body(_child);
        if (!c) {
            _alive = false;
            return;
        }
        c->heal_self_and_child();
    }

    #define RTYPE_CENTIPEDE_PATH_ONE "../assets/maps/RecyclingFactory/centipede_p1.json"
    #define RTYPE_CENTIPEDE_PATH_TWO "../assets/maps/RecyclingFactory/centipede_p2.json"

    static void load_centipede_path(std::vector<paa::Vector2f>& path, const std::string& filepath)
    {
        path.clear();

        std::ifstream file(filepath);
        nlohmann::json j;

        file >> j;

        for (auto& p : j["path"]) {
            path.push_back({p[0].get<float>(), p[1].get<float>()});
        }
    }

    float Centipede::determine_time_to_next_point()
    {
        _path_index += 1;
        if (_path_index == _path.size()) {
            if (_phase_one) {
                load_centipede_path(_path, RTYPE_CENTIPEDE_PATH_TWO);
                _phase_one = false;
                auto c = get_centipede_body(_body_part);
                spdlog::critical("Healing!");
                if (!c) {
                    spdlog::critical("Healing aled!");
                    return 0.f;
                }
                c->heal_self_and_child();
                spdlog::critical("Healing healed!");
            }
            _path_index = 0;
        }
        return 0.f;
    }

    Centipede::Centipede(const PAA_ENTITY& e)
        : AEnemy(e, EnemyType::CENTIPEDE_BOSS)
        , _lastPosition({{0, 0}, 0})
        , _phase_one(true)
    {

        paa::DynamicEntity(e).attachId(paa::Id(-1));

        _on_death_triggered = false;
        _timer.setTarget(RTYPE_CENTIPEDE_FRAME_OFFSET);

        load_centipede_path(_path, RTYPE_CENTIPEDE_PATH_ONE);

        s = PAA_GET_COMPONENT(e, paa::Sprite);
        PAA_GET_COMPONENT(e, paa::Depth).z = 1;
        _body_part = CentipedeBody::build_centipede(_e);

        _lastPosition = {{s->getPosition().x, s->getPosition().y}, 0};

        determine_time_to_next_point();
    }


    void Centipede::on_collision(const paa::CollisionBox& other)
    {
        (void)other;
    }

    void Centipede::on_death() {
        paa::GMusicPlayer::play(MUSIC_COMBAT_IS_OVER, false);
    }

    void Centipede::attempt_trigger_death_event() {
        if (!_on_death_triggered) {
            on_death();
            _on_death_triggered = true;
        }
    }

    bool Centipede::is_alive()
    {
        if (_phase_one)
            return true;
        auto c = get_centipede_body(_body_part);

        if (!c) {
            return false;
        }

        bool a = c->centipede_part_functional();

        if (!a) {
            attempt_trigger_death_event();
            c->kill();
            return false;
        }
        return true;
    }

    std::pair<paa::Position, float> Centipede::get_back()
    {
        const auto inverted_angle = _angle + 180;
        const auto dir = paa::Math::angle_to_direction(inverted_angle);
        auto pos = s->getGlobalBounds();

        return _lastPosition;
    }

    static void update_angle(float &angle, const paa::Vector2f &dir)
    {
        if (dir.x == 0) {
            if (dir.y == 0) {
            } else if (dir.y < 0) {
                angle = 180;
            } else {
                angle = 0;
            }
        } else if (dir.x < 0) {
            if (dir.y == 0) {
                angle = 90;
            } else if (dir.y < 0) {
                angle = 135;
            } else {
                angle = 45;
            }
        } else {
            if (dir.y == 0) {
                angle = 270;
            } else if (dir.y < 0) {
                angle = 225;
            } else {
                angle = 315;
            }
        }
    }

    void Centipede::update()
    {
        auto& cpos = get_position();
        //cpos.x -= g_game.scroll;

        const auto target = _path[_path_index];
        const float angle_speed = 0.1f;
        const float speed = RTYPE_CENTIPEDE_SPEED * PAA_DELTA_TIMER.getDeltaTime();


        if (_timer.isFinished()) {
            _lastPosition = {{cpos.x, cpos.y}, _angle};
            _timer.restart();
        }

        auto dir = paa::Math::distance(cpos, target);

        //spdlog::info("target: {}, {}", target.x, target.y);

        dir.x = dir.x < speed && dir.x > -speed ? 0 : dir.x;
        dir.y = dir.y < speed && dir.y > -speed ? 0 : dir.y;

        cpos.x = dir.x < 0 ? cpos.x + speed : dir.x > 0 ? cpos.x - speed : cpos.x;
        cpos.y = dir.y < 0 ? cpos.y + speed : dir.y > 0 ? cpos.y - speed : cpos.y;

        cpos.x = std::abs(cpos.x - target.x) < speed ? target.x : cpos.x;
        cpos.y = std::abs(cpos.y - target.y) < speed ? target.y : cpos.y;

        update_angle(_angle, dir);

        s->setRotation(_angle);

        if (cpos.x == target.x && cpos.y == target.y) {
            determine_time_to_next_point();
        }

        //cpos.x += g_game.scroll;
    }

    PAA_ENTITY EnemyFactory::make_centipede_boss(double const& x, double const& y)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();
        auto s = e.attachSprite("centiped_boss");

        s->useAnimation("head");

        e.attachPosition(paa::Position(x, y));
        e.attachHealth(paa::Health(1));
        s->setPosition(x, y);
        s->setOrigin(s->getGlobalBounds().width / 2, s->getGlobalBounds().height / 2);
        e.attachCollision(CollisionFactory::makeEnemyCollision(
            paa::recTo<int>(s->getGlobalBounds()), e.getEntity()));

        Enemy enemy = EnemyFactory::make_enemy<Centipede>(e.getEntity());
        e.insertComponent(std::move(enemy));
        return e.getEntity();
    }
}
}
