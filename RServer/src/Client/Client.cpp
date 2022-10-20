#include "RServer/Client/Client.hpp"
#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    void UDP_TCP_Client::run()
    {
        _context_io_udp_thread = boost::make_shared<boost::thread>(
            [&udp_io_context=_udp_io_context, &is_running=_is_running]() {
                while (is_running)
                    udp_io_context.run();
            });
        _context_io_tcp_thread = boost::make_shared<boost::thread>(
            [&tcp_io_context=_tcp_io_context, &is_running=_is_running]() {
                while (is_running)
                    tcp_io_context.run();
            });
    }

    UDP_TCP_Client::UDP_TCP_Client(const char* host_tcp, const char* host_udp,
                                        const char* tcp_port, const char* udp_port)
                                        : _tcp_io_context(boost::asio::io_context())
                                        , _udp_io_context(boost::asio::io_context())
                                        , _udp_client(_udp_io_context, host_udp, udp_port)
                                        , _tcp_client(_tcp_io_context, host_tcp, tcp_port)
    {
        _is_running = true;
        run();
    }

    UDP_TCP_Client::~UDP_TCP_Client()
    {
        _is_running = false;
        _udp_io_context.stop();
        _tcp_io_context.stop();
        _context_io_udp_thread->join();
        _context_io_tcp_thread->join();
    }

    UDPClient& UDP_TCP_Client::udp()
    {
        return _udp_client;
    }


    TCPClient& UDP_TCP_Client::tcp()
    {
        return _tcp_client;
    }

    bool UDP_TCP_Client::should_restart() const
    {
        return _tcp_client.stopped() || _udp_client.stopped();
    }
}
}
