#include "ClientWrapper.hpp"
#include <fstream>

namespace rtype {
namespace game {

    bool ClientWrapper::run(const std::string& host,
                            const rtype::net::PortType& tcp_port,
                            const rtype::net::PortType& udp_port)
    {
        try {
            this->host = host;
            this->tcp_port = tcp_port;
            this->udp_port = udp_port;

            client = std::make_shared<net::Client>(
                host.c_str(), host.c_str(),
                std::to_string(tcp_port).c_str(),
                std::to_string(udp_port).c_str());

            return is_service_on();
        } catch (const std::exception& e) {
            spdlog::error("ClientWrapper::run: tcp({}:{}) or udp({}:{}) "
                            "could not be reached or permission was denied.",
                            host, tcp_port, host, udp_port);
            client = nullptr;
        }
        return false;
    }

    bool ClientWrapper::run(const std::string& configuration_file)
    {
        try {
            nlohmann::json json;
            std::ifstream file(configuration_file);
            file >> json;
            return run(json["host"], json["tcp_port"], json["udp_port"]);
        } catch (const std::exception& e) {
            spdlog::error("ClientWrapper::run: configuration file {} "
                            "could not be opened or is invalid.", configuration_file);
            return false;
        }
    }

    bool ClientWrapper::run()
    {
        return run(host, tcp_port, udp_port);
    }

    void ClientWrapper::stop()
    {
        client = nullptr;
    }

    bool ClientWrapper::is_service_on() const
    {
        return client && !client->should_restart();
    }

    bool ClientWrapper::restart_if_necessary()
    {
        if (is_service_on())
            return true;
        return run();
    }

    bool ClientWrapper::tcp_is_connected() const
    {
        return client && client->tcp().is_connected();
    }

    bool ClientWrapper::udp_is_connected() const
    {
        return client && client->udp().is_connected();
    }

    bool ClientWrapper::connected() const
    {
        return tcp_is_connected() && udp_is_connected();
    }

};
}