#pragma once

#include "Server.hpp"
#include <iostream>

namespace rtype {
namespace net {

    using shared_message_t = boost::shared_ptr<IMessage>;

    class IClient {
    public:
        /**
         * @brief Creates a new client
         */
        IClient() = default;
        /**
         * @brief Destroys the client
         */
        virtual ~IClient() = default;

        /**
         * @brief Poll the client for messages
         * @param message::shared_message_t The message
         * @return bool True if a message was received
         */
        virtual bool poll(shared_message_t&) = 0;
    };

    class AClient : public IClient {
    public:
        AClient() = default;
        ~AClient() override = default;

        bool poll(shared_message_t& message) override final
        {
            return _queue.async_pop(message);
        }

    private:
        async_queue<shared_message_t> _queue;

    protected:
        void add_event(shared_message_t message) { _queue.async_push(message); }
    };

    class UDPClient : public AClient {
    private:
        boost::asio::ip::udp::resolver _resolver;
        boost::asio::ip::udp::resolver::query _query;
        boost::asio::ip::udp::endpoint _receiver_endpoint;
        boost::asio::ip::udp::socket _socket;
        boost::asio::ip::udp::endpoint _sender_endpoint;

    public:
        class HeaderMessage {
        public:
            HeaderMessage(udp_buffer_t& buffer)
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

            bool is_valid() const { return _magic == MAGIC_NUMBER; }

            std::size_t size() const
            {
                return sizeof(_magic) + sizeof(_seq) + sizeof(_id);
            }

            uint64_t get_msg_sequence() { return _seq; }

            uint16_t get_sender_id() { return _id; }

        private:
            uint64_t _magic;
            uint64_t _seq;
            uint16_t _id;
        };

        /**
         * @brief Creates a new UDP client
         * @param boost::asio::io_service & The io_service
         * @param const char * The host
         * @param const char * The port the client is listening to
         */
        UDPClient(boost::asio::io_context& io_context, const char* host,
                                                            const char* port)
                                    : _resolver(io_context)
                                    , _query(boost::asio::ip::udp::v4(), host, "daytime")
                                    , _receiver_endpoint(*_resolver.resolve({ host, port }))
                                    , _socket(io_context)
                                    , _sender_endpoint()

        {
            _socket.open(boost::asio::ip::udp::v4());
            _udp_connection_polling_thread = std::unique_ptr<boost::thread>(
                new boost::thread([this]() { receive(); }));
        }

        void receive()
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
                }
            }
        );
        }

        void send(rtype::net::udp_server::shared_message_info_t message, size_t size=-1)
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

    private:
        boost::shared_ptr<udp_buffer_t> _buf_recv;
        std::unique_ptr<boost::thread> _udp_connection_polling_thread;
    };

    class TCPClient : public AClient {
    private:
        boost::asio::ip::tcp::resolver _resolver;
        boost::asio::ip::tcp::resolver::query _query;
        boost::asio::ip::tcp::resolver::iterator _endpoint_iterator;
        boost::asio::ip::tcp::socket _socket;

    public:

        /**
         * @brief Creates a new TCP client
         * @param boost::asio::io_service & The io_service
         * @param const char * The host
         * @param const char * The port the client is listening to
         */
        TCPClient(boost::asio::io_context& io_context, const char* host,
            const char* port);

        void receive();

        void send(boost::shared_ptr<tcp_buffer_t> message, size_t size = -1);

    private:

        boost::shared_ptr<tcp_buffer_t> _buf_recv;
        std::unique_ptr<boost::thread> _tcp_connection_polling_thread;
    };

    class UDP_TCP_Client {
    private:
        boost::asio::io_context _tcp_io_context;
        boost::asio::io_context _udp_io_context;

        UDPClient _udp_client;
        TCPClient _tcp_client;

    public:
        /**
         * @brief Creates a new UDP_TCP_Client
         * @param const char * The tcp host
         * @param const char * The udp host
         * @param const char * The TCP port the socket is listening to
         * @param const char * The UDP port the socket is listening to
         */
        UDP_TCP_Client(const char* host_tcp, const char* host_udp,
            const char* tcp_port, const char* udp_port);

        /**
         * @brief Gets the UDP client
         * @return UDPClient & The UDP client
         */
        UDPClient& udp();

        /**
         * @brief Gets the TCP client
         * @return TCPClient & The TCP client
         */
        TCPClient& tcp();
    };
}
}
