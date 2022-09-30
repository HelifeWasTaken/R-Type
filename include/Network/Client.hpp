#pragma once

#define "Server.hpp"

#define MAGIC_NUMBER 0x0fficecoffeedefec

namespace rtype {
namespace net {

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
         * @brief Receive data from the server (Blocking)
         * @param void * The buffer
         * @param size_t The size of the buffer
         * @return size_t Number of bytes received
         */
        virtual size_t receive(void*, const size_t) = 0;

        /**
         * @brief Send data to the client (Blocking)
         * @param void * The buffer
         * @param size_t The size of the buffer
         * @return size_t Number of bytes sent
         */
        virtual size_t send(const void*, const size_t) = 0;
    };

    class UDPClient : public IClient {
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
            //send("hello world", 12);
        }

        size_t receive(void* data, const size_t size) override
        {
            return _socket.receive_from(
                boost::asio::buffer(data, size), _sender_endpoint);
        }

        size_t send(const void* data, const size_t size) override
        {
            return _socket.send_to(
                boost::asio::buffer(data, size), _receiver_endpoint);
        }
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

            if (_socket.is_open()) {
                char c = RFCMessage_TCP::CONN_INIT;
                send(&c, 1);
            }
            if (error) {
                throw boost::system::system_error(error);
            }
        }

        size_t receive(void* data, const size_t size) override
        {
            return _socket.receive(boost::asio::buffer(data, size));
        }

        size_t send(const void* data, const size_t size) override
        {
            return _socket.send(boost::asio::buffer(data, size));
        }
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
