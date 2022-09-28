#include <iostream>
#include "poc.hpp"
#include <Network/Server.hpp>
/*
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using namespace boost;
*/


static void display_event(rtype::net::ServerEvent& event)
{
    std::cout << "event coucou" << std::endl;
    if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_CONNECTION) {
        std::cout << "Connection: " << event.get_event<size_t>() << std::endl;
    } else if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_DISCONNECTION) {
        std::cout << "Disconnection: " << event.get_event<size_t>() << std::endl;
    } else if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_MESSAGE) {
        auto& ref = event.get_event<rtype::net::ServerEvent::ServerMessageTCP>();
        std::cout << "TCP Message: " << ref.id << " -> "
                << std::string(ref.buffer.c_array(), ref.buffer.c_array() + ref.used) << std::endl;
    } else if (event.get_type() == rtype::net::ServerEvent::ServerEventType::UDP_MESSAGE) {
        auto& ref = event.get_event<rtype::net::ServerEvent::ServerMessageUDP>();
        std::cout << "UDP Message: " << ref.id << " -> "
                << std::string(ref.buffer.c_array(), ref.buffer.c_array() + ref.used) << std::endl;
    }
}

void poc_server_example()
{
    rtype::net::Server s(5050, 5051);

    while (s.is_running()) {
        rtype::net::ServerEvent event;
        while (s.poll(event)) {
            display_event(event);
        }
    }
}
