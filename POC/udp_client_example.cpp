#include "poc.hpp"
#include "RServer/Client/Client.hpp"
#include <iostream>

void poc_udp_client_example(void)
{
    rtype::net::Client client("127.0.0.1", "127.0.0.1", "4242", "4243");
    rtype::net::shared_message_t s;

    while (true) {
        if (client.tcp().is_connected()) {

            client.udp().send(
                rtype::net::udp_server::new_message(
                    0,
                    rtype::net::FeedInitRequest(0, 42)
                )
            );

            while (client.udp().poll(s)) {
                std::cout << "Received: ";
                auto data = s->serialize();
                rtype::net::dump_memory(data.data(), data.size());
            }

        } else {
            client.tcp().poll(s); // wait for connection by polling anything
        }
        if (client.udp().stopped()) {
            spdlog::critical("UDP Client stopped");
            exit(1);
        }
        if (client.tcp().stopped()) {
            spdlog::critical("TCP Client stopped");
            exit(1);
        }
    }
}