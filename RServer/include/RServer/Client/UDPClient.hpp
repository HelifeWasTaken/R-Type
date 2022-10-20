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

        std::atomic_bool _connected;
        TokenType _token;

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
            BufferSizeType size() const;

            /**
             * @brief Gets the sender id
             */
            ClientID get_sender_id() const;


        private:
            MagicNumber _magic;
            ClientID _id;
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

        /**
         * @brief Add a new event or handles it
         *
         * @param message The message to handle
         */
        void _add_event(shared_message_t& message);

    public:
        /**
         * @brief Send a message to the server asynchronously
         * @param shared_message_t The message to send
         * @param size The size of the message (if -1 size is udp_buffer_t::size)
         */
        void send(rtype::net::udp_server::shared_message_info_t message, BufferSizeType size=-1);

        /**
         * @brief Send a message to the server asynchronously
         * @param message The message to send
         */
        void send(const IMessage& message);

        /**
         * @brief Request connection init from feed
         *
         * @param token The token
         * @param playerId The player ID
         */
        void feed_request(TokenType token, ClientID playerId);

        /**
         * @brief Tells you wheter you are connected on the server or not
         */
        bool is_connected() const;

        /**
         * @brief Returns the token id of the user
         */
        TokenType token() const;

    private:
        boost::shared_ptr<udp_buffer_t> _buf_recv;
        ClientID _id;
    };
}
}
