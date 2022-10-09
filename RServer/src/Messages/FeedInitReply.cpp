#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    FeedInitReply::FeedInitReply(int32_t token)
        : Message(message_code::FEED_INIT_REP)
        , _token(token)
    {}

    void FeedInitReply::from(const uint8_t *data, const size_t size) {
        Serializer s(data, size);
        s >> _message_code >> _token;
    }

    std::vector<uint8_t> FeedInitReply::serialize() const {
        Serializer s;
        s << _message_code << _token;
        return s.data;
    }

    uint32_t FeedInitReply::token() const { return _token; }

}
}