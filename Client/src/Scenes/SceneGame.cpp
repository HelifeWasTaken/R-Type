#include "ClientScenes.hpp"

using namespace rtype::net;

#include "RServer/Messages/Types.hpp"

PAA_ENTITY new_player()
{
    PAA_ENTITY e = PAA_NEW_ENTITY();
    paa::Sprite& s = PAA_SET_SPRITE(e, "spaceship");

    s.setPosition(rand() % 500, rand() % 500);
    s.useAnimation("idle");
    return e;
}

PAA_START_CPP(game_scene)
{
    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        if (g_game.connected_players[i]) {
            players[i] = new_player();
        }
    }
}

PAA_UPDATE_CPP(game_scene)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);

    auto& tcp = g_game.service.tcp();
    auto& udp = g_game.service.udp();
    shared_message_t msg;

    while (tcp.poll(msg) || udp.poll(msg)) {
        if (msg->code() == message_code::UPDATE_VECTOR2_MOVEMENT) {
            auto rep = parse_message<UpdateMessage>(msg.get());
            const vector2i pos(rep->data());
            PAA_GET_COMPONENT(players[rep->sid()], paa::Sprite).move(pos.x, pos.y);
        } else if (msg->code() == rtype::net::message_code::ROOM_CLIENT_DISCONNECT) {
            auto rep = parse_message<UserDisconnectFromRoom>(msg.get());
            spdlog::info("Client: Player {} disconnected from room new host is {}", rep->get_disconnected_user_id(), rep->get_new_host_id());
            if (rep->get_new_host_id() == g_game.id) {
                spdlog::info("Client: I'm the new host");
                g_game.is_host = true;
            }
            PAA_DESTROY_ENTITY(players[rep->get_disconnected_user_id()]);
            g_game.connected_players[rep->get_disconnected_user_id()] = false;
        } else {
            spdlog::info("Client game_scene: Received message of type {}", msg->type());
        }
    }
}

PAA_EVENTS_CPP(game_scene)
{
    const vector2i pos(
        input.isKeyDown(paa::Keyboard::Key::Right) - input.isKeyDown(paa::Keyboard::Key::Left),
        input.isKeyDown(paa::Keyboard::Key::Down) - input.isKeyDown(paa::Keyboard::Key::Up)
    );

    if (pos.x != 0 || pos.y != 0) {
        g_game.service.udp().send(UpdateMessage(g_game.id, pos, message_code::UPDATE_VECTOR2_MOVEMENT));
        PAA_GET_COMPONENT(players[g_game.id], paa::Sprite).move(pos.x, pos.y);
    }
}