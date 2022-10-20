#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    ConnectionInitReply::ConnectionInitReply(PlayerID playerId, TokenType token)
        : Message(message_code::CONN_INIT_REP)
        , _playerId(playerId)
        , _token(token)
    {}

    void ConnectionInitReply::from(const Byte *data, const BufferSizeType size) {
        Serializer s(data, size);
        s >> _message_code >> _playerId >> _token;
    }

    std::vector<Byte> ConnectionInitReply::serialize() const {
        Serializer s;
        s << _message_code << _playerId << _token;
        return s.data;
    }

    BufferSizeType ConnectionInitReply::size() const {
        return sizeof(_message_code) + sizeof(_playerId) + sizeof(_token);
    }

    PlayerID ConnectionInitReply::playerId() const { return _playerId; }
    TokenType ConnectionInitReply::token() const { return _token; }

}
}