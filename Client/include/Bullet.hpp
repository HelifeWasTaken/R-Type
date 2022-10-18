#pragma once

enum BulletType : uint8_t {
    BASIC_BULLET
};

// Base impl

class ABullet {
private:
    paa::Timer _timer;
    BulletType _type;
public:
    ABullet(const double lifeTime, const BulletType type)
        : _type(type) { _timer.setTarget(lifeTime); }
    bool is_alive() const { return !_timer.isFinished(); }
    BulletType get_type() const { return _type; }
    virtual void update(paa::Vector2f& position) = 0;
};

using Bullet = std::shared_ptr<ABullet>;
template<typename B, typename ...Args>
Bullet make_bullet(Args&& ...args) {
    return std::make_shared<B>(std::forward<Args>(args)...);
}

/// Bullets


struct BasicBullet : public ABullet {
    BasicBullet() : ABullet(1000, BASIC_BULLET) {}
    void update(paa::Vector2f& position) override
    {
        position.x += 1;
    }
};

// Factory

class BulletFactory {
private:
    static inline std::unordered_map<
        std::string,
        std::function<void(const PAA_ENTITY&, const PAA_ENTITY&)>
    > _bullets = {
        {
            "basic_bullet",
            [](const PAA_ENTITY& e, const PAA_ENTITY& sender_id) {
                auto& s = PAA_SET_SPRITE(e, "basic_bullet");
                s.useAnimation("base_animation");
                s.setPosition(PAA_GET_COMPONENT(sender_id, paa::Sprite)->getPosition());
                PAA_SET_COMPONENT(e, Bullet, make_bullet<BasicBullet>());
            }
        }
    };

public:
    BulletFactory() = default;
    ~BulletFactory() = default;

    static void setup_systems()
    {
        static bool done = false;

        if (done) return;

        done = true;
        PAA_ECS.register_component<Bullet>();
        PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
            for (auto&& [e, b, s] : r.view<Bullet, paa::Sprite>()) {
                if (b->is_alive()) {
                    auto pos = s->getPosition();
                    b->update(pos);
                    s->setPosition(pos);
                } else {
                    PAA_DESTROY_ENTITY(e);
                }
            }
        });
    }

    static PAA_ENTITY create(const std::string& bulletType, const PAA_ENTITY& sender_id) {
        PAA_ENTITY e = PAA_NEW_ENTITY();

        _bullets[bulletType](e, sender_id);
        return e;
    }

};

