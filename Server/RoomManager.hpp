#include "Room.hpp"

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

        for (auto& already_connected : _rooms[roomID]->get_clients()) {
            _server.get_client(already_connected.first)->send_main(
                rtype::net::UserConnectRoom(id)
            );
            if (already_connected.first == client) {
                continue;
            }
            _server.get_client(client)->send_main(
                rtype::net::UserConnectRoom(already_connected.second)
            );
        }
    }

    void removePlayerIfInRoom(uint16_t client) {
        auto it = _client_to_room_id.find(client);
        if (it != _client_to_room_id.end()) {
            auto &room = _rooms[it->second];
            room->removePlayer(client);
            if (room->isEmpty()) {
                spdlog::info("Room {} is empty, removing it", it->second);
                _rooms.erase(it->second);
            } else {
                room->main_broadcast(rtype::net::UserDisconnectFromRoom(client, room->getHostID()));
            }
        }
    }

    Room* getRoom(uint16_t client) {
        auto it = _client_to_room_id.find(client);
        if (it != _client_to_room_id.end()) {
            return _rooms[it->second].get();
        }
        return nullptr;
    }
};