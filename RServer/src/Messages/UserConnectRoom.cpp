#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    UserConnectRoom::UserConnectRoom(uint8_t playerID)
        : Message(message_code::ROOM_CLIENT_CONNECT)
        , _playerID(playerID)
    {}

    void UserConnectRoom::from(const uint8_t *data, const size_t size) {
        Serializer s(data, size);
        s >> _message_code >> _playerID;
    }

    std::vector<uint8_t> UserConnectRoom::serialize() const {
        Serializer s;
        s << _message_code << _playerID;
        return s.data;
    }

    uint8_t UserConnectRoom::playerID() const { return _playerID; }

}
}