/*
** EPITECH PROJECT, 2022
** B-CPP-500-RUN-5-1-rtype-mattis.dalleau
** File description:
** Client
*/

#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using namespace boost;
using namespace boost::asio::ip;

namespace PAA {

    class Client {

        //Functions
        public:

            /**
             * @brief default constructor
             * @param  service: the current service the server is running with
             * @retval None
             */
            Client(tcp::socket &socket);

            /**
             * @brief  default destructor
             * @retval None
             */
            ~Client() = default;

            /**
             * @brief return the tcpSocket used by the client
             * @retval the tcp socket
             */
            tcp::socket& getTcpSocket();

            void sendMessage(std::string const &msg, tcp::socket& receiver);

        //Variables
        private:
            tcp::socket& _tcpSocket;

    };
}
