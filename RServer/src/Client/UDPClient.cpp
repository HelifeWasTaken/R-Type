#include "RServer/Client/UDPClient.hpp"

namespace rtype {
namespace net {

    UDPClient::HeaderMessage::HeaderMessage(udp_buffer_t& buffer)
    {
        unsigned char* msg
            = reinterpret_cast<unsigned char*>(buffer.c_array());
        std::memcpy(&_magic, msg, sizeof(uint64_t));
        std::memcpy(&_seq, msg + sizeof(uint64_t), sizeof(uint64_t));
        std::memcpy(
            &_id, msg + (2 * sizeof(uint64_t)), sizeof(uint16_t));
        _magic = boost::endian::big_to_native(_magic);
        _seq = boost::endian::big_to_native(_seq);
        _id = boost::endian::big_to_native(_id);
    }

    bool UDPClient::HeaderMessage::is_valid() const
    {
        return _magic == MAGIC_NUMBER;
    }

    std::size_t UDPClient::HeaderMessage::size() const
    {
        return sizeof(_magic) + sizeof(_seq) + sizeof(_id);
    }

    uint64_t UDPClient::HeaderMessage::get_msg_sequence() const
    {
        return _seq;
    }

    uint16_t UDPClient::HeaderMessage::get_sender_id() const
    {
        return _id;
    }

    UDPClient::UDPClient(boost::asio::io_context& io_context, const char* host,
                                                            const char* port)
                                    : _resolver(io_context)
                                    , _query(boost::asio::ip::udp::v4(), host, "daytime")
                                    , _receiver_endpoint(*_resolver.resolve({ host, port }))
                                    , _socket(io_context)
                                    , _sender_endpoint()
    {
        _socket.open(boost::asio::ip::udp::v4());
        _stopped = false;
        receive();
    }

    void UDPClient::receive()
    {
        spdlog::info("UDPClient::receive: Started receiving");
        _socket.async_receive_from(boost::asio::buffer(*_buf_recv),
        _sender_endpoint,
        [this, buf_recv = _buf_recv](
            const boost::system::error_code& ec, size_t bytes) {
            spdlog::info("UDPClient::receive: try to send message");
                if (!ec) {
                    HeaderMessage header(*buf_recv);
                    if (!header.is_valid()) {
                        spdlog::info("UDPClient::receive: Invalid Magic !");
                        return;
                    }
                    std::size_t header_size = header.size();
                    auto msg = parse_message(
                        reinterpret_cast<uint8_t*>(
                            _buf_recv->c_array() + header_size),
                        bytes - header_size);
                    if (msg == nullptr) {
                        spdlog::error(
                            "UDPClient::receive: Invalid message");
                    } else {
                        add_event(msg);
                    }
                    receive();
                } else {
                    spdlog::error("UDPClient::receive: {}", ec.message());
                    _stopped = true;
                }
            }
        );
    }

    void UDPClient::send(rtype::net::udp_server::shared_message_info_t message, size_t size)
    {
        spdlog::info("UDPClient::send: Sending message");
        if (size == (size_t)-1)
            size = message->size();
        _socket.async_send_to(
        boost::asio::buffer(message->msg(), size),
        _receiver_endpoint,
            [message](const boost::system::error_code& ec, size_t bytes) {
                (void)bytes;
                if (!ec) {
                    spdlog::info("UDPClient::send: Sent {} bytes", bytes);
                } else {
                    spdlog::error("UDPClient::receive: {}", ec.message());
                }
            }
        );
    }

}
}