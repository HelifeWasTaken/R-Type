
#include "ClientScenes.hpp"
#include "Enemies.hpp"
#include "Player.hpp"

using namespace rtype::net;

static const int SCROLL_SPEED = 1;

static paa::Controller new_keyboard()
{
    paa::ControllerKeyboard* keyboard = new paa::ControllerKeyboard();

    keyboard->setAxis(paa::Joystick::Axis::X, paa::Keyboard::Key::Left,
        paa::Keyboard::Key::Right);
    keyboard->setAxis(paa::Joystick::Axis::Y, paa::Keyboard::Key::Up,
        paa::Keyboard::Key::Down);
    keyboard->setKey(RTYPE_SHOOT_BUTTON, paa::Keyboard::Key::Space);
    return paa::Controller(keyboard);
}

static paa::Controller new_simulated_controller()
{
    paa::SimulatedController* controller = new paa::SimulatedController();
    return paa::Controller(controller);
}

static void scroll_map(rtype::game::Map& map)
{

    if (g_game.lock_scroll == false) {
        g_game.old_scroll = g_game.scroll;
        g_game.scroll += SCROLL_SPEED;
        auto v = PAA_SCREEN.getView();
        v.move(SCROLL_SPEED, 0);
        PAA_SCREEN.setView(v);
    }
    map.update();
}

PAA_START_CPP(game_scene)
{
    g_game.scroll = 0;
    sf::View v = PAA_SCREEN.getView();
    v.setSize(384, 256);
    v.setCenter(v.getSize().x / 2, v.getSize().y / 2);
    PAA_SCREEN.setView(v);

    spdlog::error("Client: Starting game scene");

    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        spdlog::error("Client: Player {} is {}", i,
            g_game.connected_players[i] ? "connected" : "disconnected");
        if (g_game.connected_players[i]) {
            paa::Controller c
                = i == g_game.id ? new_keyboard() : new_simulated_controller();
            g_game.players_entities[i]
                = rtype::game::PlayerFactory::addPlayer(i, c);
            g_game.players_alive[i] = true;
            spdlog::error("Client: Player {} added", i);
        }
    }

    map = std::make_unique<rtype::game::Map>(
        "../assets/maps/BydoEmpire/BydoMap.json");
}

PAA_END_CPP(game_scene)
{
    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        if (g_game.players_entities[i]) {
            g_game.players_entities[i] = PAA_ENTITY();
            g_game.connected_players[i] = false;
            g_game.players_alive[i] = false;
        }
    }
    PAA_ECS.clear();

    g_game.scroll = 0;
}

static void update_player_position(shared_message_t& msg)
{
    const auto sp = parse_message<UpdateMessage>(msg);
    const rtype::game::SerializablePlayer p(sp->data());
    paa::DynamicEntity e = g_game.players_entities[p.get_player()];
    e.getComponent<rtype::game::Player>()->update_info(p);
}

static void update_enemy_death(shared_message_t& msg)
{
    const auto sp = parse_message<UpdateMessage>(msg);
    SerializedEnemyDeath e;
    e.from(sp->data().data(), sp->data().size());

    auto it = g_game.enemies_to_entities.find(e.getElement());
    if (it != g_game.enemies_to_entities.end()) {
        PAA_ECS.kill_entity(it->second);
        g_game.enemies_to_entities.erase(it);
    }
}

static void update_player_death(shared_message_t& msg)
{
    const auto sp = parse_message<UpdateMessage>(msg);
    SerializedPlayerDeath e;
    e.from(sp->data().data(), sp->data().size());

    PAA_ECS.kill_entity(g_game.players_entities[e.getElement()]);
    g_game.players_alive[e.getElement()] = false;
}

static void update_server_event()
{
    auto& tcp = g_game.service.tcp();
    auto& udp = g_game.service.udp();

    shared_message_t msg;
    while (tcp.poll(msg) || udp.poll(msg)) {
        switch (msg->code()) {
        case message_code::UPDATE_ENEMY_DESTROYED:
            update_enemy_death(msg);
            break;
        case message_code::UPDATE_PLAYER_DESTROYED:
            update_player_death(msg);
            break;
        case message_code::UPDATE_PLAYER:
            update_player_position(msg);
            break;
        default:
            break;
        }
    }
}

PAA_UPDATE_CPP(game_scene)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);

    scroll_map(*map);
    update_server_event();
}

PAA_EVENTS_CPP(game_scene) { }