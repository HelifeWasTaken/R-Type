#include "Network/Server.hpp"
#include <iostream>

namespace PAA {

    Server::Server(asio::io_service& s, tcp::endpoint ep) : _endpoint(ep),
                                                            _service(s),
                                                            _acceptor(s, ep),
                                                            _tcpSocket(s)
    {
        _rawIpAddrs =  ep.address().to_string();
        _port = ep.port();
    }

    const std::string& Server::getRawIPAddrs() { return _rawIpAddrs; }

    address Server::getAddrs() { return make_address(_rawIpAddrs); }

    int Server::getPort() { return _port; }

    tcp::endpoint& Server::getEndpoint() { return _endpoint; }

    asio::io_service& Server::getService() { return _service; }

    int Server::acceptClient(tcp::socket& clientSocket)
    {
        this->errorCode = _acceptor.accept(clientSocket, this->errorCode);
        if (this->errorCode.value() == 0)
            _clientList.push_back(Client(clientSocket));
        return this->errorCode.value();
    }

    void Server::sendMessage(tcp::socket& clientSocket, std::string const &buffer)
    {
        clientSocket.send(boost::asio::buffer(buffer));
    }

    void Server::runService() { _service.run(); }

    tcp::socket& Server::getTcpSocket() { return _tcpSocket; }

    void Server::read()
    {
        boost::asio::async_read(_tcpSocket, boost::asio::buffer(_msgBuffer),
            [&] (system::error_code ec, std::size_t bytes_transferred) {
            std::cout << _msgBuffer << std::endl;
        });
    }

    std::vector<Client> Server::getClientsList()
    {
        return _clientList;
    }
}