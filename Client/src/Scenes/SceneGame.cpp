
#include "ClientScenes.hpp"
#include "Enemies.hpp"
#include "Player.hpp"
#include "utils.hpp"
#include "PileAA/MusicPlayer.hpp"

using namespace rtype::net;

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

static const char *const MAPS[] = {
    "../assets/maps/BydoEmpire/BydoMap.json",
    "../assets/maps/MiningField/MiningField.json",
    "../assets/maps/Ruins/Ruins.json",
    "../assets/maps/RecyclingFactory/RecyclingFactory.json",
    nullptr
};

static std::unique_ptr<rtype::game::Map> load_next_map(unsigned int &index)
{
    if (MAPS[index] != NULL) {
        return std::make_unique<rtype::game::Map>(MAPS[index++]);
    }
    return nullptr;
}

static void scroll_map(rtype::game::Map& map)
{
    if (g_game.in_transition() == false && g_game.lock_scroll == false) {
        g_game.old_scroll = g_game.scroll;
        g_game.scroll += g_game.scroll_speed;
        g_game.game_view.move(g_game.scroll_speed, 0);
        g_game.use_game_view();
    }
    map.update();
}

static void reinitialize_game()
{
    PAA_ECS.clear();

    g_game.score = 0;
    g_game.scroll = 0;
    g_game.old_scroll = 0;
    g_game.scroll_speed = DEFAULT_SCROLL_SPEED;
    g_game.enemies_to_entities.clear();

    g_game.reset_game_view();
    g_game.use_game_view();

    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        if (g_game.connected_players[i]) {
            paa::Controller c
                = i == g_game.id ? new_keyboard() : new_simulated_controller();
            g_game.players_entities[i]
                = rtype::game::PlayerFactory::addPlayer(i, c);
            g_game.players_alive[i] = true;
            spdlog::info("Added player {} on the field", i);

            auto p = PAA_GET_COMPONENT(g_game.players_entities[i], paa::Position);
            spdlog::info("Position: ({}, {})", p.x, p.y);
        }
    }
}

PAA_START_CPP(game_scene)
{
    map_index = 0;

    reinitialize_game();

    g_game.launch_transition();

    lifeText.setCharacterSize(10);
    lifeText.setFont(font);
    lifeText.setPosition(RTYPE_HUD_LEFT + 10, RTYPE_HUD_TOP);
    lifeText.setOutlineThickness(1);
    lifeText.setOutlineColor(sf::Color::White);
    lifeText.setFillColor(sf::Color::Blue);

    scoreText.setCharacterSize(20);
    scoreText.setFont(font);
    scoreText.setPosition(RTYPE_HUD_WIDTH - 100, RTYPE_HUD_TOP + 20);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::White);
    scoreText.setFillColor(sf::Color::Red);
}

PAA_END_CPP(game_scene)
{
    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        if (g_game.players_entities[i]) {
            g_game.players_entities[i] = PAA_ENTITY();
            if (!g_game.everyone_is_dead()) {
            g_game.connected_players[i] = false;
            }
            g_game.players_alive[i] = false;
            spdlog::debug("Player {} reset", i);
        }
    }
    PAA_ECS.clear();
    map = nullptr;
}

static void update_player_position(shared_message_t& msg)
{
    const auto sp = parse_message<UpdateMessage>(msg);
    const rtype::game::SerializablePlayer p(sp->data());
    paa::DynamicEntity e = g_game.players_entities[sp->sid()];
    try {
        e.getComponent<rtype::game::Player>()->update_info(p);
    } catch (...) {
        spdlog::warn("You tried to update a dead or disconnected player");
    }
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

static void update_sync_scroll(shared_message_t& msg)
{
    const auto sync = parse_message<UpdateMessage>(msg);
    SerializedScroll s;
    s.from(sync->data().data(), sync->data().size());

    auto current_scroll = g_game.scroll;

    g_game.scroll = s.getElement();
    g_game.old_scroll = g_game.old_scroll >= g_game.scroll_speed ? (g_game.scroll - g_game.scroll_speed) : 0;

    g_game.reset_game_view();
    g_game.game_view.move(g_game.scroll, 0);
}

static void update_player_room_client_disconnect(shared_message_t& msg)
{
    const auto sp = parse_message<UserDisconnectFromRoom>(msg);

    spdlog::warn("User {} disconnected from the room, new host is {}",
                sp->get_disconnected_user_id(), sp->get_new_host_id());

    g_game.connected_players[sp->get_disconnected_user_id()] = false;
    g_game.players_alive[sp->get_disconnected_user_id()] = false;
    PAA_ECS.kill_entity(g_game.players_entities[sp->get_disconnected_user_id()]);
    g_game.players_entities[sp->get_disconnected_user_id()] = PAA_ENTITY();

    if (sp->get_new_host_id() == g_game.id) {
        spdlog::warn("You are the new host");
        g_game.is_host = true;
    }
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
        case message_code::UPDATE_SCROLL:
            update_sync_scroll(msg);
            break;
        case message_code::ROOM_CLIENT_DISCONNECT:
            update_player_room_client_disconnect(msg);
            break;
        default:
            break;
        }
    }
}

static void handle_transition(unsigned int& map_index, std::unique_ptr<rtype::game::Map>& map)
{
    if (g_game.in_transition()) {
        g_game.transition.update();
        if (g_game.transition_is_halfway()) {
            if (g_game.everyone_is_dead()) {
                PAA_SET_SCENE(game_over);
                return;
            }
            if ((map == nullptr || map->changes())) {
                reinitialize_game();
                map = load_next_map(map_index);
                if (map == nullptr) {
                    PAA_SET_SCENE(game_win);
                }
            }
        }
    }
}

PAA_UPDATE_CPP(game_scene)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);

    if (g_game.everyone_is_dead() && !g_game.in_transition()) {
        paa::GMusicPlayer::play("../assets/R-Type-Game-Over.ogg", false);
        g_game.launch_transition();
    }

    handle_transition(map_index, map);

    if (map != nullptr)
        scroll_map(*map);

    g_game.use_hud_view();

    if (!g_game.in_transition()) {
        lifeText.setString(g_game.generate_hud_text_for_players_life());
        scoreText.setString(g_game.generate_hud_text_for_score());
    }

    PAA_SCREEN.draw(lifeText);
    PAA_SCREEN.draw(scoreText);
    g_game.transition.draw();

    g_game.use_game_view();

    update_server_event();
}

PAA_EVENTS_CPP(game_scene) { }
