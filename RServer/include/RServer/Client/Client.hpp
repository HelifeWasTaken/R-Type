#pragma once

#include "RServer/Client/UDPClient.hpp"
#include "RServer/Client/TCPClient.hpp"

namespace rtype {
namespace net {

    class UDP_TCP_Client {
    private:
        boost::asio::io_context _udp_io_context;
        boost::asio::io_context _tcp_io_context;
        UDPClient _udp_client;
        TCPClient _tcp_client;

        boost::shared_ptr<boost::thread> _context_io_udp_thread;
        boost::shared_ptr<boost::thread> _context_io_tcp_thread;

        std::atomic_bool _is_running;

        /**
         * @brief Runs all the io_contexts in their own threads
         */
        void run();

    public:
        /**
         * @brief Creates a new UDP_TCP_Client
         * @param const char * The tcp host
         * @param const char * The udp host
         * @param const char * The TCP port the socket is listening to
         * @param const char * The UDP port the socket is listening to
         */
        UDP_TCP_Client(const char* host_tcp, const char* host_udp,
            const char* tcp_port, const char* udp_port);
        
        /**
         * @brief Destroy the udp tcp client object
         */
        ~UDP_TCP_Client();

        /**
         * @brief Gets the UDP client
         * @return UDPClient & The UDP client
         */
        UDPClient& udp();

        /**
         * @brief Gets the TCP client
         * @return TCPClient & The TCP client
         */
        TCPClient& tcp();

        /**
         * @brief Tells whether the server should be restarted
         *        Because a service is down
         */
        bool should_restart() const;
    };

    using Client = UDP_TCP_Client;

}
}