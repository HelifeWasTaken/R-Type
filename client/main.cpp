#include <SFML/Graphics.hpp>
#include <iostream>
#include <exception>
#include "Network/Server.hpp"

int main()
{
    try {
        rtype::net::UDP_TCP_Client client("127.0.0.1", "127.0.0.1", "4242", "4243");
        // sf::RenderWindow w(sf::VideoMode(800, 600), "SFML works!");
        rtype::net::tcp_buffer_t ptr;

        while (true) {
            std::cout << "first" << std::endl;
            size_t readed_bytes = client.udp().receive(ptr.data(), ptr.size());

            std::cout << "hello world" << ptr.data() << std::endl;
            // u_int8_t value = 0;

            // if (value = ptr[0]) {

            // }
            // std::cout << "hello world: " << ptr.data() << std::endl;
        }
    } catch(...) {
        std::cout << "RIP" << std::endl;
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
