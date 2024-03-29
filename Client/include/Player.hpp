#pragma once

#include "Collisions.hpp"
#include "PileAA/DynamicEntity.hpp"
#include "PileAA/Rand.hpp"
#include "RServer/Messages/Types.hpp"
#include "Shooter.hpp"

#ifndef RTYPE_SHOOT_BUTTON
// TODO: This might change depending on the controller
// or of the os
#define RTYPE_SHOOT_BUTTON paa::ControllerJoystick::XboxButton::Xbox_A
#endif

namespace rtype {
namespace game {

    class SerializablePlayer : public net::Serializable {
    public:
        using mask_t = uint8_t;
        using data_t = uint8_t;

        static constexpr mask_t MOVE_LEFT_MASK      = 0b00000001;
        static constexpr mask_t MOVE_LEFT_SHIFT     = 0x0;

        static constexpr mask_t MOVE_RIGHT_MASK     = 0b00000010;
        static constexpr mask_t MOVE_RIGHT_SHIFT    = 0x1;

        static constexpr mask_t MOVE_UP_MASK        = 0b00000100;
        static constexpr mask_t MOVE_UP_SHIFT       = 0x2;

        static constexpr mask_t MOVE_DOWN_MASK      = 0b00001000;
        static constexpr mask_t MOVE_DOWN_SHIFT     = 0x3;

        static constexpr mask_t SHOOT_MASK          = 0b00010000;
        static constexpr mask_t SHOOT_SHIFT         = 0x4;

        static constexpr mask_t HEALTH_MASK         = 0b11100000;
        static constexpr mask_t HEALTH_SHIFT        = 0x5;

        data_t data = 0x0;
        net::vector2i pos;

        SerializablePlayer() = default;
        SerializablePlayer(const SerializablePlayer& other) = default;

        SerializablePlayer(const std::vector<net::Byte>& data);

        SerializablePlayer(const PAA_ENTITY& entity);

        SerializablePlayer(const paa::Controller& controller,
            const paa::Position& position, const paa::Health& health,
            const paa::Id& id);

        std::vector<net::Byte> serialize() const override;

        void from(
            const net::Byte* data, const net::BufferSizeType size) override;

        void set_from_components(const paa::Controller& controller,
            const paa::Position& pos, const paa::Health& health,
            const paa::Id& id);

        void set_from_entity(const PAA_ENTITY& e);

        SerializablePlayer& set_bdata(const bool& value, mask_t mask);

        SerializablePlayer& set_move_left(bool value);
        SerializablePlayer& set_move_right(bool value);
        SerializablePlayer& set_move_up(bool value);
        SerializablePlayer& set_move_down(bool value);
        SerializablePlayer& set_shoot(bool value);
        SerializablePlayer& set_hp(const paa::u8& hp);
        SerializablePlayer& set_pos(const paa::Position& pos);
        data_t get_data(const mask_t& mask, const mask_t& shift) const;
        data_t get_move_left() const;
        data_t get_move_right() const;
        data_t get_move_up() const;
        data_t get_move_down() const;
        data_t get_shoot() const;
        data_t get_hp() const;
        const net::vector2i get_pos() const;

        bool data_is_same(const SerializablePlayer& other) const;
        bool pos_is_same(const SerializablePlayer& other) const;
        bool is_same(const SerializablePlayer& other) const;
        bool operator==(const SerializablePlayer& other) const;
        bool operator!=(const SerializablePlayer& other) const;
    };

    class APlayer {
    protected:
        paa::DynamicEntity _entity;
        const paa::Id _id;

        paa::Sprite _spriteRef;
        paa::Controller _controllerRef;
        ShooterList _shooterList;

        SerializablePlayer _info;

        bool _wallDeath;
        paa::Timer _wallDeathTimer;
        paa::Timer _syncWithTcpTimer;
        paa::Timer _syncTimer;
        paa::Timer _hurtTimer;
        paa::Timer _frameTimer;
        paa::Timer _yFrameUpdate;

        paa::Vec2 _lastCorrectPos;
        obscur::Vector2<float> _lastMoveVector;
        obscur::Vector2<float> _moveVector;

        bool _is_hurt = false;

        bool _clamp_position = true;

        int _speed_x;
        int _speed_y;

        bool _is_colliding_with_wall = false;

        bool _is_local; // is this player local or remote

        int _y_frame = 0;

        paa::Sound _hurt_sound =
            paa::Sound(PAA_RESOURCE_MANAGER.get<paa::SoundBuffer>("player_hurt"));

    public:
        static constexpr int WALL_DEATH_TIME = 500;
        static constexpr int MAX_HEALTH = 6;
        static constexpr int SYNC_RATE = 250;
        static constexpr int SYNC_WITH_TCP_RATE = 2000;
        static constexpr int Y_FRAMES = 4;
        static constexpr int SPEED_X = 200;
        static constexpr int SPEED_Y = 150;
        static constexpr int FRAME_RATE = 50;
        static constexpr int HURT_TIME = 800;

    public:
        APlayer(const PAA_ENTITY& entity, const paa::Id& id,
            paa::Sprite& spriteRef, paa::Controller& controllerRef,
            bool is_local);

        ~APlayer() = default;

        void update_info(const SerializablePlayer& info);
        void update_shoot();
        void update_data();
        void update_sprite_hurt();
        void use_frame();
        void update_position();
        void update();
        void set_clamp_position(bool clamp_position);
        void set_speed_x(int speed);
        void set_speed_y(int speed);
        int get_speed_x();
        int get_speed_y();
        void on_collision(const paa::CollisionBox& other);
        void add_shooter(Shooter shooter);
        bool is_dead() const;
        bool is_local() const;
    };

    using Player = std::shared_ptr<APlayer>;

    class PlayerFactory {
    public:
        static PAA_ENTITY addPlayer(
            const net::PlayerID pid, paa::Controller& controller, bool checkScreenBounds = true, bool isOnline = true);
    };

}
}
