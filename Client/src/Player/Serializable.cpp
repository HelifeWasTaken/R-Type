#include "Player.hpp"
#include "ClientScenes.hpp"

namespace rtype {
namespace game {

    SerializablePlayer::SerializablePlayer(const std::vector<net::Byte>& data)
    {
        from(data.data(), data.size());
    }

    SerializablePlayer::SerializablePlayer(const PAA_ENTITY& entity)
    {
        set_from_entity(entity);
    }

    SerializablePlayer::SerializablePlayer(const paa::Controller& controller,
        const paa::Position& position, const paa::Health& health,
        const paa::Id& id)
    {
        set_from_components(controller, position, health, id);
    }

    std::vector<net::Byte> SerializablePlayer::serialize() const
    {
        net::Serializer s;
        s << this->data;
        s.add_bytes(pos.serialize());
        return s.data;
    }

    void SerializablePlayer::from(
        const net::Byte* data, const net::BufferSizeType size)
    {
        net::Serializer s(data, size);
        s >> this->data;
        pos.from(s.data.data(), s.data.size());
    }

    void SerializablePlayer::set_from_components(
        const paa::Controller& controller, const paa::Position& pos,
        const paa::Health& health, const paa::Id& id)
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

    void SerializablePlayer::set_from_entity(const PAA_ENTITY& e)
    {
        paa::DynamicEntity entity = e;
        return set_from_components(entity.getComponent<paa::Controller>(),
            entity.getComponent<paa::Position>(),
            entity.getComponent<paa::Health>(), entity.getComponent<paa::Id>());
    }

    SerializablePlayer& SerializablePlayer::set_bdata(
        const bool& value, mask_t mask)
    {
        data = value ? (data | mask) : (data & ~mask);
        return *this;
    }

    SerializablePlayer& SerializablePlayer::set_move_left(bool value)
    {
        return set_bdata(value, MOVE_LEFT_MASK);
    }

    SerializablePlayer& SerializablePlayer::set_move_right(bool value)
    {
        return set_bdata(value, MOVE_RIGHT_MASK);
    }

    SerializablePlayer& SerializablePlayer::set_move_up(bool value)
    {
        return set_bdata(value, MOVE_UP_MASK);
    }

    SerializablePlayer& SerializablePlayer::set_move_down(bool value)
    {
        return set_bdata(value, MOVE_DOWN_MASK);
    }

    SerializablePlayer& SerializablePlayer::set_shoot(bool value)
    {
        return set_bdata(value, SHOOT_MASK);
    }

    SerializablePlayer& SerializablePlayer::set_hp(const paa::u8& hp)
    {
        const data_t value = static_cast<data_t>(hp);

        data &= ~HEALTH_MASK;
        data |= (value << HEALTH_SHIFT) & HEALTH_MASK;
        return *this;
    }

    SerializablePlayer& SerializablePlayer::set_player(net::PlayerID player)
    {
        const data_t value = static_cast<data_t>(player);

        data &= ~PLAYER_MASK;
        data |= (value << PLAYER_SHIFT) & PLAYER_MASK;
        return *this;
    }

    SerializablePlayer& SerializablePlayer::set_pos(const paa::Position& pos)
    {
        this->pos = { static_cast<int>(pos.x) - g_game.scroll,
            static_cast<int>(pos.y) };
        return *this;
    }

    SerializablePlayer::data_t SerializablePlayer::get_data(
        const mask_t& mask, const mask_t& shift) const
    {
        return (data & mask) >> shift;
    }

    SerializablePlayer::data_t SerializablePlayer::get_move_left() const
    {
        return get_data(MOVE_LEFT_MASK, MOVE_LEFT_SHIFT);
    }

    SerializablePlayer::data_t SerializablePlayer::get_move_right() const
    {
        return get_data(MOVE_RIGHT_MASK, MOVE_RIGHT_SHIFT);
    }

    SerializablePlayer::data_t SerializablePlayer::get_move_up() const
    {
        return get_data(MOVE_UP_MASK, MOVE_UP_SHIFT);
    }

    SerializablePlayer::data_t SerializablePlayer::get_move_down() const
    {
        return get_data(MOVE_DOWN_MASK, MOVE_DOWN_SHIFT);
    }

    SerializablePlayer::data_t SerializablePlayer::get_shoot() const
    {
        return get_data(SHOOT_MASK, SHOOT_SHIFT);
    }

    SerializablePlayer::data_t SerializablePlayer::get_player() const
    {
        return get_data(PLAYER_MASK, PLAYER_SHIFT);
    }

    SerializablePlayer::data_t SerializablePlayer::get_hp() const
    {
        return get_data(HEALTH_MASK, HEALTH_SHIFT);
    }

    const net::vector2i SerializablePlayer::get_pos() const
    {
        return net::vector2i(pos.x + g_game.scroll, pos.y);
    }

    bool SerializablePlayer::data_is_same(const SerializablePlayer& other) const
    {
        return data == other.data;
    }

    bool SerializablePlayer::pos_is_same(const SerializablePlayer& other) const
    {
        return pos == other.pos;
    }

    bool SerializablePlayer::is_same(const SerializablePlayer& other) const
    {
        return data_is_same(other) && pos_is_same(other);
    }

    bool SerializablePlayer::operator==(const SerializablePlayer& other) const
    {
        return is_same(other);
    }

    bool SerializablePlayer::operator!=(const SerializablePlayer& other) const
    {
        return !is_same(other);
    }
}
}