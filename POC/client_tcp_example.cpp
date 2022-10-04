#include <Network/Client.hpp>
#include <Network/Messages.hpp>
#include <poc.hpp>

using TCPClient = rtype::net::TCPClient;

static TCPClient* ref_c;

// a rework of this example is needed
void poc_client_tcp_example(void)
{
    boost::asio::io_context context;

    TCPClient c(context, "localhost", "4242");
    ref_c = &c;
    while (true) {
        context.run_one();
        boost::shared_ptr<rtype::net::tcp_buffer_t> message
            = boost::make_shared<rtype::net::tcp_buffer_t>();
        std::memcpy(message->data(), "Hello", 5);
        c.send(message);
        c.receive();
    }
}