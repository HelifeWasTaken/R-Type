#include "RServer/Messages/Types.hpp"
#include "RServer/Server/Server.hpp"
#include <iostream>
#include <algorithm>

using namespace rtype::net;

#define RTYPE_PLAYER_COUNT 4

class Room {
private:
    uint8_t _hostID = 0;

    std::array<bool, RTYPE_PLAYER_COUNT> _connected_players = {true, false, false, false};
    std::unordered_map<uint8_t, int> _client_to_index;

    rtype::net::server& _server;

public:
    Room(rtype::net::server& server, uint16_t client)
        : _server(server)
    {
        _client_to_index[client] = 0;
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
};

class RoomManager {
private:
    std::unordered_map<std::string, Room> _rooms;
public:
    RoomManager() = default;
    ~RoomManager() = default;

    std::string newRoom(uint16_t client) {
        std::string roomID = 
        _rooms[roomID] = Room(_server, client);
        return roomID;
    }

};

int main()
{
    server server(4242, 4243, true);
    while (true) {
        server::event event;
        server.poll(event);
    }
    return 0; 
}