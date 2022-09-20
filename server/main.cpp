#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using namespace boost;
using namespace boost::asio::ip;

int connection_success(tcp::socket& socket,
                        tcp::endpoint ep,
                        system::error_code ec,
                        asio::io_service& service)
{
    std::cout << "[Server] Accepted a connection from client with ip address: " << socket.remote_endpoint().address().to_string() << std::endl;
    ec = socket.connect(ep, ec);
    if (!ec) {
        socket.send(boost::asio::buffer("hello world"));
        service.run();
    }
    return ec.value();
}

int main()
{
    asio::io_service service;
    using namespace boost::asio::ip;
    system::error_code ec;

    tcp::endpoint ep(tcp::v4(), 4242);
    tcp::acceptor acceptor(service, ep);

    while (1) {
        tcp::socket socket(service);
        ec = acceptor.accept(socket, ec);
        if (!ec) {
            connection_success(socket, ep, ec, service);
        } else {
            std::cout << "[Server] cannot connect : " << ec.message() << std::endl;
            return ec.value();
        }
    }
    return 0;
}
