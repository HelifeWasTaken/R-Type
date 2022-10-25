//#include "Network/Server.hpp"
#include "RServer/Server/Server.hpp"
#include "poc.hpp"
#include <iostream>
#include <thread>
#include <unordered_set>

using namespace rtype::net;

static std::unordered_set<BufferSizeType> VALID_INDEXES;
static std::mutex mut;
static tcp_server* s;

static void __event_check()
{
    while (true) {
        tcp_event event;
        while (s->poll(event)) {
            std::lock_guard<std::mutex> lock(mut);
            if (event.get_type() == tcp_event_type::Connexion) {
                std::cout << "Connection: "
                          << event.get<tcp_event_connexion>().get_id()
                          << std::endl;
                VALID_INDEXES.insert(event.get<tcp_event_connexion>().get_id());
            } else if (event.get_type() == tcp_event_type::Disconnexion) {
                std::cout << "Disconnection: "
                          << event.get<tcp_event_disconnexion>().get_id()
                          << std::endl;
                VALID_INDEXES.erase(
                    event.get<tcp_event_disconnexion>().get_id());
            } else {
                std::cout << "Idc about messages bro" << std::endl;
            }
        }
    }
}

static void __write_async()
{
    while (true) {
        std::lock_guard<std::mutex> lock(mut);
        for (const auto& index : VALID_INDEXES)
            s->send(index, tcp_connection::new_message("ferocious ping\n"));
    }
}

void poc_server_write_tcp_example(void)
{
    boost::asio::io_context ctx;
    s = new rtype::net::tcp_server(ctx, 4242);

    std::thread t1(__event_check);
    std::thread t2(__write_async);

    ctx.run();

    t1.join();
    t2.join();
}