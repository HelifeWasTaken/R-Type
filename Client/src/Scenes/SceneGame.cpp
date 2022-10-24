
#include "ClientScenes.hpp"
#include "Player.hpp"

using namespace rtype::net;

static PAA_SCENE_DECL(game_scene) * self = nullptr;

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

    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        if (g_game.connected_players[i]) {
            paa::Controller c = i == g_game.id ? new_keyboard() : new_simulated_controller();
            g_game.players_entities[i] = rtype::game::PlayerFactory::addPlayer(i, c);
        }
    }
}

PAA_UPDATE_CPP(game_scene)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);
}

PAA_EVENTS_CPP(game_scene) { }
