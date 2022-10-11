#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    CreateRoomReply::CreateRoomReply(const std::string& token) :
                                        _token(token),
                                        Message(message_code::CREATE_ROOM_REPLY)
    {
    }

    void CreateRoomReply::from(const uint8_t *data, const size_t size)
    {
        Serializer s(data, size);
        s >> _message_code;
        _token = std::string(s.data.begin(), s.data.end());
    }

    std::vector<uint8_t> CreateRoomReply::serialize() const
    {
        Serializer s;
        s << _message_code;
        s.add_bytes(_token);
        return s.data;
    }

    const std::string& CreateRoomReply::token() const
    {
        return _token;
    }

}
}