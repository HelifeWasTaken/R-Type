/*
** EPITECH PROJECT, 2022
** B-CPP-500-RUN-5-1-rtype-mattis.dalleau
** File description:
** Client
*/

#include "Client.hpp"

namespace PAA {

    Client::Client(tcp::socket &socket) : _tcpSocket(socket)
    {

    }

    tcp::socket& Client::getTcpSocket() { return _tcpSocket; }

    void Client::sendMessage(std::string const &msg, tcp::socket& receiver)
    {
        boost::asio::write(receiver, boost::asio::buffer(msg));
    }

}
