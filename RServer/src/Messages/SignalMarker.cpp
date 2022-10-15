#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {
    SignalMarker::SignalMarker(const message_code& code)
        : Message(code)
    {}

    void SignalMarker::from(const Byte *data, const BufferSizeType size) {
        Serializer s(data, size);
        s >> _message_code;
    }

    std::vector<Byte> SignalMarker::serialize() const {
        Serializer s;
        s << _message_code;
        return s.data;
    }
}
}