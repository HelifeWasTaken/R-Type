#include "RServer/Messages/Messages.hpp"

namespace rtype {
    namespace net {

        SyncMessage::SyncMessage(int16_t sid, const Serializable& serializable)
            : Message(message_code::SYNC_MESSAGE)
            , _sid(sid)
            , _data(serializable.serialize())
        {}

        void SyncMessage::from(const uint8_t *data, const size_t size) {
            Serializer s(data, size);
            s >> _message_code >> _sid;
            _data = s.data;
        }

        std::vector<uint8_t> SyncMessage::serialize() const {
            Serializer s;
            s << _message_code << _sid;
            s.add_bytes(_data);
            return s.data;
        }

        uint16_t SyncMessage::sid() const { return _sid; }

        const std::vector<uint8_t>& SyncMessage::data() const { return _data; }

    }
}