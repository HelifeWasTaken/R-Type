#include <iostream>
#include <string>
#include "Network/Server.hpp"

int connection_success(tcp::socket& socket, PAA::Server& server)
{
    std::cout << "[Server] Accepted a connection from client with ip address: " << socket.remote_endpoint().address().to_string() << std::endl;
    server.errorCode = socket.connect(server.getEndpoint(), server.errorCode);
    socket.send(boost::asio::buffer("hello world"));
    server.getService().run();
    return server.errorCode.value();
}

int main()
{
    PAA::Server server = PAA::Server(*new asio::io_service(), tcp::endpoint(make_address("127.0.0.1"), 4242));

    while (1) {
        tcp::socket socket(server.getService());

        int value = server.acceptClient(socket);
        std::cout << value << std::endl;
        if (value >= 0) {
            connection_success(socket, server);
        } else {
            std::cout << "[Server] cannot connect : " << server.errorCode.message() << std::endl;
            return server.errorCode.value();
        }
    }
    return 0;
}
