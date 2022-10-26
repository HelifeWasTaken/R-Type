#include "Bullet.hpp"

namespace rtype {
namespace game {

    BulletQuery::BulletQuery(bullet_type_t type, net::PlayerID id)
        : type(type)
        , id(id)
    {
    }

    BulletQuery::BulletQuery(const std::vector<net::Byte>& bytes)
    {
        from(bytes.data(), bytes.size());
    }

    std::vector<net::Byte> BulletQuery::serialize(void) const
    {
        rtype::net::Serializer s;
        s << id << type;
        return s.data;
    }

    void BulletQuery::from(
        const net::Byte* data, const net::BufferSizeType size)
    {
        net::Serializer s(data, size);
        s >> id >> type;
    }

    ABullet::ABullet(const PAA_ENTITY& e,
                     const BulletType type,
                     const double life_time,
                     const double aim_angle,
                     const double damage)
        : _e(e)
        , _type(type)
        , _aim_angle(aim_angle)
        , _damage(damage)
        , _destroyed_on_collision(true)
    {
        _timer.setTarget(life_time);
    }

    bool ABullet::is_alive() const { return !_timer.isFinished(); }
    BulletType ABullet::get_type() const { return _type; }
    paa::Position& ABullet::get_position() const { return PAA_GET_COMPONENT(_e, paa::Position); }
    double ABullet::get_aim_angle() const { return _aim_angle; }

};
}
