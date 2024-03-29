#include "RServer/Client/UDPClient.hpp"

namespace rtype {
namespace net {

    UDPClient::HeaderMessage::HeaderMessage(udp_buffer_t& buffer)
    {
        Serializer s(buffer.data(), buffer.size());

        s >> _magic >> _id;
    }

    bool UDPClient::HeaderMessage::is_valid() const
    {
        return _magic == RTYPE_MAGIC_NUMBER;
    }

    BufferSizeType UDPClient::HeaderMessage::size() const
    {
        return sizeof(_magic) + sizeof(_id);
    }

    ClientID UDPClient::HeaderMessage::get_sender_id() const { return _id; }

    UDPClient::UDPClient(
        boost::asio::io_context& io_context, const char* host, const char* port)
        : _resolver(io_context)
        , _query(boost::asio::ip::udp::v4(), host, "daytime")
        , _receiver_endpoint(*_resolver.resolve({ host, port }))
        , _socket(io_context)
        , _sender_endpoint()
        , _buf_recv(new udp_buffer_t)
    {
        _socket.open(boost::asio::ip::udp::v4());
        _socket.connect(_receiver_endpoint);
        _stopped = false;

        receive();
    }

    void UDPClient::feed_request(TokenType token, ClientID playerId)
    {
        auto msg = FeedInitRequest(playerId, token);
        auto shared_message = udp_server::new_message(playerId, msg);
        _id = playerId;
        this->send(shared_message, shared_message->size());
    }

    void UDPClient::_add_event(shared_message_t& message)
    {
        if (message->code() == message_code::FEED_INIT_REP) {
            auto feedinit = parse_message<FeedInitReply>(message);
            _token = feedinit->token();
            _connected = true;
            spdlog::info("UDPClient::receive: "
                         "You are Sync with the server now: token({})",
                feedinit->token());
        } else {
            add_event(message);
        }
    }

    void UDPClient::receive()
    {
        spdlog::info("UDPClient::receive: Started receiving");
        _socket.async_receive_from(boost::asio::buffer(*_buf_recv),
            _sender_endpoint,
            [this, buf_recv = _buf_recv](
                const boost::system::error_code& ec, BufferSizeType bytes) {
                spdlog::info(
                    "UDPClient::receive: readed a message of size: {}", bytes);
                if (ec) {
                    spdlog::error("UDPClient::receive: {}", ec.message());
                    _stopped = true;
                    return;
                }
                HeaderMessage header(*buf_recv);
                if (!header.is_valid()) {
                    spdlog::info("UDPClient::receive: Invalid Magic !");
                    return;
                }
                auto msg
                    = parse_message(reinterpret_cast<Byte*>(_buf_recv->c_array()
                                        + RTYPE_UDP_MESSAGE_HEADER),
                        bytes - RTYPE_UDP_MESSAGE_HEADER);
                if (msg == nullptr) {
                    spdlog::error("UDPClient::receive: Invalid message");
                } else {
                    _add_event(msg);
                }
                receive();
            });
    }

    void UDPClient::send(rtype::net::udp_server::shared_message_info_t message,
        BufferSizeType size)
    {
        spdlog::info("UDPClient::send: Sending message");
        if (size == (BufferSizeType)-1)
            size = message->size();
        _socket.async_send_to(boost::asio::buffer(message->msg(), size),
            _receiver_endpoint,
            [message](
                const boost::system::error_code& ec, BufferSizeType bytes) {
                (void)bytes;
                if (!ec) {
                    spdlog::info("UDPClient::send: Sent {} bytes", bytes);
                } else {
                    spdlog::error("UDPClient::receive: {}", ec.message());
                }
            });
    }

    void UDPClient::send(const IMessage& msg)
    {
        auto shared_message = udp_server::new_message(_id, msg);
        this->send(shared_message, shared_message->size());
    }

    bool UDPClient::is_connected() const { return _connected; }

    TokenType UDPClient::token() const { return _token; }

}
}
