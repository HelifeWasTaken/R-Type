#include "RServer/Messages/Messages.hpp"

#include <iostream>
#include <spdlog/spdlog.h>
#include <unordered_map>

namespace rtype {
namespace net {

    namespace token {
        std::string generate_token(void)
        {
            std::string available = "azertyuiopqsdfghjklmwxcvbn";
            std::string token = "#";

            for (size_t size = 0; size < RTYPE_TOKEN_SIZE; size++) {
                std::size_t index = std::rand() % available.size();
                token += available[index];
            }
            return token;
        }
    }

    // message_code to message_type
    static std::unordered_map<message_code, message_type>
        message_code_to_type_map = {
            // Signal Markers
            { message_code::CONN_INIT, message_type::SIGNAL_MARKER },
            { message_code::CREATE_ROOM, message_type::SIGNAL_MARKER },
            { message_code::LAUNCH_GAME, message_type::SIGNAL_MARKER },

            // YesNo messages
            { message_code::LAUNCH_GAME_REP, message_type::YES_NO_MESSAGES },

            // Special messages
            { message_code::CONN_INIT_REP,
                message_type::CONNECTION_INIT_REPLY },
            { message_code::FEED_INIT, message_type::FEED_INIT_REQUEST },
            { message_code::FEED_INIT_REP, message_type::FEED_INIT_REPLY },

            { message_code::TEXT_MESSAGE, message_type::TEXT_MESSAGE },
            { message_code::REQUEST_CONNECT_ROOM,
                message_type::REQUEST_CONNECT_ROOM },
            { message_code::CONNECT_ROOM_REQ_REP,
                message_type::CONNECT_ROOM_REQ_REP },
            { message_code::CREATE_ROOM_REPLY,
                message_type::CREATE_ROOM_REPLY },
            { message_code::ROOM_CLIENT_DISCONNECT,
                message_type::ROOM_CLIENT_DISCONNECT },
            { message_code::ROOM_CLIENT_CONNECT,
                message_type::ROOM_CLIENT_CONNECT },

            // Sync messages
            { message_code::SYNC_PLAYER, message_type::SYNC_MESSAGE },

            // Update messages
            { message_code::UPDATE_PLAYER, message_type::UPDATE_MESSAGE },
            { message_code::UPDATE_ENEMY_DESTROYED,
                message_type::UPDATE_MESSAGE },
            { message_code::UPDATE_PLAYER_DESTROYED,
                message_type::UPDATE_MESSAGE }
        };

    message_type message_code_to_type(const message_code& code)
    {
        return message_code_to_type_map[code];
    }

    message_code message_type_to_code(const message_type& type)
    {
        auto it = std::find_if(message_code_to_type_map.begin(),
            message_code_to_type_map.end(),
            [type](const std::pair<message_code, message_type>& p) {
                return p.second == type;
            });
        return message_code::DUMMY;
    }

    boost::shared_ptr<IMessage> parse_message(
        const Byte* buffer, BufferSizeType size)
    {
        try {
            assert(size > 0);

            const message_code code = static_cast<message_code>(buffer[0]);
            const message_type type = message_code_to_type(code);

            switch (type) {
            case message_type::UPDATE_MESSAGE:
                return Message::deserialize<UpdateMessage>(buffer, size);
            case message_type::SIGNAL_MARKER:
                return Message::deserialize<SignalMarker>(buffer, size);
            case message_type::YES_NO_MESSAGES:
                return Message::deserialize<YesNoMarker>(buffer, size);
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
            case message_type::CONNECT_ROOM_REQ_REP:
                return Message::deserialize<RequestConnectRoomReply>(
                    buffer, size);
            case message_type::REQUEST_CONNECT_ROOM:
                return Message::deserialize<RequestConnectRoom>(buffer, size);
            case message_type::CREATE_ROOM_REPLY:
                return Message::deserialize<CreateRoomReply>(buffer, size);
            case message_type::ROOM_CLIENT_CONNECT:
                return Message::deserialize<UserConnectRoom>(buffer, size);
            case message_type::ROOM_CLIENT_DISCONNECT:
                return Message::deserialize<UserDisconnectFromRoom>(
                    buffer, size);
            default:
                spdlog::error("Unknown message type of code/type: {}/{}",
                    (int)code, (int)type);
            }
        } catch (...) {
            spdlog::error("Error while parsing message");
        }
        return nullptr;
    }

    std::vector<boost::shared_ptr<IMessage>> parse_messages(
        const Byte* buffer, BufferSizeType size)
    {
        std::vector<boost::shared_ptr<IMessage>> messages;
        BufferSizeType offset = 0;

        while (offset < size) {
            const message_code code = static_cast<message_code>(buffer[offset]);
            const message_type type = message_code_to_type(code);

            auto msg = parse_message(buffer + offset, size - offset);
            if (msg == nullptr) {
                spdlog::error("MessageList::parse_messages: Message is null "
                              "skipping everything else");
                return messages;
            }
            messages.push_back(msg);
            offset += msg->size();
        }
        return messages;
    }

    boost::shared_ptr<IMessage> parse_message(const std::vector<Byte>& buff)
    {
        return parse_message(buff.data(), buff.size());
    }
}
}