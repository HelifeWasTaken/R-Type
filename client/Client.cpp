#include "Network/Client.hpp"
#include "Network/Server.hpp"
#include <memory>

namespace rtype {
namespace net {


    //Abstract Client class implémentation
    bool AClient::poll(shared_message_t& message)
    {
        return _queue.async_pop(message);
    }

    void AClient::add_event(shared_message_t message)
    {
        return _queue.async_push(message);
    }

    void UDPClient::send(rtype::net::udp_server::shared_message_info_t message, size_t size);

    TCPClient::TCPClient(boost::asio::io_context& io_context, const char* host,
                                                            const char* port)
                                                    : _resolver(io_context)
                                                    , _query(host, "daytime")
                                                    , _endpoint_iterator(_resolver.resolve({ host, port }))
                                                    , _socket(io_context)
                                                    , _buf_recv(new tcp_buffer_t)
    {
        boost::system::error_code error
                = boost::asio::error::host_not_found;
        boost::asio::ip::tcp::resolver::iterator end;
        while (error && _endpoint_iterator != end) {
            _socket.close();
            _socket.connect(*_endpoint_iterator++, error);
        }

        if (error) {
            throw boost::system::system_error(error);
        }
        _tcp_connection_polling_thread = std::unique_ptr<boost::thread>(
            new boost::thread([this]() { receive(); }));
    }

    void TCPClient::receive()
    {
        spdlog::info("TCPClient::receive: Start receiving");
        _socket.async_receive(boost::asio::buffer(*_buf_recv),
            [this, buf_recv = _buf_recv](
                const boost::system::error_code& ec, size_t bytes) {
                if (!ec) {
                    auto msg = parse_message(
                        reinterpret_cast<uint8_t*>(_buf_recv->c_array()),
                        bytes);
                    if (msg) {
                        spdlog::info(
                            "TCPClient::receive: Received {} bytes", bytes);
                        spdlog::info("TCPClient::receive: {}",
                            std::string(_buf_recv->c_array(), bytes));
                        tcp_event(msg);
                    } else {
                        spdlog::error(
                            "TCPClient::receive: Invalid message");
                    }
                    receive();
                } else {
                    spdlog::error("UDPClient::receive: {}", ec.message());
                }
            }
        );
    }

    void TCPClient::send(boost::shared_ptr<tcp_buffer_t> message, size_t size=-1)
    {
        if (size == (size_t)-1)
            size = message->size();
        spdlog::info("TCPClient::send: Sending {} bytes", size);
        _socket.async_send(boost::asio::buffer(*message, size),
            [this, message](
                const boost::system::error_code& ec, size_t bytes) {
                (void)bytes;
                if (!ec) {
                    spdlog::info("TCPClient::send: Sent {} bytes", bytes);
                } else {
                    spdlog::error("UDPClient::send: {}", ec.message());
                }
            }
        );
    }

    UDP_TCP_Client::UDP_TCP_Client(const char* host_tcp, const char* host_udp,
                                        const char* tcp_port, const char* udp_port)
                                        : _tcp_io_context(boost::asio::io_context())
                                        , _udp_io_context(boost::asio::io_context())
                                        , _udp_client(_udp_io_context, host_udp, udp_port)
                                        , _tcp_client(_tcp_io_context, host_tcp, tcp_port)
    {
    }

    UDPClient& UDP_TCP_Client::udp()
    {
        return _udp_client;
    }


    TCPClient& UDP_TCP_Client::tcp()
    {
        return _tcp_client;
    }
}
}