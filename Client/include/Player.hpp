#pragma once

#include "Shooter.hpp"
#include "Collisions.hpp"
#include "PileAA/DynamicEntity.hpp"
#include "PileAA/Rand.hpp"
#include "RServer/Message/Types.hpp"

#ifndef RTYPE_PLAYER_MAX_HEALTH
    #define RTYPE_PLAYER_MAX_HEALTH 3
#endif

#ifndef RTYPE_PLAYER_SYNC_RATE
    #define RTYPE_PLAYER_SYNC_RATE 20
#endif

#ifndef RTYPE_PLAYER_Y_FRAMES
    #define RTYPE_PLAYER_Y_FRAMES 4
#endif

#ifndef RTYPE_PLAYER_SPEED
    #define RTYPE_PLAYER_SPEED 0.5f
#endif

#ifndef RTYPE_PLAYER_FRAME_RATE
    #define RTYPE_PLAYER_FRAME_RATE 50
#endif

namespace rtype {
namespace game {

class SerializablePlayer : public net::Serializable {
public:
    using mask_t = paa::u16;
    using data_t = paa::u16;

    static constexpr mask_t MOVE_LEFT_MASK     = 0b0000000000000001;
    static constexpr mask_t MOVE_LEFT_SHIFT    = 0x0;

    static constexpr mask_t MOVE_RIGHT_MASK    = 0b0000000000000010;
    static constexpr mask_t MOVE_RIGHT_SHIFT   = 0x1;

    static constexpr mask_t MOVE_UP_MASK       = 0b0000000000000100;
    static constexpr mask_t MOVE_UP_SHIFT      = 0x2;

    static constexpr mask_t MOVE_DOWN_MASK     = 0b0000000000001000;
    static constexpr mask_t MOVE_DOWN_SHIFT    = 0x3;

    static constexpr mask_t MOVE_MASK_X_AXIS   = MOVE_LEFT_MASK | MOVE_RIGHT_MASK;
    static constexpr mask_t MOVE_MASK_Y_AXIS   = MOVE_UP_MASK | MOVE_DOWN_MASK;
    static constexpr mask_t MOVE_MASK          = MOVE_MASK_X_AXIS | MOVE_MASK_Y_AXIS;

    static constexpr mask_t SHOOT_MASK         = 0b0000000000010000;
    static constexpr mask_t SHOOT_SHIFT        = 0x4;

    static constexpr mask_t PLAYER_MASK        = 0b0000000001100000;
    static constexpr mask_t PLAYER_SHIFT       = 0x5;

    static constexpr mask_t HEALTH_MASK        = 0b0000000110000000;
    static constexpr mask_t HEALTH_SHIFT       = 0x7;

    static constexpr mask_t UNUSED_MASK        = 0b1111111000000000;
    static constexpr mask_t UNUSED_SHIFT       = 0x9;

    data_t data = 0x0;
    net::vector2i pos = {0, 0};

    SerializableInput() = default;
    SerializableInput(const SerializableInput& other) = default;

    SerializableInput(const std::vector<Byte>& data)
    { from(data.data(), data.size()); }

    Serializable(const PAA_ENTITY& entity)
    { set_from_entity(entity); }

    Serializable(const paa::Controller& controller,
                const paa::Position& position,
                const paa::Health& health)
    { set_from_components(controller, position, health); }

    std::vector<Byte> serialize() const override {
        net::Serializer s;
        s << data;
        s.add_bytes(pos.serialize());
        return s.data;
    }

    void from(const Byte *data, const BufferSizeType size) override {
        net::Serializer s(data, size);
        s >> data;
        pos.from(s.data.data(), s.data.size());
    }

    void set_from_components(const paa::Controller& controller,
                            const paa::Position& pos,
                            const paa::Health& health) {
        const paa::Vector2f xy = controller->getAxisXY();

        set_position(pos)
            .set_hp(health.hp)
            .set_move_left(xy.x < -20.f)
            .set_move_right(xy.x > 20.f)
            .set_move_up(xy.y < -20.f)
            .set_move_down(xy.y > 20.f)
            .set_shoot(controller->isButtonPressed(0));
    }

    void set_from_entity(const PAA_ENTITY& e) {
        paa::DynamicEntity entity = e;
        return set_from_components(
            entity.get_component<paa::Controller>(),
            entity.get_component<paa::Position>();
            entity.get_component<paa::Health>()
        );
    }

    SerializableInput& set_data(const data_t& value, mask_t mask) {
        if (value) input |= mask;
        else input &= ~mask;
        return *this;
    }

    SerializableInput& set_move_left(bool value) {
        return set_data(static_cast<data_t>(value), MOVE_LEFT_MASK);
    }

    SerializableInput& set_move_right(bool value) {
        return set_data(static_cast<data_t>(value), MOVE_RIGHT_MASK);
    }

    SerializableInput& set_move_up(bool value) {
        return set_data(static_cast<data_t>(value), MOVE_UP_MASK);
    }

    SerializableInput& set_move_down(bool value) {
        return set_data(static_cast<data_t>(value), MOVE_DOWN_MASK);
    }

    SerializableInput& set_shoot(bool value) {
        return set_data(static_cast<data_t>(value), SHOOT_MASK);
    }

    SerializableInput& set_hp(const paa::u8& hp) {
        return set_data(static_cast<data_t>(hp), HEALTH_MASK);
    }

    SerializableInput& set_player(net::PlayerID player) {
        input &= ~PLAYER_MASK;
        input |= (player << 5);
        return *this;
    }

    SerializableInput& set_pos(const paa::Position& pos) {
        this->pos = {pos.x, pos.y};
        return *this;
    }

    data_t get_data(const mask_t& mask, const mask_t& shift) const {
        return (input & mask) >> shift;
    }

    data_t get_move_left() const {
        return get_data(MOVE_LEFT_MASK, MOVE_LEFT_SHIFT);
    }

    data_t get_move_right() const {
        return get_data(MOVE_RIGHT_MASK, MOVE_RIGHT_SHIFT);
    }

    data_t get_move_up() const {
        return get_data(MOVE_UP_MASK, MOVE_UP_SHIFT);
    }

    data_t get_move_down() const {
        return get_data(MOVE_DOWN_MASK, MOVE_DOWN_SHIFT);
    }

    data_t get_shoot() const {
        return get_data(SHOOT_MASK, SHOOT_SHIFT);
    }

    data_t get_player() const {
        return get_data(PLAYER_MASK, PLAYER_SHIFT);
    }

    data_t get_hp() const {
        return get_data(HEALTH_MASK, HEALTH_SHIFT);
    }

    const net::vector2i& get_pos() const {
        return pos;
    }

    bool input_is_same(const SerializablePlayer& other) {
        return input == other.input;
    }

    bool pos_is_same(const SerializablePlayer& other) {
        return pos == other.pos;
    }

    bool is_same(const SerializablePlayer& other) {
        return input_is_same(other) && pos_is_same(other);
    }

    bool operator==(const SerializablePlayer& other) const {
        return is_same(other);
    }

    bool operator!=(const SerializablePlayer& other) const {
        return !is_same(other);
    }
};

class APlayer {
protected:
    const PAA_ENTITY _entity;
    paa::Position& _positionRef;
    paa::Health& _healthRef;
    paa::Sprite _spriteRef;
    paa::Controller _controllerRef;
    paa::ShooterList _shooterList;

    SerializablePlayer _info;

    paa::Timer _syncTimer;
    paa::Timer _hurtTimer;
    paa::Timer _frameTimer;

    bool _is_hurt = false;

    bool _is_local; // is this player local or remote

    paa::Vector2u _original_scale;

    int _y_frame = 0;

public:
    APlayer(const PAA_ENTITY& entity,
            paa::Position& positionRef,
            paa::Health& health,
            paa::Sprite& spriteRef,
            paa::Controller& controllerRef,
            bool is_local)
        : _entity(entity)
        , _positionRef(positionRef)
        , _healthRef(health)
        , _spriteRef(spriteRef)
        , _controllerRef(controllerRef)
        , _is_local(is_local)
    {
        _syncTimer.setTarget(RTYPE_PLAYER_SYNC_RATE);
        _frameTimer.setTarget(RTYPE_PLAYER_FRAME_RATE);
        _original_scale = _spriteRef->getScale();
    }

    ~APlayer() = default;

    void update_info(const SerializablePlayer& info) {
        _info = info;
        _positionRef = {info.get_pos().x, info.get_pos().y};
        _controllerRef->simulateAxisMovement(paa::InputHandler::Axis::X, (info.get_move_left() - info.get_move_right()) * 100.f);
        _controllerRef->simulateAxisMovement(paa::InputHandler::Axis::Y, (info.get_move_up() - info.get_move_down()) * 100.f);
        _healthRef.hp = info.get_hp();
        if (info.get_shoot()) {
            _controllerRef->simulateButtonHeld(0);
        } else {
            _controllerRef->simulateButtonRelease(0);
        }
    }

    void update_shoot() {
        if (_controllerRef->isButtonPressed(0))
            for (auto& shooter : _shooterList)
                shooter->shoot();
    }

    void update_data() {
        // Sync player position and input with server
        // We only sync if the player is local
        // and if the timer is ready
        if (_is_local && _syncTimer.isFinished()) {
            SerializablePlayer info(_entity);
            if (info != _info) {
                g_game.service.udp().send(net::UpdateMessage(info, net::message_code::UPDATE_PLAYER));
                _info = info;
            }
            _syncTimer.restart();
        }
    }

    void update_sprite_hurt()
    {
        if (_hurtTimer.isFinished() && is_hurt == true) {
            _spriteRef->setScale(_original_scale);
            _spriteRef->setColor(sf::Color::White);
            is_hurt = false;
        } else {
            is_hurt = true;
            Vector2f scale = _spriteRef->getScale();
            scale.x += (paa::Random::rand() % 4 - 2) / 10.f;
            scale.y += (paa::Random::rand() % 4 - 2) / 10.f;
            _spriteRef->setScale(scale);
            _spriteRef->setColor(sf::Color::Red);
        }
    }

    void update_position()
    {
        auto axis = _controllerRef->getAxisXY();

        _positionRef.x += axis.x > 0 ? RTYPE_PLAYER_SPEED * PAA_DELTA_TIMER.getDeltaTime() : 0;
        _positionRef.x -= axis.x < 0 ? RTYPE_PLAYER_SPEED * PAA_DELTA_TIMER.getDeltaTime() : 0;
        _positionRef.y += axis.y > 0 ? RTYPE_PLAYER_SPEED * PAA_DELTA_TIMER.getDeltaTime() : 0;
        _positionRef.y -= axis.y < 0 ? RTYPE_PLAYER_SPEED * PAA_DELTA_TIMER.getDeltaTime() : 0;

        if (_frameTimer.isFinished()) {
            if (axis.y) {
                axis.y > 0 ? ++_y_frame : --_y_frame;
            } else {
                --_y_frame;
            }
        }
        _y_frame = std::clamp(_y_frame, 0, RTYPE_PLAYER_Y_FRAMES - 1);

        // TODO: Change sprite texture based on input
        // _spriteRef->useAnimation("frame_" + std::to_string(_y_frame));
    }

    void update()
    {
        update_position();
        update_shoot();
        update_data();
        update_sprite_hurt();
    }

    void on_collision(const paa::CollisionBox& other)
    {
        if (other.get_id() == CollisionType::POWER_UP) {
            // TODO: Add power up
            // This might be a good place to use a visitor pattern
        }
        if (other.get_id() == CollisionType::STATIC_WALL) {
            // TODO: Implement wall collision
        }

        const bool hurtable_object = other.get_id() == CollisionType::ENEMY_BULLET
                                    || other.get_id() == CollisionType::ENEMY;
        if (hurtable_object && !_is_hurt) {
            // Only local player can be hurt
            // Damage is calculated on server
            if (_is_local) 
                _healthRef.hp -= 1;
            _hurtTimer.restart();
        }
    }

    void add_shooter(Shooter& shooter)
    {
        _shooterList.push_back(shooter);
    }

    bool is_dead() const
    {
        return _healthRef.hp <= 0;
    }

    bool is_local() const
    {
        return _is_local;
    }
};

using Player = std::shared_ptr<APlayer>;

class PlayerFactory {
public:
    static void player_system()
    {
        PAA_ECS.register_component<Player>();
        PAA_ECS.add_system([](hl::silva::registry& r) {
            for (auto& [e, player, id] : r.view<Player, paa::Id>()) {
                player->update();
                if (player->is_dead()) {
                    // TODO: Send message to kill player By id
                    r.kill_entity(e);
                }
            }
        });
    }

    static void addPlayer(const net::PlayerID pid, paa::Controller& controller)
    {
        const paa::Position position(0, PAA_SCREEN.getSize().y / RTYPE_PLAYER_COUNT * id);
        const std::string texture = "player" + std::to_string(id);
        paa::DynamicEntity entity = PAA_NEW_ENTITY();
        paa::Position &p = entity.attachPosition(position);
        paa::Health &h = entity.attachHealth(paa::Health{RTYPE_PLAYER_MAX_HEALTH});
        paa::Sprite &s = entity.attachSprite(texture);

        s->setPosition(position.x, position.y);
        s->useAnimation("frame_0");
        entity.attachId(paa::Id{id});
        entity.attachCollision(CollisionFactory::makePlayerCollision(s->getGlobalBounds(), entity.get_id()));

        Player p = Player(new APlayer(entity.getEntity(), p, h, s, controller, pid == g_game.id));
        entity.insertComponent(std::move(p));
    }
};

}
}