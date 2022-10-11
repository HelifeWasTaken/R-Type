#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    RequestConnectRoomReply::RequestConnectRoomReply(uint8_t playerID) :
                                    Message(message_code::CONNECT_ROOM_REQ_REP),
                                    _playerID(playerID)
    {
    }

    void RequestConnectRoomReply::from(const uint8_t *data, const size_t size)
    {
        Serializer s(data, size);
        s >> _message_code >> _playerID;
    }

    std::vector<uint8_t> RequestConnectRoomReply::serialize() const
    {
        Serializer s;
        s << _message_code << _playerID;
        return s.data;
    }

    uint8_t RequestConnectRoomReply::playerID() const
    {
        return _playerID;
    }

}
}