#include "RServer/Messages/Messages.hpp"

namespace rtype {
    namespace net {

        SyncMessage::SyncMessage(PlayerID sid, const Serializable& serializable, const message_code& code)
            : Message(code)
            , _sid(sid)
            , _data(serializable.serialize())
        {}

        void SyncMessage::from(const Byte *data, const BufferSizeType size) {
            Serializer s(data, size);
            s >> _message_code >> _sid;
            _data = s.data;
        }

        std::vector<Byte> SyncMessage::serialize() const {
            Serializer s;
            s << _message_code << _sid;
            s.add_bytes(_data);
            return s.data;
        }

        PlayerID SyncMessage::sid() const { return _sid; }

        const std::vector<Byte>& SyncMessage::data() const { return _data; }

    }
}