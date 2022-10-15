#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {
    UpdateMessage::UpdateMessage(
        const PlayerID &sid, const Serializable& data, const message_code& code)
        : Message(code)
        , _sid(sid)
        , _data(data.serialize())
    {}

    void UpdateMessage::from(const Byte *data, const BufferSizeType size) {
        Serializer s(data, size);
        s >> _message_code >> _sid;
        _data = s.data;
    }

    std::vector<Byte> UpdateMessage::serialize() const {
        Serializer s;
        s << _message_code << _sid;
        s.add_bytes(_data);
        return s.data;
    }

    PlayerID UpdateMessage::sid() const { return _sid; }

    const std::vector<Byte>& UpdateMessage::data() const { return _data; }

}
}