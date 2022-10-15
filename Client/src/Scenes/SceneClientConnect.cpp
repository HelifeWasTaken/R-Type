#include "Scenes/SceneClientConnect.hpp"

#include "PileAA/GUI.hpp"

using namespace rtype::net;

static PAA_SCENE_DECL(client_connect) *self = nullptr;

static bool connect_server_if_not_connected()
{
    if (g_game.service.restart_if_necessary() == false)
        return false;

    auto& tcp = g_game.service.tcp();
    auto& udp = g_game.service.udp();
    shared_message_t msg;

    while (tcp.poll(msg) || udp.poll(msg)); // Auto manage event polling and connection

    if (tcp.is_connected()) {
        if (!udp.is_connected() && self->timer.isFinished()) {
            udp.feed_request(tcp.token(), tcp.id());
            self->timer.restart();
            return false;
        }
        return true;
    } else if (self->timer.isFinished()) {
        self->timer.restart();
        g_game.service.run(); // Restart the service so the tcp send another CONN_INIT request
        return false;
    }
    return false;
}

PAA_START_CPP(client_connect)
{
    self = this;

    g_game.service.run("../Client.conf");

    // 1 second for each try to connect to feed or server
    timer.setTarget(1000);

    gui.addObject(new paa::Button("Connect", [this]() {
        if (g_game.service.connected()) {
            PAA_SET_SCENE(connect_room);
            std::cout << "Connected to server" << std::endl;
        } else {
            text->setText("Cannot connect to room as server is not connected");
        }
    }));

    gui.addObject(new paa::Button("Host", [this]() {
        if (g_game.service.connected()) {
            PAA_SET_SCENE(create_room);
        } else {
            text->setText("Cannot host room as server is not connected");
        }
    }));

    gui.addObject(text);
}

PAA_UPDATE_CPP(client_connect)
{
    connect_server_if_not_connected();
    gui.update();
}
