#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    FeedInitReply::FeedInitReply(TokenType token)
        : Message(message_code::FEED_INIT_REP)
        , _token(token)
    {
    }

    void FeedInitReply::from(const Byte* data, const BufferSizeType size)
    {
        Serializer s(data, size);
        s >> _message_code >> _token;
    }

    std::vector<Byte> FeedInitReply::serialize() const
    {
        Serializer s;
        s << _message_code << _token;
        return s.data;
    }

    BufferSizeType FeedInitReply::size() const
    {
        return sizeof(_message_code) + sizeof(_token);
    }

    TokenType FeedInitReply::token() const { return _token; }

}
}