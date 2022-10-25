
#include "ClientScenes.hpp"
#include "Player.hpp"

using namespace rtype::net;

static paa::Controller new_keyboard()
{
    paa::ControllerKeyboard *keyboard = new paa::ControllerKeyboard();

    keyboard->setAxis(paa::Joystick::Axis::X, paa::Keyboard::Key::Left, paa::Keyboard::Key::Right);
    keyboard->setAxis(paa::Joystick::Axis::Y, paa::Keyboard::Key::Up, paa::Keyboard::Key::Down);
    keyboard->setKey(RTYPE_SHOOT_BUTTON, paa::Keyboard::Key::Space);
    return paa::Controller(keyboard);
}

static paa::Controller new_simulated_controller()
{
    paa::SimulatedController *controller = new paa::SimulatedController();
    return paa::Controller(controller);
}

PAA_START_CPP(game_scene)
{
    spdlog::error("Client: Starting game scene");
    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        spdlog::error("Client: Player {} is {}", i, g_game.connected_players[i] ? "connected" : "disconnected");
        if (g_game.connected_players[i]) {
            paa::Controller c = i == g_game.id ? new_keyboard() : new_simulated_controller();
            g_game.players_entities[i] = rtype::game::PlayerFactory::addPlayer(i, c);
            spdlog::error("Client: Player {} added", i);
        }
    }
}

PAA_END_CPP(game_scene)
{
    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        if (g_game.players_entities[i]) {
            g_game.players_entities[i] = PAA_ENTITY();
            g_game.connected_players[i] = false;
        }
    }
    PAA_ECS.clear();
}

PAA_UPDATE_CPP(game_scene)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);

    auto& tcp = g_game.service.tcp();
    auto& udp = g_game.service.udp();

    shared_message_t msg;
    while (tcp.poll(msg) || udp.poll(msg)) {
        if (msg->code() == message_code::UPDATE_PLAYER) {
            const auto sp = parse_message<UpdateMessage>(msg);
            const rtype::game::SerializablePlayer p(sp->data());
            paa::DynamicEntity e = g_game.players_entities[p.get_player()];
            e.getComponent<rtype::game::Player>()->update_info(p);
        }
    }
}

PAA_EVENTS_CPP(game_scene) { }
