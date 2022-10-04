#pragma once

#include <iostream>
#include "Server.hpp"

#define MAGIC_NUMBER 0x0fficecoffeedefec

namespace rtype {
namespace net {
namespace message {

    class IMessage {
        public:
            IMessage() = default;
            ~IMessage() = default;

        virtual IMessage& add_value(char byte) = 0;
    };

    class Message : public IMessage, public std::vector<char> {
        public:
            Message() = default;
            ~Message() = default;

        IMessage& add_value(char byte) override
        {
            push_back(byte);
            return *this;
        }
    };

    enum RFCMesssage_TCP {
        CONN_INIT,
        CONN_OK,
    };

    using shared_message_t = boost::shared_ptr<IMessage>;
}


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
        virtual bool poll(message::shared_message_t&) = 0;
    };

    class AClient : public IClient {
    public:
        AClient() = default;
        ~AClient() override = default;

        bool poll(message::shared_message_t& message) override final
        {
            return _queue.async_pop(message);
        }

    private:
        async_queue<message::shared_message_t> _queue;

    protected:
        void add_event(message::shared_message_t message)
        {
            _queue.async_push(message);
        }
    };

    class UDPClient : public AClient {
    private:
        boost::asio::ip::udp::resolver _resolver;
        boost::asio::ip::udp::resolver::query _query;
        boost::asio::ip::udp::endpoint _receiver_endpoint;
        boost::asio::ip::udp::socket _socket;
        boost::asio::ip::udp::endpoint _sender_endpoint;

    public:
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
            , _receiver_endpoint(*_resolver.resolve({host, port}))
            , _socket(io_context)
            , _sender_endpoint()
        {
            _socket.open(boost::asio::ip::udp::v4());
            // send("good world", 11);
        }

    private:
        size_t receive()
        {
            spdlog::info("UDPClient::receive: Started receiving");
            _socket.async_receive_from(
                boost::asio::buffer(*_buf_recv),
                _sender_endpoint,
                [this, buf_recv=_buf_recv](const boost::system::error_code& ec, size_t bytes) {
                    if (!ec) {
                        auto msg = parse_message(_buf_recv->c_array(), bytes);
                        if (msg == nullptr) {
                            spdlog::error("UDPClient::receive: Invalid message");
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

        size_t send(boost::shared_ptr<udp_buffer_t> message, size_t size=-1)
        {
            // return _socket.send_to(
            // boost::asio::buffer(data, size), _receiver_endpoint);

            spdlog::info("UDPClient::send: Sending message");
            if (size == (size_t)-1)
                size = message->size();
            _socket.async_send_to(
                boost::asio::buffer(*message, size),
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

        boost::shared_ptr<udp_buffer_t> _buf_recv;
    };

    class TCPClient : public IClient {
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
                                                        const char *port)
            : _resolver(io_context)
            , _query(host, "daytime")
            , _endpoint_iterator(_resolver.resolve({host, port}))
            , _socket(io_context)
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
        }

    private:
        size_t receive()
        {
            spdlog::info("TCPClient::receive: Start receiving");
            _socket.async_receive(
                boost::asio::buffer(*_buf_recv),
                [this, buf_recv=_buf_recv](const boost::system::error_code& ec, size_t bytes) {
                    if (!ec) {
                        auto msg = parse_message(_buf_recv->c_array(), bytes);
                        if (msg) {
                            spdlog::info("TCPClient::receive: Received {} bytes", bytes);
                            add_event(msg);
                        } else {
                            spdlog::error("TCPClient::receive: Invalid message");
                        }
                        receive();
                    } else {
                        spdlog::error("UDPClient::receive: {}", ec.message());
                    }
                }
            );
        }

        size_t send(boost::shared_ptr<tcp_buffer_t> message, size_t size=-1)
        {
            if (size == (size_t)-1)
                size = message->size();
            spdlog::info("TCPClient::send: Sending {} bytes", size);
            _socket.async_receive(
                boost::asio::buffer(*message, size),
                [this, message](const boost::system::error_code& ec, size_t bytes) {
                    (void)bytes;
                    if (!ec) {
                        spdlog::info("TCPClient::send: Sent {} bytes", bytes);
                    } else {
                        spdlog::error("UDPClient::receive: {}", ec.message());
                    }
                }
            );
        }

        boost::shared_ptr<tcp_buffer_t> _buf_recv;
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
                        const char* tcp_port, const char* udp_port)
            : _tcp_io_context(boost::asio::io_context())
            , _udp_io_context(boost::asio::io_context())
            , _udp_client(_udp_io_context, host_udp, udp_port)
            , _tcp_client(_tcp_io_context, host_tcp, tcp_port)
        {
        }

        /**
         * @brief Gets the UDP client
         * @return UDPClient & The UDP client
         */
        UDPClient& udp() { return _udp_client; }

        /**
         * @brief Gets the TCP client
         * @return TCPClient & The TCP client
         */
        TCPClient& tcp() { return _tcp_client; }
    };
}
}
