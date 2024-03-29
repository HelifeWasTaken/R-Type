#include "ClientScenes.hpp"
#include <boost/algorithm/string/case_conv.hpp>
#include "PileAA/MusicPlayer.hpp"
#include "MenuParallax.hpp"

using namespace rtype::net;

static PAA_SCENE_DECL(waiting_room) * self = nullptr;

static void set_action_text(const std::string& msg) {
    self->actionText.setString(msg);
    self->actionText.setPosition(RTYPE_MENU_CENTERED_X(self->actionText), 500);
}

static void manage_room_client_connect(shared_message_t msg)
{
    auto rep = parse_message<UserConnectRoom>(msg.get());
    if (!rep) {
        spdlog::error("Client: Failed to parse ROOM_CLIENT_CONNECT message");
        return;
    } else if (rep->playerID() == RTYPE_INVALID_PLAYER_ID) {
        spdlog::error("Client: A player tried to join: Room full.");
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
    spdlog::info("Client: Player {} disconnected from room",
        rep->get_disconnected_user_id());
    g_game.connected_players[rep->get_disconnected_user_id()] = false;
    if (rep->get_new_host_id() == g_game.id) {
        g_game.is_host = true;
        spdlog::info("Client: I am now the host of the room");
    }
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

static void manage_server_events()
{
    auto& tcp = g_game.service.tcp();
    shared_message_t msg;

    if (!tcp.poll(msg))
        return;

    switch (msg->code()) {
    case message_code::ROOM_CLIENT_CONNECT:
        return manage_room_client_connect(msg);
    case message_code::ROOM_CLIENT_DISCONNECT:
        return manage_room_client_disconnect(msg);
    case message_code::LAUNCH_GAME_REP:
        return manage_launch_game(msg);
    default:
        spdlog::info("Client waiting_room: Received message of type {}",
            (int)msg->code());
        break;
    }
}

///// ---------- /////

PAA_START_CPP(waiting_room)
{
    self = this;

    isWaitingForServer = false;

    std::memset(
        g_game.connected_players.data(), 0, g_game.connected_players.size());
    g_game.connected_players[g_game.id] = true;

    gui.addObject(paa::GuiFactory::new_button("Launch game", [this]() {
        if (g_game.is_host) {
            g_game.service.tcp().send(SignalMarker(message_code::LAUNCH_GAME));
            server_log->setText("Requested the server to launch the game...");
        } else {
            server_log->setText("You are not the host of the room");
        }
    }));
    gui.addObject(server_log);
    gui.addObject(paa::GuiFactory::new_gui_text("Token: " + g_game.room_token));
    gui.addObject(paa::GuiFactory::new_button(
        "Disconnect", []() { PAA_SET_SCENE(client_connect); }));

    roomCode.setCharacterSize(40);
    roomCode.setString(boost::to_upper_copy(std::string(g_game.room_token.begin() + 1, g_game.room_token.end())));
    roomCode.setFont(font);
    roomCode.setOutlineThickness(2);
    roomCode.setOutlineColor(sf::Color::White);
    roomCode.setFillColor(sf::Color::Red);
    roomCode.setPosition(RTYPE_MENU_CENTERED_X(roomCode), 230);

    actionText.setCharacterSize(15);
    actionText.setString("_");
    actionText.setFont(font);
    actionText.setOutlineThickness(2);
    actionText.setOutlineColor(sf::Color::White);
    actionText.setFillColor(sf::Color::Blue);

    playersCount.setCharacterSize(15);
    playersCount.setFont(font);
    playersCount.setOutlineThickness(2);
    playersCount.setOutlineColor(sf::Color::White);
    playersCount.setFillColor(sf::Color::Blue);
}

PAA_END_CPP(waiting_room) { gui.clear(); }

PAA_UPDATE_CPP(waiting_room)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);
    rtype::MenuParallax::update();
    manage_server_events();

    if (PAA_INPUT.isKeyPressed(paa::Keyboard::F11)) {
        PAA_APP.setFullscreen(!PAA_APP.isFullscreen());
        g_game.reset_game_view();
        g_game.use_hud_view();
    }

    if (g_game.is_host) {
        if (isWaitingForServer) {
            set_action_text("Requested the server to launch the game..");
        } else {
            set_action_text("Press Enter to start the game");
        }
    } else {
        set_action_text("Waiting for the host...");
    }

    if (g_game.is_host && !isWaitingForServer && PAA_INPUT.isKeyPressed(paa::Keyboard::Enter)) {
        g_game.service.tcp().send(SignalMarker(message_code::LAUNCH_GAME));
    }

    if (PAA_INPUT.isKeyPressed(paa::Keyboard::Escape)) {
        g_game.service.stop();
        PAA_SET_SCENE(client_connect);
    }

    int playersNb = 0;
    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++)
        if (g_game.connected_players[i])
            playersNb++;

    playersCount.setString(std::to_string(playersNb) + " players connected");
    playersCount.setPosition(RTYPE_MENU_CENTERED_X(playersCount), 300);

    PAA_SCREEN.draw(roomCode);
    PAA_SCREEN.draw(actionText);
    PAA_SCREEN.draw(playersCount);
}
