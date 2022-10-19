#pragma once

enum BulletType : uint8_t {
    BASIC_BULLET
};

struct BulletQuery : public rtype::net::Serializable {
    BulletType type;
    PlayerID id;

    BulletQuery() = default;
    BulletQuery(BulletType type, PlayerID id) : type(type), id(id) {}
    BulletQuery(const std::vector<Byte>& bytes) { from(bytes.data(), bytes.size()); }

    std::vector<Byte> serialize(void) const override
    {
        rtype::net::Serializer s;
        s << id << (uint8_t)type;
        return s.data;
    }

    void from(const Byte *data, const BufferSizeType size) override
    {
        rtype::net::Serializer s(data, size);
        uint8_t t;
        s >> id >> t;
        type = (BulletType)t;
    }

    void create_then_send_message() const;
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
    virtual void update(paa::FloatRect& position) = 0;
};

using Bullet = std::shared_ptr<ABullet>;
template<typename B, typename ...Args>
Bullet make_bullet(Args&& ...args) {
    return std::make_shared<B>(std::forward<Args>(args)...);
}

/// Bullets


struct BasicBullet : public ABullet {
    BasicBullet() : ABullet(1000, BASIC_BULLET) {}
    void update(paa::FloatRect& position) override
    {
        position.left += 10;
    }
};

// Factory

class BulletFactory {
private:
    static inline std::unordered_map<
        BulletType,
        std::function<void(const PAA_ENTITY&, const PAA_ENTITY&)>
    > _bullets = {
        {
            BulletType::BASIC_BULLET,
            [](const PAA_ENTITY& e, const PAA_ENTITY& sender_id) {
                auto& s = PAA_SET_SPRITE(e, "basic_bullet");
                const auto bounds = PAA_GET_COMPONENT(sender_id, paa::Sprite)->getGlobalBounds();
                s.useAnimation("base_animation");
                s.setPosition(paa::Vector2f(bounds.left + bounds.width, bounds.top));
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
                    auto bounds = s->getGlobalBounds();
                    b->update(bounds);
                    s->setPosition(paa::Vector2f(bounds.left, bounds.top));
                } else {
                    PAA_DESTROY_ENTITY(e);
                }
            }
        });
    }

    static PAA_ENTITY create(const BulletType bulletType, const PAA_ENTITY& sender_id) {
        PAA_ENTITY e = PAA_NEW_ENTITY();

        _bullets[bulletType](e, sender_id);
        return e;
    }

    static PAA_ENTITY create(const BulletQuery& query) {
        return create(query.type, g_game.players_entities[query.id]);
    }

};

    void BulletQuery::create_then_send_message() const
    {
        BulletFactory::create(*this);
        g_game.service.udp().send(UpdateMessage(id, *this, message_code::PLAYER_SHOOT));
    }
