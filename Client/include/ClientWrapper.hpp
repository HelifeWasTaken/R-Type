#pragma once

#include "PileAA/external/nlohmann/json.hpp"
#include "RServer/Client/Client.hpp"

#include <string>
#include <exception>
#include <memory>

namespace rtype {
namespace game {
struct ClientWrapper {

    std::shared_ptr<rtype::net::UDP_TCP_Client> client = nullptr;

    ClientWrapper() = default;
    ~ClientWrapper() = default;

    bool run(const std::string& host,
            const unsigned int& tcp_port,
            const unsigned int& udp_port);

    bool run(const std::string& configuration_file);
    bool run();

    void stop();

    bool is_service_on() const;

    bool restart_if_necessary();
    bool connected() const;
    bool tcp_is_connected() const;
    bool udp_is_connected() const;

    rtype::net::TCPClient& tcp() { return client->tcp(); }
    rtype::net::UDPClient& udp() { return client->udp(); }

    std::string host = "127.0.0.1";
    unsigned int tcp_port = 4242;
    unsigned int udp_port = 4243;
};

#define RTYPE_CLIENT rtype::game::ClientWrapper

}
}