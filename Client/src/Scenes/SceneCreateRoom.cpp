#include "ClientScenes.hpp"

using namespace rtype::net;

static void manage_create_room_reply(shared_message_t msg)
{
    auto rep = parse_message<CreateRoomReply>(msg.get());
    if (!rep) {
        spdlog::error("Client: Failed to parse CREATE_ROOM_REPLY message");
        PAA_SET_SCENE(
            connect_room); // Go back to choice between host and connect
    }
    spdlog::info(
        "Client create_room: Created room with token {}", rep->token());
    g_game.room_token = rep->token();
    g_game.id = 0;
    g_game.is_host = true;
    PAA_SET_SCENE(waiting_room);
    return;
}

static void manage_server_events()
{
    auto& tcp = g_game.service.tcp();
    shared_message_t msg;

    if (!tcp.poll(msg))
        return;

    switch (msg->code()) {
    case message_code::CREATE_ROOM_REPLY:
        manage_create_room_reply(msg);
        break;
    default:
        spdlog::info(
            "Client create_room: Received message of type {}", msg->type());
        break;
    }
}

PAA_START_CPP(create_room)
{
    g_game.service.tcp().send(SignalMarker(message_code::CREATE_ROOM));
}

PAA_END_CPP(create_room)
{}

PAA_UPDATE_CPP(create_room)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);
    manage_server_events();
}
