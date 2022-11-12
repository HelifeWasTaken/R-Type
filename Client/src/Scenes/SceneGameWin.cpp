
#include "ClientScenes.hpp"
#include "Enemies.hpp"
#include "Player.hpp"
#include "utils.hpp"
#include "PileAA/MusicPlayer.hpp"

using namespace rtype::net;

static const int SCROLL_SPEED = 6;

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

static void move_player_axis(hl::silva::Entity entity, paa::Vector2f vec) {
    auto c = PAA_GET_COMPONENT(entity, paa::Controller);

    c->simulateAxisMovement(paa::Joystick::Axis::Y, vec.y);
    c->simulateAxisMovement(paa::Joystick::Axis::X, vec.x);
}

static bool move_player_towards(hl::silva::Entity entity, paa::Vector2f pos) {
    auto p = PAA_GET_COMPONENT(entity, paa::Position);

    paa::Vector2f delta = paa::Math::distance(p, pos);
    paa::Vector2f move = paa::Math::normalize(delta);
    float distance = paa::Math::distance_between(p, pos);

    if (std::abs(p.x - pos.x) < 10) {
        move.x = 0;
    }
    if (std::abs(p.y - pos.y) < 10) {
        move.y = 0;
    }

    if (distance < 60) {
        move_player_axis(entity, { 0, 0 });
        return false;
    }
    move_player_axis(entity, move);
    return true;
}


static const std::array<paa::Vector2f, 2> MOVEPOINTS = {
    paa::Vector2f(205, 140), paa::Vector2f(-80, -400) };

static const char *const MAPS[] = {
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
        g_game.scroll += SCROLL_SPEED;
        g_game.game_view.move(SCROLL_SPEED, 0);
        g_game.use_game_view();
    }
    map.update();
}

static void reinitialize_game()
{
    PAA_ECS.clear();

    g_game.scroll = 0;
    g_game.old_scroll = 0;
    g_game.enemies_to_entities.clear();

    g_game.reset_game_view();
    g_game.use_game_view();

    paa::Controller c = new_simulated_controller();
    g_game.players_entities[g_game.id] = rtype::game::PlayerFactory::addPlayer(g_game.id, c, false, false);
    g_game.players_alive[g_game.id] = true;
    spdlog::info("Added player {} on the field", g_game.id);

    auto player = PAA_GET_COMPONENT(g_game.players_entities[g_game.id], rtype::game::Player);
    player->set_speed_x(150.f / 3.f);
    player->set_speed_y(100.f / 3.f);
    auto& p = PAA_GET_COMPONENT(g_game.players_entities[g_game.id], paa::Position);
    p.x = -15;
    spdlog::info("Position: ({}, {})", p.x, p.y);
}

static void spawn_spaceships_batch(bool doubleShipsAmount, std::vector<hl::silva::Entity>& ships)
{
    size_t size = ships.size();
    int max_ships = rand() % (doubleShipsAmount ? 12 : 6);
    for (int i = 0; i < max_ships; i++) {
        paa::Controller c = new_simulated_controller();
        auto playerEntity = rtype::game::PlayerFactory::addPlayer(rand() % 3, c, false, false);
        spdlog::info("Added spaceship {} on the field", playerEntity);

        auto player = PAA_GET_COMPONENT(playerEntity, rtype::game::Player);
        player->set_speed_x(150.f / 3.f + (rand() % 60) - 60);
        player->set_speed_y(100.f / 3.f);
        auto& p = PAA_GET_COMPONENT(playerEntity, paa::Position);
        p.y += (rand() % 300) - 150;
        p.x = -100;
        ships.push_back(playerEntity);
        spdlog::info("Position: ({}, {})", p.x, p.y);
    }
}

static void update_shaceships_batch(std::vector<hl::silva::Entity>& ships)
{
    for (int i = 0; i < ships.size(); i++) {
        auto p = PAA_GET_COMPONENT(ships[i], paa::Position);
        if (p.x > 820) {
            PAA_DESTROY_ENTITY(ships[i]);
            spdlog::info("Destroyed spaceship {}", ships[i]);
            ships.erase(ships.begin() + i);
            i--;
        } else {
            move_player_towards(ships[i], paa::Vector2f(900, p.y));
        }
    }
}

PAA_START_CPP(game_win)
{
    map_index = 0;
    localPlayerTargetId = 0;
    paa::GMusicPlayer::play("../assets/game_win.ogg", false);

    reinitialize_game();
    deltaTimer.restart();
    showMoreShipsTimer.setTarget(3000);
    showMoreShipsTimer.restart();
    doubleShipsAmountTimer.setTarget(18000);
    doubleShipsAmountTimer.restart();
    cinematicEndTimer.setTarget(37000);
    cinematicEndTimer.restart();

    endText.setCharacterSize(20);
    endText.setFont(font);
    endText.setPosition(RTYPE_HUD_WIDTH - 100, RTYPE_HUD_TOP + 20);
    endText.setOutlineThickness(2);
    endText.setOutlineColor(sf::Color::White);
    endText.setFillColor(sf::Color::Blue);
    endText.setString(
        "THANKS TO YOUR BRAVE\n\n\n\nFIGHTING. THE COSMOS\n\n\n\nRESTORED PEACE\n\n\n\nTHE BYDE EMPIRE WAS\n\n\n\nANNIHILATED TO NEVER\n\n\n\nSCARE "
        "PEOPLE AGAIN.\n\n\n\nYOUR NAME WILL REMAIN\n\n\n\nIN THE UNIVERSE FOREVER.\n\n\n\nTHANK YOU FOR PLAYING\n\n\n\nTHE GAME TO THE END.");
    auto endTextRect = endText.getGlobalBounds();
    endText.setPosition(RTYPE_HUD_WIDTH - (int)(endTextRect.width / 2) + 40, 690);
}

PAA_END_CPP(game_win)
{
    g_game.service.stop();
    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        if (g_game.players_entities[i]) {
            g_game.players_entities[i] = PAA_ENTITY();
            g_game.connected_players[i] = false;
            g_game.players_alive[i] = false;
        }
    }
    PAA_ECS.clear();
    backgroundShips.clear();
    map = nullptr;
}

static void handle_transition(paa::Timer& cinematicEndTimer, std::unique_ptr<rtype::game::Map>& map)
{
    if (g_game.in_transition()) {
        g_game.transition.update();
        if (g_game.transition_is_halfway()) {
            if (cinematicEndTimer.isFinished()) {
                PAA_SET_SCENE(client_connect);
            }
        }
    } else {
        if (cinematicEndTimer.isFinished()) {
            g_game.launch_transition();
        }
    }
}

static void handle_end_text(paa::Text& endText, paa::DeltaTimer& deltaTimer)
{
    paa::Vector2f pos = endText.getPosition();
    pos.y -= 38 * deltaTimer.getDeltaTime();
    endText.setPosition(pos);
}

static void update_player_room_client_disconnect(shared_message_t& msg)
{
    const auto sp = parse_message<UserDisconnectFromRoom>(msg);

    spdlog::warn("User {} disconnected from the room, new host is {}",
                sp->get_disconnected_user_id(), sp->get_new_host_id());

    g_game.connected_players[sp->get_disconnected_user_id()] = false;
    g_game.players_alive[sp->get_disconnected_user_id()] = false;
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
        case message_code::ROOM_CLIENT_DISCONNECT:
            update_player_room_client_disconnect(msg);
            break;
        default:
            spdlog::info(
                "Client game_win: Received message of type {}", msg->type());
            break;
        }
    }
}

PAA_UPDATE_CPP(game_win)
{
    handle_transition(cinematicEndTimer, map);

    deltaTimer.update();
    handle_end_text(endText, deltaTimer);
    update_shaceships_batch(backgroundShips);

    if (showMoreShipsTimer.isFinished()) {
        spawn_spaceships_batch(doubleShipsAmountTimer.isFinished(), backgroundShips);
        showMoreShipsTimer.setTarget(1000);
        showMoreShipsTimer.restart();
    }

    if (map != nullptr)
        scroll_map(*map);

    g_game.use_hud_view();

    PAA_SCREEN.draw(endText);

    g_game.transition.draw();

    g_game.use_game_view();

    if (localPlayerTargetId < 2) {
        if (!move_player_towards(g_game.players_entities[g_game.id], MOVEPOINTS[localPlayerTargetId])) {
            localPlayerTargetId++;
        }
    }

    update_server_event();
}

PAA_EVENTS_CPP(game_win) { }
