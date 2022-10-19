#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    UserConnectRoom::UserConnectRoom(PlayerID playerID)
        : Message(message_code::ROOM_CLIENT_CONNECT)
        , _playerID(playerID)
    {}

    void UserConnectRoom::from(const Byte *data, const BufferSizeType size) {
        Serializer s(data, size);
        s >> _message_code >> _playerID;
    }

    std::vector<Byte> UserConnectRoom::serialize() const {
        Serializer s;
        s << _message_code << _playerID;
        return s.data;
    }

    BufferSizeType UserConnectRoom::size() const {
        return sizeof(_message_code) + sizeof(_playerID);
    }

    PlayerID UserConnectRoom::playerID() const { return _playerID; }

}
}