#include "poc.hpp"
#include <Server.hpp>
#include <iostream>

static rtype::net::tcp_server* ref_s;

static void display_event(rtype::net::tcp_event& event)
{
    if (event.get_type() == rtype::net::tcp_event_type::Connexion) {
        spdlog::info("EventPoller: Connexion: {}",
            event.get<rtype::net::tcp_event_connexion>().get_id());
    } else if (event.get_type() == rtype::net::tcp_event_type::Disconnexion) {
        spdlog::info("EvenPoller: Disconnection: {}",
            event.get<rtype::net::tcp_event_disconnexion>().get_id());
    } else if (event.get_type() == rtype::net::tcp_event_type::Message) {
        auto& ref = event.get<rtype::net::tcp_event_message>();
        spdlog::info("TCP Message: {} -> {}", ref.get_id(),
            ref.get_message()->to_string());
        ref_s->send(ref.get_id(),
            rtype::net::tcp_connection::new_message(
                rtype::net::TextReplyMessage(0, "pong\n")));
    }
}

void poc_tcp_server_example()
{
    boost::asio::io_context context;
    rtype::net::tcp_server s(context, 4242);

    ref_s = &s;
    while (true) {
        context.run_one();
        rtype::net::tcp_event event;
        while (s.poll(event)) {
            display_event(event);
        }
    }
}
