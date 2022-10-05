#include "Network/Server.hpp"
#include "Network/Messages.hpp"
#include <iostream>

using namespace rtype::net;

int main()
{
    server server(4242, 4243);

    while (true) {
        server::event event;

        if (server.poll(event)) {
            if (event.type == server::event_type::MainMessage
                && event.message->code() == message_code::SYNC_MSG) {
                
                auto msg = parse_message<UpdateMessage>(event.message->to_vec());

                for (auto& it : server.clients()) {
                    auto& id = it.first;
                    auto& client = it.second;
                    if (id != event.client->id())
                        client->send_main(*msg);
                }
            }
        }
    }
    return 0;
}