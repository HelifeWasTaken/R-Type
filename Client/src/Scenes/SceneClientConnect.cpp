#include "Scenes/SceneClientConnect.hpp"

#include "PileAA/GUI.hpp"

using namespace rtype::net;

static PAA_SCENE_DECL(client_connect) * self = nullptr;

static bool connect_server_if_not_connected()
{
    if (g_game.service.is_service_on() == false)
        return false;


    auto& tcp = g_game.service.tcp();
    auto& udp = g_game.service.udp();
    shared_message_t msg;

    while (tcp.poll(msg) || udp.poll(msg))
        ; // Auto manage event polling and connection

    if (tcp.is_connected()) {
        if (!udp.is_connected() && self->timer.isFinished()) {
            udp.feed_request(tcp.token(), tcp.id());
            self->timer.restart();
            return false;
        } else if (udp.is_connected()) {
            self->text->setText("Connected to server!");
            return true;
        } else {
            return false;
        }
    } else if (self->timer.isFinished()) {
        self->timer.restart();
        g_game.service.run(); // Restart the service so the tcp send another
                              // CONN_INIT request
        return false;
    }
    return false;
}

PAA_START_CPP(client_connect)
{
    self = this;

    // Make sure the ECS clear
    PAA_ECS.clear();

    // Always restart on this
    g_game.service.run("../Client.conf");

    // 1 second for each try to connect to feed or server
    timer.setTarget(1000);

    // use HUD View in case the player was in game
    g_game.use_hud_view();

    gui.addObject(new paa::Button("Connect", [this]() {
        if (g_game.service.connected()) {
            PAA_SET_SCENE(connect_room);
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

    gui.addObject(new paa::Button("Refresh connection", [this]() {
        g_game.service.run(inputIP->getText(),
                            g_game.service.tcp_port,
                            g_game.service.udp_port);

        text->setText("Trying to connect to server...");
    }));

    inputIP = paa::GuiFactory::new_input_text(g_game.service.host, "Server IP");

    gui.addObject(inputIP);
    gui.addObject(text);
}

PAA_END_CPP(client_connect) { gui.clear(); }

PAA_UPDATE_CPP(client_connect)
{
    connect_server_if_not_connected();
    gui.update();
}
