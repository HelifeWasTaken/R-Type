#include "RServer/Messages/Messages.hpp"

#include <iostream>

namespace rtype {
namespace net {

    CreateRoomReply::CreateRoomReply(const std::string& token)
        : _token(token)
        , Message(message_code::CREATE_ROOM_REPLY)
    {
    }

    void CreateRoomReply::from(const Byte* data, const BufferSizeType size)
    {
        Serializer s(data, size);

        s >> _message_code;
        _token = std::string(s.data.begin(), s.data.end());
    }

    std::vector<Byte> CreateRoomReply::serialize() const
    {
        Serializer s;
        s << _message_code;
        s.add_bytes(_token);
        return s.data;
    }

    BufferSizeType CreateRoomReply::size() const
    {
        return sizeof(_message_code) + _token.size();
    }

    const std::string& CreateRoomReply::token() const { return _token; }

}
}
