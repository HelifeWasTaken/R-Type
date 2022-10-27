#pragma once

#include "Collisions.hpp"
#include "PileAA/DynamicEntity.hpp"
#include "PileAA/Rand.hpp"
#include "RServer/Messages/Types.hpp"
#include "Shooter.hpp"

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
#define RTYPE_PLAYER_SPEED paa::Vector2f(150.f, 100.f)
#endif

#ifndef RTYPE_PLAYER_FRAME_RATE
#define RTYPE_PLAYER_FRAME_RATE 50
#endif

#ifndef RTYPE_PLAYER_SPRITE_SCALE
#define RTYPE_PLAYER_SPRITE_SCALE paa::Vector2f(1, 1)
#endif

#ifndef RTYPE_SHOOT_BUTTON
// TODO: This might change depending on the controller
// or of the os
#define RTYPE_SHOOT_BUTTON paa::ControllerJoystick::XboxButton::Xbox_A
#endif

namespace rtype {
namespace game {

    class SerializablePlayer : public net::Serializable {
    public:
        using mask_t = uint16_t;
        using data_t = uint16_t;

        static constexpr mask_t MOVE_LEFT_MASK = 0b0000000000000001;
        static constexpr mask_t MOVE_LEFT_SHIFT = 0x0;

        static constexpr mask_t MOVE_RIGHT_MASK = 0b0000000000000010;
        static constexpr mask_t MOVE_RIGHT_SHIFT = 0x1;

        static constexpr mask_t MOVE_UP_MASK = 0b0000000000000100;
        static constexpr mask_t MOVE_UP_SHIFT = 0x2;

        static constexpr mask_t MOVE_DOWN_MASK = 0b0000000000001000;
        static constexpr mask_t MOVE_DOWN_SHIFT = 0x3;

        static constexpr mask_t MOVE_MASK_X_AXIS
            = MOVE_LEFT_MASK | MOVE_RIGHT_MASK;
        static constexpr mask_t MOVE_MASK_Y_AXIS
            = MOVE_UP_MASK | MOVE_DOWN_MASK;
        static constexpr mask_t MOVE_MASK = MOVE_MASK_X_AXIS | MOVE_MASK_Y_AXIS;

        static constexpr mask_t SHOOT_MASK = 0b0000000000010000;
        static constexpr mask_t SHOOT_SHIFT = 0x4;

        static constexpr mask_t PLAYER_MASK = 0b0000000001100000;
        static constexpr mask_t PLAYER_SHIFT = 0x5;

        static constexpr mask_t HEALTH_MASK = 0b0000000110000000;
        static constexpr mask_t HEALTH_SHIFT = 0x7;

        static constexpr mask_t UNUSED_MASK = 0b1111111000000000;
        static constexpr mask_t UNUSED_SHIFT = 0x9;

        data_t data = 0x0;
        net::vector2i pos = { 0, 0 };

        SerializablePlayer() = default;
        SerializablePlayer(const SerializablePlayer& other) = default;

        SerializablePlayer(const std::vector<net::Byte>& data)
        {
            from(data.data(), data.size());
        }

        SerializablePlayer(const PAA_ENTITY& entity)
        {
            set_from_entity(entity);
        }

        SerializablePlayer(const paa::Controller& controller,
            const paa::Position& position, const paa::Health& health,
            const paa::Id& id)
        {
            set_from_components(controller, position, health, id);
        }

        std::vector<net::Byte> serialize() const override
        {
            net::Serializer s;
            s << this->data;
            s.add_bytes(pos.serialize());
            return s.data;
        }

        void from(
            const net::Byte* data, const net::BufferSizeType size) override
        {
            net::Serializer s(data, size);
            s >> this->data;
            pos.from(s.data.data(), s.data.size());
        }

        void set_from_components(const paa::Controller& controller,
            const paa::Position& pos, const paa::Health& health,
            const paa::Id& id)
        {
            const paa::Vector2f xy = controller->getAxisXY();

            set_pos(pos)
                .set_hp(health.hp)
                .set_move_left(xy.x < -20.f)
                .set_move_right(xy.x > 20.f)
                .set_move_up(xy.y < -20.f)
                .set_move_down(xy.y > 20.f)
                .set_shoot(controller->isButtonPressed(0))
                .set_player(id.id);
        }

        void set_from_entity(const PAA_ENTITY& e)
        {
            paa::DynamicEntity entity = e;
            return set_from_components(entity.getComponent<paa::Controller>(),
                entity.getComponent<paa::Position>(),
                entity.getComponent<paa::Health>(),
                entity.getComponent<paa::Id>());
        }

        SerializablePlayer& set_bdata(const bool& value, mask_t mask)
        {
            data = value ? (data | mask) : (data & ~mask);
            return *this;
        }

        SerializablePlayer& set_move_left(bool value)
        {
            return set_bdata(value, MOVE_LEFT_MASK);
        }

        SerializablePlayer& set_move_right(bool value)
        {
            return set_bdata(value, MOVE_RIGHT_MASK);
        }

        SerializablePlayer& set_move_up(bool value)
        {
            return set_bdata(value, MOVE_UP_MASK);
        }

        SerializablePlayer& set_move_down(bool value)
        {
            return set_bdata(value, MOVE_DOWN_MASK);
        }

        SerializablePlayer& set_shoot(bool value)
        {
            return set_bdata(value, SHOOT_MASK);
        }

        SerializablePlayer& set_hp(const paa::u8& hp)
        {
            const data_t value = static_cast<data_t>(hp);

            data &= ~HEALTH_MASK;
            data |= (value << HEALTH_SHIFT) & HEALTH_MASK;
            return *this;
        }

        SerializablePlayer& set_player(net::PlayerID player)
        {
            const data_t value = static_cast<data_t>(player);

            data &= ~PLAYER_MASK;
            data |= (value << PLAYER_SHIFT) & PLAYER_MASK;
            return *this;
        }

        SerializablePlayer& set_pos(const paa::Position& pos)
        {
            this->pos = { static_cast<int>(pos.x) - g_game.scroll, static_cast<int>(pos.y) };
            return *this;
        }

        data_t get_data(const mask_t& mask, const mask_t& shift) const
        {
            return (data & mask) >> shift;
        }

        data_t get_move_left() const
        {
            return get_data(MOVE_LEFT_MASK, MOVE_LEFT_SHIFT);
        }

        data_t get_move_right() const
        {
            return get_data(MOVE_RIGHT_MASK, MOVE_RIGHT_SHIFT);
        }

        data_t get_move_up() const
        {
            return get_data(MOVE_UP_MASK, MOVE_UP_SHIFT);
        }

        data_t get_move_down() const
        {
            return get_data(MOVE_DOWN_MASK, MOVE_DOWN_SHIFT);
        }

        data_t get_shoot() const { return get_data(SHOOT_MASK, SHOOT_SHIFT); }

        data_t get_player() const
        {
            return get_data(PLAYER_MASK, PLAYER_SHIFT);
        }

        data_t get_hp() const { return get_data(HEALTH_MASK, HEALTH_SHIFT); }

        const net::vector2i get_pos() const { return net::vector2i(pos.x + g_game.scroll, pos.y); }

        bool data_is_same(const SerializablePlayer& other) const
        {
            return data == other.data;
        }

        bool pos_is_same(const SerializablePlayer& other) const
        {
            return pos == other.pos;
        }

        bool is_same(const SerializablePlayer& other) const
        {
            return data_is_same(other) && pos_is_same(other);
        }

        bool operator==(const SerializablePlayer& other) const
        {
            return is_same(other);
        }

        bool operator!=(const SerializablePlayer& other) const
        {
            return !is_same(other);
        }
    };

    class APlayer {
    protected:
        paa::DynamicEntity _entity;
        const paa::Id _id;

        paa::Sprite _spriteRef;
        paa::Controller _controllerRef;
        ShooterList _shooterList;

        SerializablePlayer _info;

        paa::Timer _syncTimer;
        paa::Timer _hurtTimer;
        paa::Timer _frameTimer;
        paa::Timer _yFrameUpdate;

        bool _is_hurt = false;

        bool _is_local; // is this player local or remote

        paa::Vector2f _original_scale;

        int _y_frame = 0;

    public:
        APlayer(const PAA_ENTITY& entity, const paa::Id& id,
            paa::Sprite& spriteRef, paa::Controller& controllerRef,
            bool is_local)
            : _entity(entity)
            , _id(id)
            , _spriteRef(spriteRef)
            , _controllerRef(controllerRef)
            , _is_local(is_local)
        {
            _syncTimer.setTarget(RTYPE_PLAYER_SYNC_RATE);
            _frameTimer.setTarget(RTYPE_PLAYER_FRAME_RATE);

            _original_scale = _spriteRef->getScale();
            use_frame();
        }

        ~APlayer() = default;

        void update_info(const SerializablePlayer& info)
        {
            auto& positionRef = _entity.getComponent<paa::Position>();
            auto& healthRef = _entity.getComponent<paa::Health>();

            _info = info;

            // positionRef = { (double)info.get_pos().x, (double)info.get_pos().y };
            healthRef.hp = info.get_hp();

            /*
            _controllerRef->simulateAxisMovement(paa::Joystick::Axis::X,
                (info.get_move_left() - info.get_move_right()) * 100.f);
            _controllerRef->simulateAxisMovement(paa::Joystick::Axis::Y,
                (info.get_move_down() - info.get_move_up()) * 100.f);
            */
            const auto minoffset = 10;
            const auto offsetx = info.get_pos().x - positionRef.x;
            const auto offsety = info.get_pos().y - positionRef.y;

            _controllerRef->simulateAxisMovement(paa::Joystick::Axis::X,
                offsetx < -minoffset ? 100 : offsetx > minoffset ? -100 : 0);

            _controllerRef->simulateAxisMovement(paa::Joystick::Axis::Y,
                offsety < -minoffset ? 100 : offsety > minoffset ? -100 : 0);

            info.get_shoot()
                ? _controllerRef->simulateButtonPress(RTYPE_SHOOT_BUTTON)
                : _controllerRef->simulateButtonIdle(RTYPE_SHOOT_BUTTON);
        }

        void update_shoot()
        {
            if (_controllerRef->isButtonPressed(0)) {
                for (auto& shooter : _shooterList)
                    shooter->shoot();
            }
        }

        void update_data()
        {
            // Sync player position and input with server
            // We only sync if the player is local
            // and if the timer is ready
            if (_is_local && _syncTimer.isFinished()) {
                SerializablePlayer info(_entity.getEntity());
                if (!info.data_is_same(_info)) {
                    g_game.service.udp().send(net::UpdateMessage(
                        _id.id, info, net::message_code::UPDATE_PLAYER));
                    _info = info;
                }
                _syncTimer.restart();
            }
        }

        void update_sprite_hurt()
        {
            if (_hurtTimer.isFinished() && _is_hurt == true) {
                _spriteRef->setScale(_original_scale);
                _spriteRef->setColor(sf::Color::White);
                _is_hurt = false;
            } else if (_is_hurt) {
                _is_hurt = true;
                paa::Vector2f scale = _spriteRef->getScale();
                scale.x += (paa::Random::rand() % 4 - 2) / 10.f;
                scale.y += (paa::Random::rand() % 4 - 2) / 10.f;
                _spriteRef->setScale(scale);
                _spriteRef->setColor(sf::Color::Red);
            }
        }

        void use_frame()
        {
            const std::string anim = std::to_string(_id.id) + "_" + std::to_string(_y_frame);
            _spriteRef->useAnimation(anim);
        }

        void update_position()
        {
            static const auto speed = RTYPE_PLAYER_SPEED;

            const double xspeed = speed.x * PAA_DELTA_TIMER.getDeltaTime();
            const double yspeed = speed.x * PAA_DELTA_TIMER.getDeltaTime();

            auto axis = _controllerRef->getAxisXY();
            auto& positionRef = _entity.getComponent<paa::Position>();

            positionRef.x -= axis.x() > 0 ? xspeed : 0;
            positionRef.x += axis.x() < 0 ? xspeed : 0;
            positionRef.y -= axis.y() > 0 ? yspeed : 0;
            positionRef.y += axis.y() < 0 ? yspeed : 0;

            positionRef.x = positionRef.x - g_game.old_scroll + g_game.scroll;

            if (_frameTimer.isFinished()) {
                _frameTimer.restart();
                axis.y() > 0 ? ++_y_frame : --_y_frame;
            }
            _y_frame = std::clamp(_y_frame, 0, RTYPE_PLAYER_Y_FRAMES - 1);
            use_frame();
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
            spdlog::warn("Player {} touched this {}", _id.id, other.get_id());

            if (other.get_id() == CollisionType::POWER_UP) {
                // TODO: Add power up
                // This might be a good place to use a visitor pattern
            }
            if (other.get_id() == CollisionType::STATIC_WALL) {
                spdlog::warn("Player {} is touching a wall!", _id.id);
                // TODO: Implement wall collision
            }

            /*
            const bool hurtable_object
                = other.get_id() == CollisionType::ENEMY_BULLET
                || other.get_id() == CollisionType::ENEMY;
            if (hurtable_object && !_is_hurt) {
                // Only local player can be hurt
                // Damage is calculated on server
                if (_is_local)
                    _healthRef.hp -= 1;
                _hurtTimer.restart();
                _is_hurt = true;
            }
            */
        }

        void add_shooter(Shooter& shooter) { _shooterList.push_back(shooter); }

        bool is_dead() const { return _entity.getComponent<paa::Health>().hp <= 0; }

        bool is_local() const { return _is_local; }
    };

    using Player = std::shared_ptr<APlayer>;

    class PlayerFactory {
    public:
        static PAA_ENTITY addPlayer(
            const net::PlayerID pid, paa::Controller& controller)
        {
            const paa::Position sposition(
                0, PAA_SCREEN.getSize().y / RTYPE_PLAYER_COUNT * pid);

            paa::DynamicEntity entity = PAA_NEW_ENTITY();
            const paa::Id& id = entity.attachId(paa::Id { pid });
            paa::Position& position = entity.attachPosition(sposition);
            paa::Health& health
                = entity.attachHealth(paa::Health { RTYPE_PLAYER_MAX_HEALTH });
            paa::Sprite& sprite = entity.attachSprite("player");

            sprite->setScale(RTYPE_PLAYER_SPRITE_SCALE);

            paa::Controller cmove = controller;
            entity.insertComponent(std::move(cmove));

            sprite->setPosition(position.x, position.y);

            Player player = Player(new APlayer(entity.getEntity(), id,
                sprite, controller, pid == g_game.id));

            // After creating the player, the sprite frame should be updated
            // So that the player has a the correct collision box
            Shooter shooter = make_shooter<BasicShooter>(entity.getEntity(), 0);
            player->add_shooter(shooter);
            const paa::FloatRect g_bounds(sprite->getGlobalBounds());
            const paa::IntRect irect(
                g_bounds.left, g_bounds.top, g_bounds.width, g_bounds.height);
            entity.attachCollision(
                CollisionFactory::makePlayerCollision(irect, entity.getId()));

            entity.insertComponent(std::move(player));
            return entity.getEntity();
        }
    };

}
}
