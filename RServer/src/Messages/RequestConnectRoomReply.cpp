#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    RequestConnectRoomReply::RequestConnectRoomReply(PlayerID playerID) :
                                    Message(message_code::CONNECT_ROOM_REQ_REP),
                                    _playerID(playerID)
    {
    }

    void RequestConnectRoomReply::from(const Byte *data, const BufferSizeType size)
    {
        Serializer s(data, size);
        s >> _message_code >> _playerID;
    }

    std::vector<Byte> RequestConnectRoomReply::serialize() const
    {
        Serializer s;
        s << _message_code << _playerID;
        return s.data;
    }

    PlayerID RequestConnectRoomReply::playerID() const
    {
        return _playerID;
    }

}
}