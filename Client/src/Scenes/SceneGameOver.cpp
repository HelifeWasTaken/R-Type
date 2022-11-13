
#include "ClientScenes.hpp"
#include "Enemies.hpp"
#include "Player.hpp"
#include "utils.hpp"

using namespace rtype::net;

static paa::Controller new_keyboard()
{
    paa::ControllerKeyboard* keyboard = new paa::ControllerKeyboard();

    keyboard->setKey(RTYPE_SHOOT_BUTTON, paa::Keyboard::Key::Space);
    return paa::Controller(keyboard);
}

PAA_START_CPP(game_over)
{
    exitGameOver = false;
    keyboard = new_keyboard();
    pressAnyKeyTimer.setTarget(1000);
    pressAnyKeyTimer.restart();
    
    gameOverText.setCharacterSize(20);
    gameOverText.setString("Game Over");
    gameOverText.setFont(font);
    gameOverText.setOutlineThickness(2);
    gameOverText.setOutlineColor(sf::Color::White);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(RTYPE_MENU_CENTERED_X(gameOverText), 200);

    retryText.setCharacterSize(10);
    retryText.setFont(font);
    retryText.setOutlineThickness(1);
    retryText.setOutlineColor(sf::Color::White);
    retryText.setFillColor(sf::Color::Blue);

    scoreText.setCharacterSize(12);
    scoreText.setString("Your score - " +  std::to_string(g_game.score) + " pts");
    scoreText.setFont(font);
    scoreText.setOutlineThickness(1);
    scoreText.setOutlineColor(sf::Color::White);
    scoreText.setFillColor(sf::Color::Blue);
    scoreText.setPosition(RTYPE_MENU_CENTERED_X(scoreText), 300);
}

PAA_END_CPP(game_over)
{
}

static void manage_launch_game(shared_message_t msg)
{
    auto rep = parse_message<GameLauncher>(msg.get());
    if (!rep) {
        spdlog::error("Client: Failed to parse LaunchGame message");
        return;
    } else if (rep->yes()) {
        paa::Random::srand(rep->seed());
        spdlog::info("Client: Launching game");
        paa::GMusicPlayer::play("../assets/launch_game.ogg", false);
        PAA_SET_SCENE(game_scene);
    } else {
        spdlog::info("Client: Receive no when tried to launch game");
    }
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
        case message_code::LAUNCH_GAME_REP:
            manage_launch_game(msg);
            break;
        case message_code::ROOM_CLIENT_DISCONNECT:
            update_player_room_client_disconnect(msg);
            break;
        default:
            spdlog::info(
                "Client create_room: Received message of type {}", msg->type());
            break;
        }
    }
}

PAA_UPDATE_CPP(game_over)
{
    if (PAA_INPUT.isKeyReleased(paa::Keyboard::Key::Escape)) {
        g_game.service.stop();
    }
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);

    g_game.use_hud_view();

    if (g_game.is_host) {
        retryText.setString("Press Space to continue");
    } else {
        retryText.setString("Waiting for the host...");
    }
    retryText.setPosition(RTYPE_MENU_CENTERED_X(retryText), 350);
    
    if (g_game.in_transition()) {
        g_game.transition.update();
        if (exitGameOver && g_game.transition_is_halfway()) {
            if (g_game.is_host && g_game.service.is_service_on()) {
                g_game.service.tcp().send(SignalMarker(message_code::LAUNCH_GAME));
            }
        }
    } else {
        if (g_game.is_host && pressAnyKeyTimer.isFinished() && keyboard->isButtonPressed(RTYPE_SHOOT_BUTTON)) {
            exitGameOver = true;
            g_game.launch_transition();
        }
    }


    PAA_SCREEN.draw(gameOverText);
    PAA_SCREEN.draw(retryText);
    PAA_SCREEN.draw(scoreText);
    g_game.transition.draw();

    g_game.use_game_view();

    update_server_event();
}

PAA_EVENTS_CPP(game_over) { }
