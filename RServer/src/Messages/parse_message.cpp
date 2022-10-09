#include "RServer/Messages/Messages.hpp"

#include <unordered_map>

namespace rtype {
namespace net {

    // message_code to message_type
    static std::unordered_map<message_code, message_type> message_code_to_type_map = {
        {message_code::CONN_INIT, message_type::SIGNAL_MARKER},
        {message_code::CONN_INIT_REP, message_type::CONNECTION_INIT_REPLY},
        {message_code::FEED_INIT, message_type::FEED_INIT_REQUEST},
        {message_code::FEED_INIT_REP, message_type::FEED_INIT_REPLY},
        {message_code::SYNC_MESSAGE, message_type::SYNC_MESSAGE},
        {message_code::UPDATE_MESSAGE, message_type::UPDATE_MESSAGE},
        {message_code::TEXT_MESSAGE, message_type::TEXT_MESSAGE}
    };

    message_type message_code_to_type(const message_code& code) {
        return message_code_to_type_map[code];
    }

    message_code message_type_to_code(const message_type& type) {
        auto it = std::find_if(
            message_code_to_type_map.begin(), message_code_to_type_map.end(),
            [type](const std::pair<message_code, message_type>& p) {
                return p.second == type;
            });
        return message_code::DUMMY;
    }

    boost::shared_ptr<IMessage> parse_message(const uint8_t* buffer, size_t size)
    {
        assert(size > 0);

        const message_code code = static_cast<message_code>(buffer[0]);
        const message_type type = message_code_to_type(code);

        switch (type) {
        case message_type::UPDATE_MESSAGE:
            return Message::deserialize<UpdateMessage>(buffer, size);
        case message_type::SIGNAL_MARKER:
            return Message::deserialize<SignalMarker>(buffer, size);
        case message_type::SYNC_MESSAGE:
            return Message::deserialize<SyncMessage>(buffer, size);
        case message_type::CONNECTION_INIT_REPLY:
            return Message::deserialize<ConnectionInitReply>(buffer, size);
        case message_type::FEED_INIT_REQUEST:
            return Message::deserialize<FeedInitRequest>(buffer, size);
        case message_type::FEED_INIT_REPLY:
            return Message::deserialize<FeedInitReply>(buffer, size);
        case message_type::TEXT_MESSAGE:
            return Message::deserialize<TextMessage>(buffer, size);
        default:
            return nullptr;
        }
    }

    boost::shared_ptr<IMessage> parse_message(const std::vector<uint8_t>& buff)
    {
        return parse_message(buff.data(), buff.size());
    }
}
}