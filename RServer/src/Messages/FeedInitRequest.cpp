#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    FeedInitRequest::FeedInitRequest(ClientID playerId, TokenType token)
        : Message(message_code::FEED_INIT)
        , _playerId(playerId)
        , _token(token)
    {
    }

    void FeedInitRequest::from(const Byte *data, const BufferSizeType size) {
        Serializer s(data, size);
        s >> _message_code >> _playerId >> _token;
    }

    std::vector<Byte> FeedInitRequest::serialize() const {
        Serializer s;
        s << _message_code << _playerId << _token;
        return s.data;
    }

    ClientID FeedInitRequest::playerId() const { return _playerId; }

    TokenType FeedInitRequest::token() const { return _token; }

}
}