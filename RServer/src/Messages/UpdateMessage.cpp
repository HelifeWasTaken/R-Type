#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {
    UpdateMessage::UpdateMessage(
        const int16_t &sid, const Serializable& data)
        : Message(message_code::UPDATE_MESSAGE)
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

    uint16_t UpdateMessage::sid() const { return _sid; }

    const std::vector<uint8_t>& UpdateMessage::data() const { return _data; }

}
}