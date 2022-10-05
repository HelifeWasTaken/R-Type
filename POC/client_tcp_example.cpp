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
    boost::shared_ptr<rtype::net::tcp_buffer_t> message
        = boost::make_shared<rtype::net::tcp_buffer_t>();
    rtype::net::shared_message_t recv_msg;
    while (true) {
        context.run_one();
        std::memcpy(message->data(), "Hello", 5);
        c.send(message, 5);
        while (c.poll(recv_msg)) {
            if (recv_msg->code() == rtype::net::message_code::TEXT_REP) {
                auto msg
                    = rtype::net::parse_message<rtype::net::TextReplyMessage>(
                        recv_msg);
                std::cout << "Received: " << msg->text() << std::endl;
            }
            std::memcpy(message->data(), "World", 5);
            c.send(message, 5);
        }
        c.receive();
    }
}