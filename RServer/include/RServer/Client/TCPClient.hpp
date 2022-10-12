#pragma once

#include "RServer/Client/AClient.hpp"

namespace rtype {
namespace net {

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

        /**
         * @brief Sends a message to the server (async)
         * 
         * @param message The message to send
         * @param size The size of the message (if -1 size is tcp_buffer_t::size)
         */
        void send(boost::shared_ptr<tcp_buffer_t> message, size_t size = -1);

        /**
         * @brief Sends a message to the server (async)
         * @param message The message to send
         * @param size The size of the message (if -1 size is tcp_buffer_t::size)
         */
        void send(const tcp_buffer_t& message, size_t size = -1);

        /**
         * @brief Sends a message to the server (async)
         * 
         * @param message The message to send
         */
        void send(const IMessage& message);

        /**
         * @brief Tells you whether the client is sync with the server
         * 
         */
        bool is_connected() const;

        /**
         * @brief Gives you the client token
         */
        int32_t token() const;

        /**
         * @brief Gives you the client id
         */
        int16_t id() const;

    private:

        /**
         * @brief Receives a message from the server (async)
         */
        void receive();

        /**
         * @brief Gets the last event and parses it may chose to not add the event
         * if the server can handle it itself
         */
        void _add_event(shared_message_t message);

        boost::shared_ptr<tcp_buffer_t> _buf_recv;

        int32_t _token = -1;
        int16_t _id = -1;
        std::atomic_bool _is_connected;
    };

}
}