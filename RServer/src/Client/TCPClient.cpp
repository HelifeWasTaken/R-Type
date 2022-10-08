#include "RServer/Client/TCPClient.hpp"

namespace rtype {
namespace net {

    TCPClient::TCPClient(boost::asio::io_context& io_context, const char* host,
                                                            const char* port)
                                                    : _resolver(io_context)
                                                    , _query(host, "daytime")
                                                    , _endpoint_iterator(_resolver.resolve({ host, port }))
                                                    , _socket(io_context)
                                                    , _buf_recv(new tcp_buffer_t)
    {
        _is_connected = false;

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

        _stopped = false;

        receive();

        // CONN_INIT
        uint8_t byte = (uint8_t)rtype::net::message_code::CONN_INIT;
        send(tcp_connection::new_message(&byte, sizeof(byte))->buffer, 1);
    }

    void TCPClient::_add_event(shared_message_t message)
    {
        if (message->code() == message_code::CONN_INIT_REP) {
            auto msg = dynamic_cast<ConnectionInitReply *>(message.get());
            _id = msg->playerId();
            _token = msg->token();
            _is_connected = true;
            spdlog::info("TCPClient: Client is syncronised: Id({}) Token({})", _id, _token);
        } else {
            add_event(message);
        }
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
                        spdlog::info("TCPClient::receive: Received {} bytes", bytes);
                        _add_event(msg);
                    } else {
                        spdlog::error("TCPClient::receive: Invalid message");
                    }
                    receive();
                } else {
                    spdlog::error("UDPClient::receive: {}", ec.message());
                    _stopped = true;
                }
            }
        );
    }

    void TCPClient::send(boost::shared_ptr<tcp_buffer_t> message, size_t size)
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

    void TCPClient::send(const tcp_buffer_t& message, size_t size)
    {
        send(boost::make_shared<tcp_buffer_t>(message), size);
    }

}
}