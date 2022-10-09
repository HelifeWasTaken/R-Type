#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {
    SignalMarker::SignalMarker(const message_code& code)
        : Message(code)
    {}

    void SignalMarker::from(const uint8_t *data, const size_t size) {
        Serializer s(data, size);
        s >> _message_code;
    }

    std::vector<uint8_t> SignalMarker::serialize() const {
        Serializer s;
        s << _message_code;
        return s.data;
    }
}
}