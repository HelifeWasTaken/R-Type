#include "Scenes/SceneClientConnect.hpp"

using namespace rtype::net;

static bool connect_server_if_not_connected(paa::Timer& timer)
{
    if (g_game.service.restart_if_necessary() == false)
        return false;

    auto& tcp = g_game.service.tcp();
    auto& udp = g_game.service.udp();
    shared_message_t msg;

    while (tcp.poll(msg) || udp.poll(msg)); // Auto manage event polling and connection

    if (tcp.is_connected()) {
        if (!udp.is_connected() && timer.isFinished()) {
            udp.feed_request(tcp.token(), tcp.id());
            timer.restart();
            return false;
        }
        return true;
    } else if (timer.isFinished()) {
        timer.restart();
        g_game.service.run(); // Restart the service so the tcp send another CONN_INIT request
        return false;
    }
    return false;
}

PAA_START_CPP(client_connect)
{
    g_game.service.run("../Client.conf");

    // 1 second for each try to connect to feed or server
    _timer.setTarget(1000);

    std::cout << "host or connect: ";
    std::cin >> choice;
}

PAA_UPDATE_CPP(client_connect)
{
    if (connect_server_if_not_connected(_timer)) {
        // Means trying to connect to server
    } else {
        // is connected to server
    }
}
