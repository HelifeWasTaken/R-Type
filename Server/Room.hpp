#include "RServer/Messages/Types.hpp"
#include "RServer/Server/Server.hpp"
#include <algorithm>
#include <unordered_map>
#include <functional>

class Room {
private:
    uint8_t _hostID = 0;

    std::array<bool, RTYPE_PLAYER_COUNT> _connected_players;
    std::unordered_map<uint8_t, int> _client_to_index;

    rtype::net::server& _server;

    bool _started = false;

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
        if (hasStarted()) {
            return RTYPE_INVALID_PLAYER_ID;
        }
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
        return _hostID == RTYPE_INVALID_PLAYER_ID || _client_to_index.empty();
    }

    bool launchGame() {
        if (isEmpty()) {
            return false;
        }
        _started = true;
        return true;
    }

    bool hasStarted() const {
        return _started;
    }

    bool stop() {
        if (isEmpty()) {
            return false;
        }
        _started = false;
        return true;
    }

    void main_broadcast(const rtype::net::IMessage& message, uint16_t except = 0xFFFF) {
        auto buffer = message.serialize();
        for (auto& client : _client_to_index) {
            if (client.first != except) {
                _server.get_client(client.first)->send_main(
                    buffer.data(), buffer.size());
            }
        }
    }

    void feed_broadcast(const rtype::net::IMessage& message, uint16_t except = 0xFFFF) {
        auto buffer = message.serialize();
        for (auto& client : _client_to_index) {
            if (client.first != except) {
                _server.get_client(client.first)->send_feed(
                    buffer.data(), buffer.size());
            }
        }
    }

    std::unordered_map<uint8_t, int> get_clients() const {
        return _client_to_index;
    }
};