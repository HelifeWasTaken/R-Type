#include "Collisions.hpp"
#include "Enemies.hpp"
#include "PileAA/Math.hpp"
#include "PileAA/BaseComponents.hpp"
#include "PileAA/external/nlohmann/json.hpp"

#include <fstream>

#define RTYPE_CENTIPEDE_FRAME_OFFSET 150
#define RTYPE_CENTIPEDE_SPEED 100

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
        return get_centipede(PAA_GET_COMPONENT(e, Enemy));
    }

    static CentipedeBody *get_centipede_body(PAA_ENTITY e)
    {
        return get_centipede_body(PAA_GET_COMPONENT(e, Enemy));
    }

    PAA_ENTITY CentipedeBody::build_centipede(const PAA_ENTITY& parent, int depth)
    {
        paa::DynamicEntity dp = parent;
        paa::DynamicEntity e = PAA_NEW_ENTITY();
        auto s = e.attachSprite("centiped_boss");

        depth ? s->useAnimation("body") : s->useAnimation("head");
        auto ppos = dp.getComponent<paa::Position>();

        e.attachPosition(ppos);
        e.attachHealth(paa::Health(10));
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
            get_centipede_body(_child)->kill();
        }
    }

    bool CentipedeBody::is_alive() const { return _alive; }

    bool CentipedeBody::centipede_part_functional()
    {
        if (_depth == 0)
            return false;
        return get_health().hp > 1 ||
            get_centipede_body(_child)->centipede_part_functional();
    }

    CentipedeBack CentipedeBody::get_back()
    {
        return _lastPosition;
    }

    void CentipedeBody::update()
    {
        auto& cpos = get_position();

        auto target_angle = paa::Math::direction_to_angle(cpos, _target.first);
        const double angle_speed = 0.05;

        spdlog::info("head angle");

        if (_timer.isFinished()) {
            _lastPosition = {cpos, _angle};
            cpos = _target.first;
            _timer.setTarget(RTYPE_CENTIPEDE_FRAME_OFFSET);

            if (_depth != RTYPE_CENTIPEDE_BODY_COUNT) {
                _target = get_centipede_body(_parent)->get_back();
            } else {
                _target = get_centipede(_parent)->get_back();
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
    }

    float Centipede::determine_time_to_next_point()
    {
        _path_index = (_path_index + 1) % _path.size();

        return 1000.f;
    }

    static void load_centipede_path(std::vector<paa::Vector2f>& path)
    {
        std::ifstream file("../assets/map/RecyclingFactory/centipede.json");
        nlohmann::json j;

        file >> j;

        for (auto& p : j["path"]) {
            path.push_back({p[0].get<float>(), p[1].get<float>()});
        }
    }

    Centipede::Centipede(const PAA_ENTITY& e)
        : AEnemy(e, EnemyType::CENTIPEDE_BOSS)
        , _lastPosition({{0, 0}, 0})
    {

        paa::DynamicEntity(e).attachId(paa::Id(-1));

        _timer.setTarget(RTYPE_CENTIPEDE_FRAME_OFFSET);

        _path.push_back({0, 0});
        _path.push_back({0, 100});
        _path.push_back({100, 100});
        _path.push_back({100, 0});

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

    bool Centipede::is_alive() const
    {
        auto c = get_centipede_body(_body_part);
        bool a = c->centipede_part_functional();

        if (!a) {
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
        const auto target = _path[_path_index];
        const float angle_speed = 0.1f;
        const float speed = RTYPE_CENTIPEDE_SPEED * PAA_DELTA_TIMER.getDeltaTime();

        if (_timer.isFinished()) {
            _lastPosition = {{cpos.x, cpos.y}, _angle};
            _timer.restart();
        }

        auto dir = paa::Math::distance(cpos, target);

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
