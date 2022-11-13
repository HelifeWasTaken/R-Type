#include "Room.hpp"

class RoomManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Room>> _rooms;
    rtype::net::server& _server;

    std::unordered_map<rtype::net::ClientID, std::string> _client_to_room_id;

public:
    RoomManager(rtype::net::server& server)
        : _server(server)
    {
    }

    ~RoomManager() = default;

public:
    void newRoom(rtype::net::ClientID client)
    {
        std::string roomID = rtype::net::token::generate_token();
        while (_rooms.find(roomID) != _rooms.end()) {
            roomID = rtype::net::token::generate_token();
        }
        _rooms[roomID] = std::make_unique<Room>(_server, client);
        _client_to_room_id[client] = roomID;

        _server.get_client(client)->send_main(rtype::net::CreateRoomReply(roomID));
    }

    bool roomExists(const std::string& roomID) const
    {
        return _rooms.find(roomID) != _rooms.end();
    }

    void addPlayerToRoom(const std::string& roomID, rtype::net::ClientID client)
    {
        if (_rooms.find(roomID) == _rooms.end()) {
            _server.get_client(client)->send_main(
                rtype::net::RequestConnectRoomReply(RTYPE_INVALID_PLAYER_ID));
            return;
        }
        rtype::net::ClientID id = _rooms[roomID]->addPlayer(client);
        if (id != RTYPE_INVALID_PLAYER_ID) {
            _client_to_room_id[client] = roomID;
        }

        _server.get_client(client)->send_main(
            rtype::net::RequestConnectRoomReply(id));

        for (auto& already_connected : _rooms[roomID]->get_clients()) {
            spdlog::info("RoomManager: Sending connection to {} that {} is connected",
                already_connected.first, id);
            _server.get_client(already_connected.first)
                ->send_main(rtype::net::UserConnectRoom(id));
            spdlog::info("RoomManager: Sending connection to {} that {} is connected",
                client, already_connected.second);
            _server.get_client(client)->send_main(
                rtype::net::UserConnectRoom(already_connected.second));
        }
    }

    void removePlayerIfInRoom(rtype::net::ClientID client)
    {
        auto it = _client_to_room_id.find(client);
        if (it != _client_to_room_id.end()) {
            spdlog::info("Client going to be removed from: {}", it->second);
            auto& room = _rooms.at(it->second);
            auto pid = room->get_client_player_id(client);
            room->removePlayer(client);
            if (room->isEmpty()) {
                spdlog::info("Room {} is empty, removing it", it->second);
                _rooms.erase(it->second);
            } else {
                room->main_broadcast(rtype::net::UserDisconnectFromRoom(
                    pid, room->getHostID()));
            }
            _client_to_room_id.erase(client);
        }
    }

    Room* getRoom(rtype::net::ClientID client)
    {
        auto it = _client_to_room_id.find(client);
        if (it != _client_to_room_id.end()) {
            return _rooms.at(it->second).get();
        }
        return nullptr;
    }

    void launchGame(rtype::net::ClientID client)
    {
        auto it = _client_to_room_id.find(client);
        if (it != _client_to_room_id.end()
            && _rooms.at(it->second)->launchGame()) {
            _rooms.at(it->second)->main_broadcast(
                rtype::net::GameLauncher(std::time(nullptr), true)
            );
        } else {
            _server.get_client(client)->send_main(
                rtype::net::GameLauncher(-1, false)
            );
        }
    }
};
