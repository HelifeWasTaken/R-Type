#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {
    UpdateMessage::UpdateMessage(
        const uint8_t &sid, const Serializable& data, const message_code& code)
        : Message(code)
        , _sid(sid)
        , _data(data.serialize())
    {}

    void UpdateMessage::from(const uint8_t *data, const size_t size) {
        Serializer s(data, size);
        s >> _message_code >> _sid;
        _data = s.data;
    }

    std::vector<uint8_t> UpdateMessage::serialize() const {
        Serializer s;
        s << _message_code << _sid;
        s.add_bytes(_data);
        return s.data;
    }

    uint8_t UpdateMessage::sid() const { return _sid; }

    const std::vector<uint8_t>& UpdateMessage::data() const { return _data; }

}
}