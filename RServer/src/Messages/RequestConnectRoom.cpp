#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    RequestConnectRoom::RequestConnectRoom(const std::string& roomID)
        : Message(message_code::REQUEST_CONNECT_ROOM)
        , _roomID(roomID)
    {
    }

    void RequestConnectRoom::from(const uint8_t *data, const size_t size)
    {
        Serializer s(data, size);
        s >> _message_code;
        _roomID = std::string(s.data.begin(), s.data.end());
    }

    std::vector<uint8_t> RequestConnectRoom::serialize() const
    {
        Serializer s;
        s << _message_code;
        s.add_bytes(_roomID);
        return s.data;
    }

    const std::string& RequestConnectRoom::roomID() const
    {
        return _roomID;
    }
}
}