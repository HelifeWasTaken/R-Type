#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using namespace boost;
using namespace boost::asio::ip;

namespace PAA {

class Server {

    //Functions
    public:

        /***
         * @brief default constructor
         * @param s: service
         * @param ep: information about server such has ip address and port number
         * @retval none
         */
        Server(asio::io_service& s, tcp::endpoint ep);

        /***
         * @brief default destructor
         */
        ~Server() = default;

    protected:

    public:

        /***
         * @brief Convert the current address from the asio::endpoint to string
         * @retval the converted address in const std::string format
         */
        const std::string& getRawIPAddrs();

        /***
         * @brief Return the address of the server
         * @retval Adresse of the server in ip::address format
         */
        address getAddrs();

        /***
         * @brief return the current port the server is listening tos
         * @retval the port
         */
        int getPort();

        /***
         * @brief return the service the server is currently using
         * @retval service
         */
        asio::io_service& getService();

        /**
         * @brief return the endtpoint of the server
         * @retval info about the server
         */
        tcp::endpoint& getEndpoint();

        /**
         * @brief accept the socket pass as parameter
         * @param  clientSocket: the socket which the cliet try to connect with
         * @retval 0 on success, -1 otherwise
         */
        int acceptClient(tcp::socket& clientSocket);

    //Variables
    public:
        system::error_code errorCode;

    private:
        tcp::acceptor _acceptor;
        tcp::endpoint _endpoint;
        asio::io_service& _service;
        std::string _rawIpAddrs = "";
        unsigned int _port = 0000;
};

}