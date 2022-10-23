#include "ClientScenes.hpp"
#include "RServer/Messages/Messages.hpp"

using namespace rtype::net;

static PAA_SCENE_DECL(connect_room) * self = nullptr;

static void check_connect_room_reply(shared_message_t& msg)
{
    auto rep = parse_message<RequestConnectRoomReply>(msg.get());

    // Go back to choice connect or host if
    // failed to parse message or if player id is invalid
    if (!rep) {
        spdlog::error("Client: Failed to parse CONNECT_ROOM_REPLY message");
        self->text->setText(
            "Failed to connect to room invalid packet received from server");
    } else if (rep->playerID() == RTYPE_INVALID_PLAYER_ID) {
        spdlog::error("Client: Failed to connect to room");
        self->text->setText("The room is full or does not exist");
    } else {
        spdlog::info("Client: Connected to room as {}", rep->playerID());
        g_game.id = rep->playerID();
        PAA_SET_SCENE(waiting_room);
    }
}

static void manage_server_events()
{
    auto& tcp = g_game.service.tcp();
    shared_message_t msg;

    if (!tcp.poll(msg))
        return;

    switch (msg->code()) {
    case message_code::CONNECT_ROOM_REQ_REP:
        check_connect_room_reply(msg);
        break;
    default:
        spdlog::info("Client connect_room: Received message of type {}",
            (int)msg->code());
        break;
    }
}

PAA_START_CPP(connect_room)
{
    self = this;

    gui.addObject(input);
    gui.addObject(new paa::Button("Connect", [this]() {
        g_game.room_token = input->getText();
        g_game.service.tcp().send(RequestConnectRoom(g_game.room_token));
    }));
    gui.addObject(text);
}

PAA_UPDATE_CPP(connect_room)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);

    manage_server_events();
    gui.update();
    // Display waiting response from server
}
