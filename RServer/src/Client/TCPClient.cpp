#include "RServer/Client/TCPClient.hpp"

namespace rtype {
namespace net {

    TCPClient::TCPClient(
        boost::asio::io_context& io_context, const char* host, const char* port)
        : _resolver(io_context)
        , _query(host, "daytime")
        , _endpoint_iterator(_resolver.resolve({ host, port }))
        , _socket(io_context)
        , _buf_recv(new tcp_buffer_t)
    {
        _is_connected = false;

        boost::system::error_code error = boost::asio::error::host_not_found;
        boost::asio::ip::tcp::resolver::iterator end;
        while (error && _endpoint_iterator != end) {
            _socket.close();
            _socket.connect(*_endpoint_iterator++, error);
        }

        if (error) {
            spdlog::warn("Error thrown by TCP client: {}", error.message());
            return;
        }

        _stopped = false;

        receive();

        auto msg = tcp_connection::new_message(
            SignalMarker(message_code::CONN_INIT));
        // CONN_INIT
        send(msg->buffer, msg->size);
    }

    void TCPClient::_add_event(shared_message_t message)
    {
        if (message->code() == message_code::CONN_INIT_REP) {
            auto msg = dynamic_cast<ConnectionInitReply*>(message.get());
            _id = msg->playerId();
            _token = msg->token();
            _is_connected = true;
            spdlog::info("TCPClient: Client is syncronised: Id({}) Token({})",
                _id, _token);
        } else {
            add_event(message);
        }
    }

    void TCPClient::receive()
    {
        spdlog::info("TCPClient::receive: Start receiving");
        _socket.async_receive(boost::asio::buffer(*_buf_recv),
            [this, buf_recv = _buf_recv](
                const boost::system::error_code& ec, BufferSizeType bytes) {
                if (!ec) {
                    spdlog::info(
                        "TCPClient::receive: Received {} bytes", bytes);
                    auto msg = parse_messages(_buf_recv->c_array(), bytes);
                    for (auto& m : msg) {
                        _add_event(m);
                    }
                    receive();
                } else {
                    spdlog::error("TCPClient::receive: {}", ec.message());
                    _stopped = true;
                }
            });
    }

    void TCPClient::send(
        boost::shared_ptr<tcp_buffer_t> message, BufferSizeType size)
    {
        if (size == (BufferSizeType)-1)
            size = message->size();
        spdlog::info("TCPClient::send: Sending {} bytes", size);
        _socket.async_send(boost::asio::buffer(*message, size),
            [this, message](
                const boost::system::error_code& ec, BufferSizeType bytes) {
                (void)bytes;
                if (!ec) {
                    spdlog::info("TCPClient::send: Sent {} bytes", bytes);
                } else {
                    spdlog::error("TCPClient::send: {}", ec.message());
                }
            });
    }

    void TCPClient::send(const tcp_buffer_t& message, BufferSizeType size)
    {
        send(boost::make_shared<tcp_buffer_t>(message), size);
    }

    void TCPClient::send(const IMessage& message)
    {
        auto msg = tcp_connection::new_message(message);
        send(msg->buffer, msg->size);
    }

    bool TCPClient::is_connected() const { return _is_connected; }

    TokenType TCPClient::token() const { return _token; }

    ClientID TCPClient::id() const { return _id; }

}
}
