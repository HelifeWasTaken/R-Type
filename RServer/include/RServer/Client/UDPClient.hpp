#pragma once

#include "RServer/Client/AClient.hpp"

namespace rtype {
namespace net {

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
            /**
             * @brief Construct a new Header Message object
             * 
             * @param buffer The buffer to read from
             */
            HeaderMessage(udp_buffer_t& buffer);

            /**
             * @brief Tells whether the loaded message is valid
             */
            bool is_valid() const;

            /**
             * @brief Gets the size of the header
             */
            std::size_t size() const;

            /**
             * @brief Gets the message sequence
             */
            uint64_t get_msg_sequence() const;

            /**
             * @brief Gets the sender id
             */
            uint16_t get_sender_id() const;

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
                                                            const char* port);

    private:
        /**
         * @brief Receive asynchronously a message
         */
        void receive();

    public:
        /**
         * @brief Send a message to the server asynchronously
         * @param shared_message_t The message to send
         * @param size The size of the message (if -1 size is udp_buffer_t::size)
         */
        void send(rtype::net::udp_server::shared_message_info_t message, size_t size=-1);

    private:
        boost::shared_ptr<udp_buffer_t> _buf_recv;
    };
}
}