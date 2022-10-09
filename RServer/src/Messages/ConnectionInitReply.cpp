#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    ConnectionInitReply::ConnectionInitReply(int16_t playerId, int32_t token)
        : Message(message_code::CONN_INIT_REP)
        , _playerId(playerId)
        , _token(token)
    {}

    void ConnectionInitReply::from(const uint8_t *data, const size_t size) {
        Serializer s(data, size);
        s >> _message_code >> _playerId >> _token;
    }

    std::vector<uint8_t> ConnectionInitReply::serialize() const {
        Serializer s;
        s << _message_code << _playerId << _token;
        return s.data;
    }

    uint16_t ConnectionInitReply::playerId() const { return _playerId; }
    uint32_t ConnectionInitReply::token() const { return _token; }

}
}