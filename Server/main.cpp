#include "Network/Messages.hpp"
#include "Server.hpp"
#include <iostream>

using namespace rtype::net;

int main()
{
    server server(4242, 4243, true);

    while (true) {
        server::event event;

        if (server.poll(event)) {
            if (event.type == server::event_type::MainMessage &&
                event.message->code() == rtype::net::message_code::CONN_INIT) {
                std::cout << "Client asking connection" << std::endl;
            }
        }
    }
    return 0;
}
