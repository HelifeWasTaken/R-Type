#include <SFML/Graphics.hpp>
#include <iostream>
#include <exception>
#include "Network/Server.hpp"

int main()
{
    rtype::net::UDP_TCP_Client client("127.0.0.1", "127.0.0.1", "4242", "4243");
    // sf::RenderWindow w(sf::VideoMode(800, 600), "SFML works!");
    rtype::net::tcp_buffer_t ptr;

    while (true) {
        size_t readed_bytes = client.tcp().receive(ptr.data(), ptr.size());
        u_int8_t value = 0;

        if (value = ptr.data()[0]) {
            if (value == rtype::net::RFCMessage_TCP::CONN_OK)
                std::cout << "You are connected ! " << std::endl;
        }
        // std::cout << "hello world: " << ptr.data() << std::endl;
    }
    // while (w.isOpen()) {
    //     sf::Event e;
    //     while (w.pollEvent(e)) {
    //         if (e.type == sf::Event::Closed)
    //             w.close();
    //     }
    //     w.clear();
    //     w.display();
    // }
    return 0;
}
