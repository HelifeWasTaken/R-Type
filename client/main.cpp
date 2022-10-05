#include "Network/Server.hpp"
#include "Network/Client.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
/*
#include <iostream>
#include <exception>

int main()
{
    try {
        // rtype::net::UDP_TCP_Client client("127.0.0.1", "127.0.0.1", "4242", "4243");
        //
        boost::asio::io_context context;
        rtype::net::UDPClient client(context, "127.0.0.1", "13");

        // sf::RenderWindow w(sf::VideoMode(800, 600), "SFML works!");
        rtype::net::tcp_buffer_t ptr;

        while (true) {
            std::cout << "first" << std::endl;
            client.send("lol", 3);
            std::cout << "second" << std::endl;
            //size_t readed_bytes = client.udp().receive(ptr.data(), ptr.size());

            size_t readed_bytes = client.receive(ptr.data(), ptr.size());
            std::cout << "hello world" << ptr.data() << std::endl;
        }
    } catch(...) {
        std::cout << "RIP" << std::endl;
    }
    return 0;
}
*/

int main()
{
    try {
        boost::asio::io_context context;
        rtype::net::UDPClient client(context, "127.0.0.1", "4243");
        sf::RenderWindow w(sf::VideoMode(800, 600), "「R - タイプ」");

        while (w.isOpen()) {
            sf::Event event;
            while (w.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    client.send(rtype::net::udp_server::new_message(0, "closing window"));
                    w.close();
                }
            }
            client.send(rtype::net::udp_server::new_message(0, "closing window"));
            rtype::net::shared_message_t msg;
            while (client.poll(msg)) {
                auto mes = msg->serialize();
                spdlog::info("UDPClient::receive: {}", std::string(mes.begin(), mes.end()));
            }
            w.clear(sf::Color::Black);
            w.display();
            context.run_one();
        }
    } catch(...) {
        std::cout << "RIP" << std::endl;
    }
    return 0;
}
