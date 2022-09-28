#include "poc.hpp"
#include "Network/Server.hpp"
#include <iostream>
#include <unordered_set>
#include <thread>

using namespace rtype::net;

static std::unordered_set<size_t> VALID_INDEXES;
static std::mutex mut;
static rtype::net::Server *s;

static void __event_check()
{
    while (s->is_running()) {
        rtype::net::ServerEvent event;
        while (s->poll(event)) {
            std::lock_guard<std::mutex> lock(mut);
            if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_CONNECTION) {
                std::cout << "Connection: " << event.get_event<size_t>() << std::endl;
                VALID_INDEXES.insert(event.get_event<size_t>());
            } else if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_DISCONNECTION) {
                std::cout << "Disconnection: " << event.get_event<size_t>() << std::endl;
                VALID_INDEXES.erase(event.get_event<size_t>());
            } else {
                std::cout << "Idc about messages bro" << std::endl;
            }
        }
    }
}

static void __write_async()
{
    tcp_buffer_t buffer("hello world\n");

    while (s->is_running()) {
        std::lock_guard<std::mutex> lock(mut);
        for (const auto& index : VALID_INDEXES)
            s->write_tcp_socket(index, buffer, 12);
    }
}

void poc_server_write_tcp_example(void)
{
    s = new rtype::net::Server(4242, 4243);

    std::thread t1(__event_check);
    std::thread t2(__write_async);

    t1.join();
    t2.join();
}
