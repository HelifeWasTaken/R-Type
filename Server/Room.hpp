#include "RServer/Messages/Types.hpp"
#include "RServer/Server/Server.hpp"
#include <algorithm>
#include <functional>
#include <unordered_map>

class Room {
private:
    rtype::net::PlayerID _hostID = 0;

    std::array<rtype::net::Bool, RTYPE_PLAYER_COUNT> _connected_players;
    std::unordered_map<rtype::net::ClientID, rtype::net::PlayerID>
        _client_to_index;

    rtype::net::server& _server;

    rtype::net::Bool _started = false;

    using IMessage = rtype::net::IMessage;

public:
    Room(rtype::net::server& server, rtype::net::ClientID client)
        : _server(server)
    {
        memset(_connected_players.data(), 0, _connected_players.size());
        _client_to_index[client] = 0;
        _connected_players[0] = true;
    }

    ~Room() = default;

    rtype::net::PlayerID getHostID() const { return _hostID; }
    void setHostID(rtype::net::PlayerID id) { _hostID = id; }

    rtype::net::PlayerID addPlayer(rtype::net::ClientID client)
    {
        if (hasStarted()) {
            return RTYPE_INVALID_PLAYER_ID;
        }
        for (rtype::net::PlayerID i = 0; i < RTYPE_PLAYER_COUNT; i++) {
            if (!_connected_players[i]) {
                _connected_players[i] = true;
                _client_to_index[client] = i;
                return i;
            }
        }
        return RTYPE_INVALID_PLAYER_ID;
    }

    void removePlayer(rtype::net::ClientID client)
    {
        auto index = _client_to_index[client];

        _connected_players[_client_to_index[client]] = false;
        _client_to_index.erase(client);

        if (index == _hostID) {
            _hostID = RTYPE_INVALID_PLAYER_ID;
            for (rtype::net::PlayerID i = 0; i < RTYPE_PLAYER_COUNT; i++) {
                if (_connected_players[i]) {
                    _hostID = i;
                    break;
                }
            }
        }
    }

    bool clientIsInRoom(rtype::net::ClientID client) const
    {
        return _client_to_index.find(client) != _client_to_index.end();
    }

    bool isFull() const
    {
        return std::all_of(_connected_players.begin(), _connected_players.end(),
            [](bool b) { return b; });
    }

    bool isEmpty() const
    {
        return _hostID == RTYPE_INVALID_PLAYER_ID || _client_to_index.empty();
    }

    bool launchGame()
    {
        if (isEmpty()) {
            return false;
        }
        _started = true;
        return true;
    }

    bool hasStarted() const { return _started; }

    bool stop()
    {
        if (isEmpty()) {
            return false;
        }
        _started = false;
        return true;
    }

    void main_broadcast(const rtype::net::IMessage& message,
        rtype::net::ClientID except = 0xFFFF)
    {
        auto buffer = message.serialize();
        for (auto& client : _client_to_index) {
            if (client.first != except) {
                _server.get_client(client.first)
                    ->send_main(buffer.data(), buffer.size());
            }
        }
    }

    void feed_broadcast(const rtype::net::IMessage& message,
        rtype::net::ClientID except = 0xFFFF)
    {
        auto buffer = message.serialize();
        for (auto& client : _client_to_index) {
            if (client.first != except) {
                _server.get_client(client.first)
                    ->send_feed(buffer.data(), buffer.size());
            }
        }
    }

    const std::unordered_map<rtype::net::ClientID, rtype::net::PlayerID>&
    get_clients() const
    {
        return _client_to_index;
    }

    rtype::net::PlayerID get_client_player_id(const rtype::net::ClientID& id) const
    {
        auto it = get_clients().find(id);

        if (it == get_clients().end())
            return RTYPE_INVALID_PLAYER_ID;
        return it->second;
    }
};
