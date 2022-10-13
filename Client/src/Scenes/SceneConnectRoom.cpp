#include "ClientScenes.hpp"
#include "RServer/Messages/Messages.hpp"

using namespace rtype::net;

static void check_connect_room_reply(shared_message_t msg)
{
    auto rep = parse_message<RequestConnectRoomReply>(msg.get());
    if (!rep) {
        spdlog::error("Client: Failed to parse CONNECT_ROOM_REPLY message");
        PAA_SET_SCENE(connect_room);
    } else if (rep->playerID() == RTYPE_INVALID_PLAYER_ID) {
        spdlog::error("Client: Failed to connect to room");
        PAA_SET_SCENE(connect_room);
    } else {
        spdlog::info("Client: Connected to room as {}", rep->playerID());
        g_game.id = rep->playerID();
        PAA_SET_SCENE(game_scene);
    }
}

PAA_START_CPP(connect_room)
{
    std::cout << "Enter room token: ";
    std::cin >> _roomToken;

    g_game.service.tcp().send(RequestConnectRoom(_roomToken));
}

PAA_UPDATE_CPP(connect_room)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);

    auto& tcp = g_game.service.tcp();
    shared_message_t msg;

    if (tcp.poll(msg)) {
        if (msg->type() == message_type::CONNECT_ROOM_REQ_REP) {
            check_connect_room_reply(msg);
        } else {
            spdlog::info("Client connect_room: Received message of type {}", msg->type());
        }
    }
}