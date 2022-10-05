#include <iostream>
/*
#include "Network/Server.hpp"
#include "Network/Client.hpp"
#include <SFML/Graphics.hpp>
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

int main()
{
    try {
        boost::asio::io_context context;
        rtype::net::UDPClient client(context, "127.0.0.1", "4242");

        while (true) {
            context.run_one();
            rtype::net::shared_message_t msg;
            while (client.poll(msg)) {
            }
        }
    } catch(...) {
        std::cout << "RIP" << std::endl;
    }
    return 0;
}
*/

#include "PileAA/App.hpp"
#include "PileAA/AnimatedSprite.hpp"

PAA_STATE(mystate) {

    PAA_START(mystate) {
        paa::ResourceManagerInstance::get().load<sf::Texture>("../assets/r-typesheet5.png", "image");
        PAA_ENTITY entity = PAA_NEW_ENTITY();
        PAA_SET_COMPONENT(entity, paa::Sprite, "image");
    }

    PAA_METHOD(my_method) {}

    PAA_END(mystate) {}
};

PAA_PROGRAM_START(mystate);
