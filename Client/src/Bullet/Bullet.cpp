#include "Bullet.hpp"

namespace rtype {
namespace game {

    BulletQuery::BulletQuery(bullet_type_t type, net::PlayerID id)
        : type(type), id(id)
    {}

    BulletQuery::BulletQuery(const std::vector<Byte>& bytes)
    { from(bytes.data(), bytes.size()); }

    std::vector<net::Byte> BulletQuery::serialize(void) const override
    {
        rtype::net::Serializer s;
        s << id << type;
        return s.data;
    }

    void BulletQuery::from(const net::Byte *data, const net::BufferSizeType size) override
    {
        net::Serializer s(data, size);
        s >> id >> t;
    }

    Abullet::ABullet(const double lifeTime,
                    const BulletType type,
                    const double aim_angle,
                    paa::Position& posRef)
        : _type(type)
        , _posRef(_posRef)
        , _aim_angle(aim_angle)
    { _timer.setTarget(lifeTime); }

    bool ABullet::is_alive() const { return !_timer.isFinished(); }
    BulletType ABullet::get_type() const { return _type; }
    paa::Position& get_position() const { return _position; }
    double ABullet::get_aim_angle() const { return ai_angle; }

};
}