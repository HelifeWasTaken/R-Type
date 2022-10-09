#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    FeedInitRequest::FeedInitRequest(int16_t playerId, int32_t token)
        : Message(message_code::FEED_INIT)
        , _playerId(playerId)
        , _token(token)
    {
    }

    void FeedInitRequest::from(const uint8_t *data, const size_t size) {
        Serializer s(data, size);
        s >> _message_code >> _playerId >> _token;
    }

    std::vector<uint8_t> FeedInitRequest::serialize() const {
        Serializer s;
        s << _message_code << _playerId << _token;
        return s.data;
    }

    uint16_t FeedInitRequest::playerId() const { return _playerId; }

    uint32_t FeedInitRequest::token() const { return _token; }

}
}