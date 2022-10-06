#include "poc.hpp"
#include <Server.hpp>
#include <iostream>

static rtype::net::udp_server* ref_s;

void poc_udp_server_example()
{
    boost::asio::io_context context;
    rtype::net::udp_server s(context, 4243);

    ref_s = &s;
    while (true) {
        context.run_one();
        rtype::net::udp_server::shared_message_info_t msg;

        while (s.poll(msg)) {
            spdlog::info("UDP Message: {}", msg->to_string());
            ref_s->send_to(
                msg->sender(), rtype::net::udp_server::new_message(0, "pong\n"));
        }
    }
}