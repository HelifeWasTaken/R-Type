
#include "RoomManager.hpp"

#define RTYPE_SERVER_ANY_SHOULD_NOT_HANDLE_THIS_CODE(type, code) \
    { \
        code, \
        [](uint16_t client, rtype::net::IMessage& message) { \
            spdlog::error("Server {}: Received message of type {} but should not handle it", type, #code); \
        } \
    }

#define RTYPE_SERVER_ANY_GOT_THIS_MESSAGE_INFO(type, code) \
    { \
        code, \
        [](uint16_t client, rtype::net::IMessage& message) { \
            spdlog::info("RTypeServer {}: Info: Received {} message", type, #code); \
        } \
    }

#define RTYPE_SERVER_ANY_HANDLE_THIS_MESSAGE(type, code, handler) \
    { \
        code, \
        [this](uint16_t client, rtype::net::IMessage& message) { \
            spdlog::info("RTypeServer {}: Received {} message", type, #code); \
            handler; \
        } \
    }

#define RTYPE_SERVER_MAIN_SHOULD_NOT_HANDLE_THIS_CODE(code) \
    RTYPE_SERVER_ANY_SHOULD_NOT_HANDLE_THIS_CODE("main", code)

#define RTYPE_SERVER_MAIN_GOT_THIS_MESSAGE_INFO(code) \
    RTYPE_SERVER_ANY_GOT_THIS_MESSAGE_INFO("main", code)

#define RTYPE_SERVER_MAIN_HANDLE_THIS_MESSAGE(code, handler) \
    RTYPE_SERVER_ANY_HANDLE_THIS_MESSAGE("main", code, handler)

#define RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(code) \
    RTYPE_SERVER_ANY_SHOULD_NOT_HANDLE_THIS_CODE("feed", code)

#define RTYPE_SERVER_FEED_GOT_THIS_MESSAGE_INFO(code) \
    RTYPE_SERVER_ANY_GOT_THIS_MESSAGE_INFO("feed", code)

#define RTYPE_SERVER_FEED_HANDLE_THIS_MESSAGE(code, handler) \
    RTYPE_SERVER_ANY_HANDLE_THIS_MESSAGE("feed", code, handler)

class RTypeServer {
private:
    rtype::net::server _server;
    RoomManager _roomManager;

    std::unordered_map<
        rtype::net::message_code,
        std::function<void(uint16_t, rtype::net::IMessage&)>
    > _main_message_handlers = {
        RTYPE_SERVER_MAIN_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::DUMMY),

        RTYPE_SERVER_MAIN_GOT_THIS_MESSAGE_INFO(rtype::net::message_code::CONN_INIT),
        RTYPE_SERVER_MAIN_HANDLE_THIS_MESSAGE(rtype::net::message_code::CREATE_ROOM, {
            this->_roomManager.newRoom(client);
        }),
        RTYPE_SERVER_MAIN_HANDLE_THIS_MESSAGE(rtype::net::message_code::LAUNCH_GAME, {
            this->_roomManager.launchGame(client);
        }),

        RTYPE_SERVER_MAIN_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::LAUNCH_GAME_REP),

        RTYPE_SERVER_MAIN_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::CONN_INIT_REP),
        RTYPE_SERVER_MAIN_GOT_THIS_MESSAGE_INFO(rtype::net::message_code::FEED_INIT),
        RTYPE_SERVER_MAIN_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::FEED_INIT_REP),

        RTYPE_SERVER_MAIN_GOT_THIS_MESSAGE_INFO(rtype::net::message_code::TEXT_MESSAGE),

        RTYPE_SERVER_MAIN_HANDLE_THIS_MESSAGE(rtype::net::message_code::REQUEST_CONNECT_ROOM, {
            auto msg = parse_message<rtype::net::RequestConnectRoom>(message);
            if (msg) {
                this->_roomManager.addPlayerToRoom(msg->roomID(), client);
            } else {
                spdlog::error("RTypeServer main: Failed to parse REQUEST_CONNECT_ROOM message");
            }
        }),

        RTYPE_SERVER_MAIN_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::CREATE_ROOM_REPLY),
        RTYPE_SERVER_MAIN_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::CONNECT_ROOM_REQ_REP),
        RTYPE_SERVER_MAIN_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::ROOM_CLIENT_DISCONNECT),

        // Thoses should be used in feed but are there for
        // completeness and debug
        RTYPE_SERVER_MAIN_HANDLE_THIS_MESSAGE(rtype::net::message_code::SYNC_VECTOR2_POSITION, {
            Room *room = this->_roomManager.getRoom(client);
            if (room) {
                room->main_broadcast(message, client);
            }
        }),

        RTYPE_SERVER_MAIN_HANDLE_THIS_MESSAGE(rtype::net::message_code::UPDATE_VECTOR2_MOVEMENT, {
            Room *room = this->_roomManager.getRoom(client);
            if (room) {
                room->main_broadcast(message, client);
            }
        })
    };

    std::unordered_map<
        rtype::net::message_code,
        std::function<void(uint16_t, rtype::net::IMessage&)>
    > _feed_message_handler = {
        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::DUMMY),

        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::CONN_INIT),
        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::CREATE_ROOM),
        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::LAUNCH_GAME),

        RTYPE_SERVER_FEED_GOT_THIS_MESSAGE_INFO(rtype::net::message_code::LAUNCH_GAME_REP),

        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::CONN_INIT_REP),
        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::FEED_INIT),
        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::FEED_INIT_REP),

        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::TEXT_MESSAGE),
        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::REQUEST_CONNECT_ROOM),
        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::CREATE_ROOM_REPLY),
        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::CONNECT_ROOM_REQ_REP),
        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::ROOM_CLIENT_CONNECT),
        RTYPE_SERVER_FEED_SHOULD_NOT_HANDLE_THIS_CODE(rtype::net::message_code::ROOM_CLIENT_DISCONNECT),

        RTYPE_SERVER_FEED_HANDLE_THIS_MESSAGE(rtype::net::message_code::SYNC_VECTOR2_POSITION, {
            Room *room = this->_roomManager.getRoom(client);
            if (room) {
                room->feed_broadcast(message, client);
            }
        }),
        RTYPE_SERVER_FEED_HANDLE_THIS_MESSAGE(rtype::net::message_code::UPDATE_VECTOR2_MOVEMENT, {
            Room *room = this->_roomManager.getRoom(client);
            if (room) {
                room->feed_broadcast(message, client);
            }
        })
    };

    std::unordered_map<
        rtype::net::server::event_type,
        std::function<void(rtype::net::server::event&)>
    > _events_types_handler = {
        {
            rtype::net::server::event_type::Invalid,
            [](rtype::net::server::event& event) {
                spdlog::error("RTypeServer: Invalid event received");
            }
        },

        {
            rtype::net::server::event_type::MainMessage,
            [this](rtype::net::server::event& event) {
                auto msg = event.message->to_msg();
                if (!msg) {
                    spdlog::error("RTypeServer: Failed to parse Main Message");
                    return;
                }
                this->_main_message_handlers[event.message->code()](event.client->id(), *msg);
            }
        },

        {
            rtype::net::server::event_type::FeedMessage,
            [this](rtype::net::server::event& event) {
                auto msg = event.message->to_msg();
                if (!msg) {
                    spdlog::error("RTypeServer: Failed to parse Feed Message");
                    return;
                }
                this->_feed_message_handler[event.message->code()](
                    event.client->id(),
                    *msg
                );
            }
        },

        {
            rtype::net::server::event_type::Connect,
            [](rtype::net::server::event& event)
            { spdlog::info("RTypeServer: Client connected"); }
        },

        {
            rtype::net::server::event_type::Disconnect,
            [this](rtype::net::server::event& event) {
                spdlog::info("RTypeServer: Client disconnected {}", event.client->id());
                this->_roomManager.removePlayerIfInRoom(event.client->id());
            }
        }
    };

public:
    RTypeServer(const int tcp_port, const int udp_port, bool authentificate)
        : _server(tcp_port, udp_port, authentificate)
        , _roomManager(_server)
    {}

    ~RTypeServer() = default;

    void run()
    {
        while (true) {
            rtype::net::server::event event;
            while (_server.poll(event)) {
                _events_types_handler[event.type](event);
            }
        }
    }
};


#include <PileAA/external/nlohmann/json.hpp>
#include <fstream>

int main()
{
    std::ifstream ifs("../Client.conf");
    nlohmann::json json;

    ifs >> json;

    RTypeServer(json["tcp_port"], json["udp_port"], json["authentificate"]).run();
    return 0; 
}