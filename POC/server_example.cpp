#include "RServer/Server/Server.hpp"
#include "poc.hpp"
#include <iostream>

static rtype::net::server* ref_s;

static void display_event(rtype::net::server::event& event)
{
    if (event.type == rtype::net::server::event_type::Connect) {
        spdlog::info("TCP: Connection: {}", event.client->id());
    } else if (event.type == rtype::net::server::event_type::Disconnect) {
        spdlog::info("TCP: Disconnection: {}", event.client->id());
    } else if (event.type == rtype::net::server::event_type::MainMessage) {
        spdlog::info("TCP Message: {} -> {}", event.client->id(),
            event.message->to_string());
        event.client->send_main("pong\n");
    } else if (event.type == rtype::net::server::event_type::FeedMessage) {
        // NOTE: for now, it WILL fail because the connection protocol is not
        // implemented
        spdlog::info("UDP Message: {} -> {}", event.client->id(),
            event.message->to_string());
        event.client->send_feed("pong\n");
    }
}

void poc_server_example()
{
    boost::asio::io_context context;
    rtype::net::server s(4242, 4343);

    ref_s = &s;
    while (true) {
        rtype::net::server::event event;
        while (s.poll(event)) {
            display_event(event);
        }
    }
}
