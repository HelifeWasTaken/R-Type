
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
    keyboard = new_keyboard();
    gameOverText.setCharacterSize(20);
    gameOverText.setString("Game Over");
    gameOverText.setFont(font);
    gameOverText.setOutlineThickness(2);
    gameOverText.setOutlineColor(sf::Color::White);
    gameOverText.setFillColor(sf::Color::Red);
    auto gameOverTextRect = gameOverText.getGlobalBounds();
    gameOverText.setPosition(RTYPE_HUD_WIDTH - (int)(gameOverTextRect.width/2), 250);

    retryText.setCharacterSize(10);
    retryText.setString("Press Space to continue");
    retryText.setFont(font);
    retryText.setOutlineThickness(1);
    retryText.setOutlineColor(sf::Color::White);
    retryText.setFillColor(sf::Color::Blue);
    auto retryTextRect = retryText.getGlobalBounds();
    retryText.setPosition(RTYPE_HUD_WIDTH - (int)(retryTextRect.width/2), 400);

    scoreText.setCharacterSize(12);
    scoreText.setString("Score - " +  std::to_string(g_game.score) + " pts");
    scoreText.setFont(font);
    scoreText.setOutlineThickness(1);
    scoreText.setOutlineColor(sf::Color::White);
    scoreText.setFillColor(sf::Color::Blue);
    auto scoreTextRect = scoreText.getGlobalBounds();
    scoreText.setPosition(RTYPE_HUD_WIDTH - (int)(scoreTextRect.width/2), 350);

    g_game.service.stop();
}

PAA_END_CPP(game_over)
{
}

static void update_server_event()
{
    auto& tcp = g_game.service.tcp();
    auto& udp = g_game.service.udp();

    shared_message_t msg;
    while (tcp.poll(msg) || udp.poll(msg)) {
        switch (msg->code()) {
        default:
            break;
        }
    }
}

PAA_UPDATE_CPP(game_over)
{
    g_game.use_hud_view();

    
    if (g_game.in_transition()) {
        g_game.transition.update();
        if (exitGameOver && g_game.transition_is_halfway()) {
            PAA_SET_SCENE(client_connect);
        }
    } else {
        if (keyboard->isButtonPressed(RTYPE_SHOOT_BUTTON)) {
            exitGameOver = true;
            g_game.launch_transition();
        }
    }


    PAA_SCREEN.draw(gameOverText);
    PAA_SCREEN.draw(retryText);
    PAA_SCREEN.draw(scoreText);
    g_game.transition.draw();

    g_game.use_game_view();
}

PAA_EVENTS_CPP(game_over) { }
