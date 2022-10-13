#include "ClientScenes.hpp"

using namespace rtype::net;

static void manage_create_room_reply(shared_message_t msg)
{
    auto rep = parse_message<CreateRoomReply>(msg.get());
    if (!rep) {
        spdlog::error("Client: Failed to parse CREATE_ROOM_REPLY message");
        exit(1);
    }
    spdlog::info("Client create_room: Created room with token {}", rep->token());
    g_game.id = 0;
    g_game.is_host = true;
    PAA_SET_SCENE(game_scene);
    return;
}

PAA_START_CPP(create_room) {
    g_game.service.tcp().send(SignalMarker(message_code::CREATE_ROOM));
}

PAA_UPDATE_CPP(create_room) {

    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);

    shared_message_t msg;
    auto& tcp = g_game.service.tcp();

    while (tcp.poll(msg)) {
        if (msg->code() == message_code::CREATE_ROOM_REPLY) {
            manage_create_room_reply(msg);
        } else {
            spdlog::info("Client create_room: Received message of type {}", msg->type());
        }
    }
}