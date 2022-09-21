#include "Network/Server.hpp"

namespace PAA {

    Server::Server(asio::io_service& s, tcp::endpoint ep) : _endpoint(ep),
                                                            _service(s),
                                                            _acceptor(s, ep)
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

        return this->errorCode.value();
    }

    void Server::sendMessage(tcp::socket& clientSocket, std::string const &buffer)
    {
        clientSocket.send(boost::asio::buffer(buffer));
    }

    void Server::runService() { _service.run(); }
}