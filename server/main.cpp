/*
#include <iostream>
#include "Network/Server.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using namespace boost;


/*
void display_event(rtype::net::ServerEvent& event, rtype::net::Server& s)
{
    std::cout << "event coucou" << std::endl;
    if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_CONNECTION) {
        u_int8_t value = rtype::net::RFCMessage_TCP::CONN_OK;

        rtype::net::tcp_buffer_t buff = { value };
        s.write_tcp_socket(event.get_event<size_t>(), buff, 13);
    } else if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_DISCONNECTION) {
        std::cout << "Disconnection: " << event.get_event<size_t>() << std::endl;
    } else if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_MESSAGE) {
        auto& ref = event.get_event<rtype::net::ServerEvent::ServerMessageTCP>();
        std::cout << "TCP Message: " << ref.id << " -> "
                << std::string(ref.buffer.c_array(), ref.buffer.c_array() + ref.used) << std::endl;
    } else if (event.get_type() == rtype::net::ServerEvent::ServerEventType::UDP_MESSAGE) {
        auto& ref = event.get_event<rtype::net::ServerEvent::ServerMessageUDP>();
        rtype::net::udp_buffer_t toto("coucou");
        while(true) {
            std::cout << "hello" << std::endl;
            s.write_udp_socket(toto, 6);
        }
        std::cout << "UDP Message: " << ref.id << " -> "
                << std::string(ref.buffer.c_array(), ref.buffer.c_array() + ref.used) << std::endl;
    }
}

#include <assert.h>

int main(int ac, char **av)
{
    assert(ac == 3);
    rtype::net::Server s(atoi(av[1]), atoi(av[2]));

    while (s.is_running()) {
        rtype::net::ServerEvent event;
        while (s.poll(event)) {
            display_event(event, s);
        }
    }
    return 0;
}
*/

#include "Network/Server.hpp"
#include <iostream>

int main()
{
    boost::asio::io_context context;
    rtype::net::udp_server server(context, 4243);

    while (true) {
        rtype::net::udp_server::shared_message_info_t event;

        while (server.poll(event)) {
            std::cout << event.get()->size << std::endl;
        }
        context.run_one();
    }
    return 0;
}
