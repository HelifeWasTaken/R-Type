#include "Scenes/SceneClientConnect.hpp"

using namespace rtype::net;

PAA_START_CPP(client_connect)
{
    g_game.service.run("../Client.conf");

    _timer.setTarget(1000);

    std::cout << "host or connect: ";
    std::cin >> choice;
}

PAA_UPDATE_CPP(client_connect)
{
    if (g_game.service.restart_if_necessary() == false)
        return;

    auto& tcp = g_game.service.tcp();
    auto& udp = g_game.service.udp();
    shared_message_t msg;

    while (tcp.poll(msg) || udp.poll(msg));

    if (tcp.is_connected()) {
        if (udp.is_connected()) {
            if (choice == "host") {
                PAA_SET_SCENE(create_room);
            } else {
                PAA_SET_SCENE(connect_room);
            }
        } else if (_timer.isFinished()) {
            udp.feed_request(tcp.token(), tcp.id());
        }
    }
}