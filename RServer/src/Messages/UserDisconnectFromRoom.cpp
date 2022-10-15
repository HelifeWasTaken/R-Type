#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    UserDisconnectFromRoom::UserDisconnectFromRoom(PlayerID dc_user, PlayerID new_host) :
                            Message(message_code::ROOM_CLIENT_DISCONNECT),
                            _dc_user_id(dc_user),
                            _new_host_id(new_host)
    {
    }

    void UserDisconnectFromRoom::from(const Byte *data, const BufferSizeType size)
    {
        Serializer s(data, size);
        s >> _message_code >> _dc_user_id >> _new_host_id;
    }

    std::vector<Byte> UserDisconnectFromRoom::serialize() const
    {
        Serializer s;
        s << _message_code << _dc_user_id << _new_host_id;
        return s.data;
    }

    const PlayerID UserDisconnectFromRoom::get_disconnected_user_id() const
    {
        return _dc_user_id;
    }

    const PlayerID UserDisconnectFromRoom::get_new_host_id() const
    {
        return _new_host_id;
    }
}
}