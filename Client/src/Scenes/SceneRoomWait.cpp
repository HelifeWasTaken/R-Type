#include "ClientScenes.hpp"

using namespace rtype::net;

static void manage_room_client_connect(shared_message_t msg)
{
    auto rep = parse_message<UserConnectRoom>(msg.get());
    if (!rep) {
        spdlog::error("Client: Failed to parse ROOM_CLIENT_CONNECT message");
        return;
    }
    spdlog::info("Client: Player {} connected to room", rep->playerID());
    g_game.connected_players[rep->playerID()] = true;
}

static void manage_room_client_disconnect(shared_message_t msg)
{
    auto rep = parse_message<UserDisconnectFromRoom>(msg.get());
    if (!rep) {
        spdlog::error("Client: Failed to parse UserDisconnectFromRoom message");
        return;
    }
    spdlog::info("Client: Player {} disconnected from room", rep->get_disconnected_user_id());
    g_game.connected_players[rep->get_disconnected_user_id()] = false;
    if (rep->get_new_host_id() == g_game.id) {
        g_game.is_host = true;
        spdlog::info("Client: I am now the host of the room");
    }
}

static void manage_launch_game(shared_message_t msg)
{
    auto rep = parse_message<YesNoMarker>(msg.get());
    if (!rep) {
        spdlog::error("Client: Failed to parse LaunchGame message");
        return;
    } else if (rep->yes()) {
        spdlog::info("Client: Launching game");
        PAA_SET_SCENE(game_scene);
    } else {
        spdlog::info("Client: Receive no when tried to launch game");
    }
}

static void manage_server_events()
{
    auto& tcp = g_game.service.tcp();
    shared_message_t msg;

    if (!tcp.poll(msg))
        return;

    switch (msg->code()) {
    case message_code::ROOM_CLIENT_CONNECT: return manage_room_client_connect(msg);
    case message_code::ROOM_CLIENT_DISCONNECT: return manage_room_client_disconnect(msg);
    case message_code::LAUNCH_GAME_REP: return manage_launch_game(msg);
    default:
        spdlog::info("Client waiting_room: Received message of type {}", (int)msg->code());
        break;
    }
}

///// ---------- /////

PAA_START_CPP(waiting_room)
{
    std::memset(g_game.connected_players.data(), 0, g_game.connected_players.size());
    g_game.connected_players[g_game.id] = true;
}

PAA_UPDATE_CPP(waiting_room)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);
    manage_server_events();

    // Display clients connected to the room
    // If the client is the host, display a button to launch the game
    // If the client is not the host,
    // display a message saying that the host must launch the game
}
