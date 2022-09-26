#include <iostream>
#include "Network/Server.hpp"
/*
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using namespace boost;
*/


void display_event(rtype::net::ServerEvent& event)
{
    std::cout << "event coucuoo" << std::endl;
    if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_CONNECTION) {
        std::cout << "Connection: " << event.get_event<size_t>() << std::endl;
    } else if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_DISCONNECTION) {
        std::cout << "Disconnection: " << event.get_event<size_t>() << std::endl;
    } else if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_MESSAGE) {
        auto& ref = event.get_event<rtype::net::ServerEvent::ServerMessageTCP>();
        std::cout << "TCP Message: " << ref.id << " -> " << ref.buffer.c_array() << std::endl;
    }
}

int main()
{
    rtype::net::Server s(42, 43);

    s.start();
    while (s.is_running()) {
        rtype::net::ServerEvent event;
        while (s.poll(event)) {
            display_event(event);
        }
    }
    return 0;
}
