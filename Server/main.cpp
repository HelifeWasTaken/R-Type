#include "RServer/Messages/Types.hpp"
#include "RServer/Server/Server.hpp"
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <functional>

#define RTYPE_PLAYER_COUNT 4

class Room {
private:
    uint8_t _hostID = 0;

    std::array<bool, RTYPE_PLAYER_COUNT> _connected_players;
    std::unordered_map<uint8_t, int> _client_to_index;

    rtype::net::server& _server;

    using IMessage = rtype::net::IMessage;

public:
    Room(rtype::net::server& server, uint16_t client)
        : _server(server)
    {
        memset(_connected_players.data(), 0, _connected_players.size());
        _client_to_index[client] = 0;
        _connected_players[0] = true;
    }

    ~Room() = default;

    uint8_t getHostID() const { return _hostID; }
    void setHostID(uint8_t id) { _hostID = id; }

    uint8_t addPlayer(uint16_t client) {
        for (uint8_t i = 0; i < RTYPE_PLAYER_COUNT; i++) {
            if (!_connected_players[i]) {
                _connected_players[i] = true;
                _client_to_index[client] = i;
                return i;
            }
        }
        return RTYPE_INVALID_PLAYER_ID;
    }

    void removePlayer(uint16_t client) {
        auto index = _client_to_index[client];

        if (index == _hostID) {
            _hostID = RTYPE_INVALID_PLAYER_ID;
            for (uint8_t i = 0; i < RTYPE_PLAYER_COUNT; i++) {
                if (_connected_players[i]) {
                    _hostID = i;
                    break;
                }
            }
        }
        _connected_players[_client_to_index[client]] = false;
        _client_to_index.erase(client);
    }

    bool clientIsInRoom(uint16_t client) const {
        return _client_to_index.find(client) != _client_to_index.end();
    }

    bool isFull() const {
        return std::all_of(_connected_players.begin(),
                            _connected_players.end(),
                            [](bool b) { return b; });
    }

    bool isEmpty() const {
        return _hostID == RTYPE_INVALID_PLAYER_ID;
    }

    void main_broadcast(const IMessage& message) {
        auto buffer = message.serialize();
        for (auto& client : _client_to_index) {
            _server.get_client(client.first)->send_main(buffer.data(), buffer.size());
        }
    }

    void feed_broadcast(const IMessage& message) {
        auto buffer = message.serialize();
        for (auto& client : _client_to_index) {
            _server.get_client(client.first)->send_feed(buffer.data(), buffer.size());
        }
    }
};

class RoomManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Room>> _rooms;
    rtype::net::server& _server;

    std::unordered_map<uint16_t, std::string> _client_to_room_id;

public:
    RoomManager(rtype::net::server& server)
        : _server(server)
    {}

    ~RoomManager() = default;

public:
    void newRoom(uint16_t client) {
        std::string roomID = rtype::net::token::generate_token();
        while (_rooms.find(roomID) != _rooms.end()) {
            roomID = rtype::net::token::generate_token();
        }
        _rooms[roomID] = std::make_unique<Room>(_server, client);
        _client_to_room_id[client] = roomID;

        _server.get_client(client)->send_main(rtype::net::CreateRoomReply(roomID));
    }

    bool roomExists(const std::string& roomID) const {
        return _rooms.find(roomID) != _rooms.end();
    }

    void addPlayerToRoom(const std::string& roomID, uint16_t client) {
        if (_rooms.find(roomID) == _rooms.end()) {
            _server.get_client(client)->send_main(rtype::net::RequestConnectRoomReply(RTYPE_INVALID_PLAYER_ID));
            return;
        }
        uint16_t id = _rooms[roomID]->addPlayer(client);
        if (id != RTYPE_INVALID_PLAYER_ID) {
            _client_to_room_id[client] = roomID;
        }
        _server.get_client(client)->send_main(rtype::net::RequestConnectRoomReply(id));
    }

    void removePlayerIfInRoom(uint16_t client) {
        auto it = _client_to_room_id.find(client);
        if (it != _client_to_room_id.end()) {
            auto &room = _rooms[it->second];
            room->removePlayer(client);
            if (room->isEmpty()) {
                _rooms.erase(it->second);
            } else {
                room->main_broadcast(rtype::net::UserDisconnectFromRoom(client, room->getHostID()));
            }
        }
    }

};

class RTypeServer {
private:
    rtype::net::server _server;
    RoomManager _roomManager;

    std::unordered_map<
        rtype::net::message_code,
        std::function<void(uint16_t, rtype::net::IMessage&)>
    > _main_message_handlers = {
        {
            rtype::net::message_code::DUMMY,
            [](uint16_t client, rtype::net::IMessage& message) {
                spdlog::error("RTypeServer: Received DUMMY message");
            }
        },

        {
            rtype::net::message_code::CONN_INIT,
            [](uint16_t client, rtype::net::IMessage& message) {
                spdlog::info("RTypeServer: Received CONN_INIT message");
            }
        },

        {
            rtype::net::message_code::CREATE_ROOM,
            [this](uint16_t client, rtype::net::IMessage& message) {
                spdlog::info("RTypeServer: Received CREATE_ROOM message");
                this->_roomManager.newRoom(client);
            }
        },

        {
            rtype::net::message_code::CONN_INIT_REP,
            [](uint16_t client, rtype::net::IMessage& message) {
                spdlog::error("RTypeServer: Received CONN_INIT_REP message");
            }
        },

        {
            rtype::net::message_code::FEED_INIT,
            [](uint16_t client, rtype::net::IMessage& message) {
                spdlog::info("RTypeServer: Received FEED_INIT message");
            }
        },

        {
            rtype::net::message_code::FEED_INIT_REP,
            [](uint16_t client, rtype::net::IMessage& message) {
                spdlog::error("RTypeServer: Received FEED_INIT_REP message");
            }
        },

        {
            rtype::net::message_code::REQUEST_CONNECT_ROOM,
            [this](uint16_t client, rtype::net::IMessage& message) {
                spdlog::info("RTypeServer: Received REQUEST_CONNECT_ROOM message");
                auto msg = parse_message<rtype::net::RequestConnectRoom>(&message);
                if (msg) {
                    this->_roomManager.addPlayerToRoom(msg->roomID(), client);
                } else {
                    spdlog::error("RTypeServer: Failed to parse REQUEST_CONNECT_ROOM message");
                }
            }
        },

        {
            rtype::net::message_code::CONNECT_ROOM_REQ_REP,
            [](uint16_t client, rtype::net::IMessage& message) {
                spdlog::error("RTypeServer: Received REQUEST_CONNECT_ROOM_REP message");
            }
        },

        {
            rtype::net::message_code::ROOM_CLIENT_DISCONNECT,
            [](uint16_t client, rtype::net::IMessage& message) {
                spdlog::error("RTypeServer: Received USER_DISCONNECT_FROM_ROOM message");
            }
        }
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
                this->_main_message_handlers[event.message->code()](
                    event.client->id(),
                    *msg
                );
            }
        },

        {
            rtype::net::server::event_type::FeedMessage,
            [this](rtype::net::server::event& event) {
                spdlog::info("RTypeServer: Received Feed Message (non handeled yet)");
            }
        },

        {
            rtype::net::server::event_type::Connect,
            [](rtype::net::server::event& event) {
                spdlog::info("RTypeServer: Client connected");
            }
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