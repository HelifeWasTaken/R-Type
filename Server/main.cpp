#include "Network/Messages.hpp"
#include "RServer/Server/Server.hpp"
#include <iostream>
#include <algorithm>

using namespace rtype::net;

class Room {
private:
    std::vector<int> _connected_clients;
    server& _server;

public:
    Room(server& server) : _server(server) {}

    void add_client(unsigned int id) {
        _connected_clients.push_back(id);
    }

    void remove_client(unsigned int id) {
        std::erase_if(_connected_clients, [id](unsigned int i) { return i == id; });
    }

    bool has_client(unsigned int id) {
        return std::find(_connected_clients.begin(), _connected_clients.end(), id)
                != _connected_clients.end();
    }

    void send_to_all_by_main(rtype::net::IMessage& message,
                            const std::vector<int>& except = {})
    {
        for (auto& client : _connected_clients) {
            const bool can_send = std::find(except.begin(), except.end(), client) == except.end();
            if (can_send)
                _server.get_client(client)->send_main(message);
        }
    }

    void send_to_all_by_feed(rtype::net::IMessage& message,
                            const std::vector<int>& except = {})
    {
        for (auto& client : _connected_clients) {
            const bool can_send = std::find(except.begin(), except.end(), client) == except.end();
            if (can_send)
                _server.get_client(client)->send_feed(message);
        }
    }
};

void handle_main_message(Room& room, server::event& event)
{
    if (event.message->code() == message_code::CONN_INIT) {
        spdlog::info("Server: Client {} connected adding to room", event.client->id());
        room.send_to_all_by_main(*event.message->to_msg());
        room.add_client(event.client->id());
    }
}

void handle_feed_message(Room& room, server::event& event)
{
    if (event.message->code() == message_code::SYNC_MSG) {
        spdlog::info("Server: Client {} sent a sync message", event.client->id());
        room.send_to_all_by_feed(*event.message->to_msg(), {event.client->id()});
    }
}

int main()
{
    server server(4242, 4243, true);
    Room room(server);

    while (true) {
        server::event event;
        if (server.poll(event)) {
            spdlog::info("Server: Event received: {}", event.type);
            if (event.type == server::event_type::FeedMessage)
                handle_feed_message(room, event);
            else if (event.type == server::event_type::MainMessage)
                handle_main_message(room, event);
            else if (event.type == server::event_type::Disconnect) {
                if (room.has_client(event.client->id())) {
                    spdlog::info("Server: Client {} disconnected removing from room", event.client->id());
                    room.remove_client(event.client->id());
                    DisconnectMessage msg(event.client->id());
                    room.send_to_all_by_main(msg);
                }
            }
        }
    }
    return 0;
}
