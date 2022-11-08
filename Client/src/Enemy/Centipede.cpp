#include "Collisions.hpp"
#include "Enemies.hpp"
#include "PileAA/Math.hpp"
#include "PileAA/BaseComponents.hpp"

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
        auto s = e.attachSprite("centipede_boss_body");
        auto ppos = dp.getComponent<paa::Position>();

        e.attachPosition(ppos);
        e.attachHealth(paa::Health(10));
        s->setPosition(ppos.x, ppos.y);
        s->setOrigin(s->getGlobalBounds().width / 2, s->getGlobalBounds().height / 2);
        e.attachCollision(CollisionFactory::makeEnemyCollision(
            paa::recTo<int>(s->getGlobalBounds()), e.getEntity()));

        e.attachId(paa::Id(135));

        Enemy enemy = EnemyFactory::make_enemy<CentipedeBody>(e.getEntity(), parent, depth);
        e.insertComponent(std::move(enemy));
        return e.getEntity();
    }

    CentipedeBody::CentipedeBody(const PAA_ENTITY& self, const PAA_ENTITY& parent, int depth)
        : AEnemy(self, EnemyType::CENTIPEDE_BODY_BOSS)
        , _parent(parent)
        , _depth(depth)
        , _lastPosition({{0, 0}, 0})
    {
        _timer.setTarget(50 * (RTYPE_CENTIPEDE_BODY_COUNT - depth));

        if (depth > 0) {
            _child = build_centipede(self, depth - 1);
        }
        s = PAA_GET_COMPONENT(self, paa::Sprite);
        auto& sp = PAA_GET_COMPONENT(_parent, paa::Sprite);

        s->setPosition(sp->getPosition().x, sp->getPosition().y);
        _lastPosition = {{sp->getPosition().x, sp->getPosition().y}, 0};
    }

    void CentipedeBody::on_collision(const paa::CollisionBox& other)
    {
        AEnemy::on_collision(other);
        auto& hp = get_health().hp;

        hp = hp < 1 ? 1 : hp;
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
            return get_health().hp <= 1;
        return get_health().hp <= 1 ||
            get_centipede_body(_child)->centipede_part_functional();
    }

    CentipedeBack CentipedeBody::get_back()
    {
        return _lastPosition;
    }

    void CentipedeBody::update()
    {
        auto& cpos = get_position();
        CentipedeBack target = {{0, 0}, 0};

        if (_depth != RTYPE_CENTIPEDE_BODY_COUNT) {
            target = get_centipede_body(_parent)->get_back();
        } else {
            target = get_centipede(_parent)->get_back();
        }

        auto target_angle = paa::Math::direction_to_angle(cpos, target.first);
        const double angle_speed = 0.05;

        if (_timer.isFinished()) {
            _timer.setTarget(50);
            _lastPosition = {cpos, _angle};
        }

        cpos = target.first;
        _angle = target.second;
        s->setRotation(_angle, false);
    }

    float Centipede::determine_time_to_next_point()
    {
        _path_index = (_path_index + 1) % _path.size();

        return 1000.f;
    }

    Centipede::Centipede(const PAA_ENTITY& e)
        : AEnemy(e, EnemyType::CENTIPEDE_BOSS)
        , _lastPosition({{0, 0}, 0})
    {
        _path = {
            {100, 0},
            {500, 200}
        };

        _timer.setTarget(50);

        s = PAA_GET_COMPONENT(e, paa::Sprite);
        PAA_GET_COMPONENT(e, paa::Depth).z = 1;
        _body_part = CentipedeBody::build_centipede(_e);

        _lastPosition = {{s->getPosition().x, s->getPosition().y}, 0};

        s->setColor(sf::Color::Red);

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

        spdlog::info("dir: {}, {}", dir.x, dir.y);

        return _lastPosition;
    }

    void Centipede::update()
    {
        auto& cpos = get_position();
        const auto target = _path[_path_index];
        const float target_angle = paa::Math::toDegrees(paa::Math::direction_to_angle(cpos, target));
        const float angle_speed = 0.1f;
        const float speed = 500.f * PAA_DELTA_TIMER.getDeltaTime();

        _angle = target_angle;

        s->setRotation(_angle);

        if (_timer.isFinished()) {
            _lastPosition = {{cpos.x, cpos.y}, _angle};
            _timer.restart();
        }

        auto dir = paa::Math::distance(cpos, target);

        dir.x = dir.x < speed && dir.x > -speed ? 0 : dir.x;
        dir.y = dir.y < speed && dir.y > -speed ? 0 : dir.y;

        cpos.x = dir.x < 0 ? cpos.x + speed : dir.x > 0 ? cpos.x - speed : cpos.x;
        cpos.y = dir.y < 0 ? cpos.y + speed : dir.y > 0 ? cpos.y - speed : cpos.y;

        if (std::abs(cpos.x - target.x) < speed) {
            cpos.x = target.x;
        }
        if (std::abs(cpos.y - target.y) < speed) {
            cpos.y = target.y;
        }

        if (cpos.x == target.x && cpos.y == target.y) {
            determine_time_to_next_point();
        }
    }

    PAA_ENTITY EnemyFactory::make_centipede_boss(double const& x, double const& y)
    {
        paa::DynamicEntity e = PAA_NEW_ENTITY();
        auto s = e.attachSprite("centipede_boss");

        e.attachId(paa::Id(135));

        e.attachPosition(paa::Position(x, y));
        e.attachHealth(paa::Health(1));
        s->setPosition(x, y);
        s->setOrigin(s->getGlobalBounds().width / 2, s->getGlobalBounds().height / 2);
        /*
        e.attachCollision(CollisionFactory::makeEnemyCollision(
            paa::recTo<int>(s->getGlobalBounds()), e.getEntity()));
            */

        Enemy enemy = EnemyFactory::make_enemy<Centipede>(e.getEntity());
        e.insertComponent(std::move(enemy));
        return e.getEntity();
    }
}
}
