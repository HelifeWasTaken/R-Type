#include "Scenes/SceneClientConnect.hpp"

#include "PileAA/GUI.hpp"
#include "MenuParallax.hpp"

using namespace rtype::net;

static PAA_SCENE_DECL(client_connect) * self = nullptr;

static void first_connect() {
    self->isConnectionPending = true;
    spdlog::debug("started first connect");
    g_game.service.run("../Client.conf");
    self->isConnectionPending = false;
}

static void refresh_connect() {
    self->isConnectionPending = true;
    spdlog::debug("started refresh connect");
    g_game.service.run(g_game.service.host,
                        g_game.service.tcp_port,
                        g_game.service.udp_port);
    self->isConnectionPending = false;
}

static void auto_connect() {
    self->isConnectionPending = true;
    spdlog::debug("started auto connect");
    g_game.service.run();
    self->isConnectionPending = false;
}

static void async_connect(const std::string& connectType) {
    if (self->isConnectionPending)
        return;
    if (g_game.service.is_service_on()) {
        g_game.service.stop();
    }
    self->connectionTimeoutTimer.setTarget(10000);
    self->connectionTimeoutTimer.restart();
    self->connectionPendingTimer.restart();
    self->connectionPendingFrame = 0;
    if (self->connectThread != nullptr && !self->isThreadJoined)
        self->connectThread->join();
    self->isThreadJoined = false;
    if (connectType == "auto") {
        self->connectThread = boost::make_shared<boost::thread>(auto_connect);
    } else if (connectType == "first") {
        self->connectThread = boost::make_shared<boost::thread>(first_connect);
    } else if (connectType == "refresh") {
        self->connectThread = boost::make_shared<boost::thread>(refresh_connect);
    }
}

static bool connect_server_if_not_connected()
{
    if (!self->isConnectionPending && !g_game.service.tcp_is_connected() && self->timer.isFinished()) {
        self->isTypingIP = true;
        self->timer.restart();
    }

    if (g_game.service.is_service_on() == false)
        return false;

    auto& tcp = g_game.service.tcp();
    auto& udp = g_game.service.udp();
    shared_message_t msg;

    while (tcp.poll(msg) || udp.poll(msg))
        ; // Auto manage event polling and connection

    if (tcp.is_connected()) {
        if (!udp.is_connected() && self->timer.isFinished()) {
            udp.feed_request(tcp.token(), tcp.id());
            self->timer.restart();
            return false;
        } else if (udp.is_connected()) {
            self->text->setText("Connected to server!");
            return true;
        } else {
            return false;
        }
    // } else if (self->timer.isFinished()) {
    //    self->timer.restart();
    //    async_connect("auto"); // Restart the service so the tcp send another
    //                           // CONN_INIT request
    //    return false;
    }
    return false;
}

PAA_START_CPP(client_connect)
{
    self = this;

    paa::GMusicPlayer::playIfNotPlayed("../assets/menu.ogg", true);

    isThreadJoined = false;
    connectThread = nullptr;
    isTryingToReconnect = false;
    isTypingIP = false;
    cursorPos = 0;

    g_game.is_host = false;

    // Make sure the ECS clear
    PAA_ECS.clear();

    // create the parallax
    rtype::MenuParallax::recreate();

    logoEntity = PAA_NEW_ENTITY();
    logoEntity.attachSprite("logo");
    auto& s = logoEntity.getComponent<paa::Sprite>();
    s->setPosition(RTYPE_MENU_CENTERED_X(*s), 100);

    // Always restart on this
    async_connect("first");

    timer.setTarget(1000);

    // use HUD View in case the player was in game
    g_game.use_hud_view();

    ipInputBoxManager.setMaxLength(300);

    ipInputBoxTitle.setCharacterSize(20);
    ipInputBoxTitle.setString("Enter the server address");
    ipInputBoxTitle.setFont(font);
    ipInputBoxTitle.setOutlineThickness(2);
    ipInputBoxTitle.setOutlineColor(sf::Color::White);
    ipInputBoxTitle.setFillColor(sf::Color::Blue);
    ipInputBoxTitle.setPosition(RTYPE_MENU_CENTERED_X(ipInputBoxTitle), 300);

    ipInputBoxContent.setCharacterSize(22);
    ipInputBoxContent.setString("_");
    ipInputBoxContent.setFont(font);
    ipInputBoxContent.setOutlineThickness(2);
    ipInputBoxContent.setOutlineColor(sf::Color::White);
    ipInputBoxContent.setFillColor(sf::Color::Red);

    connectionPendingFrame = 0;
    connectionPendingTimer.setTarget(100);
    connectionPendingTimer.restart();
    connectionPendingText.setCharacterSize(15);
    connectionPendingText.setString("Connecting o..");
    connectionPendingText.setFont(font);
    connectionPendingText.setOutlineThickness(2);
    connectionPendingText.setOutlineColor(sf::Color::White);
    connectionPendingText.setFillColor(sf::Color::Blue);
    connectionPendingText.setPosition(RTYPE_MENU_CENTERED_X(connectionPendingText), 350);

    auto& joinButton = buttons[0];
    joinButton.setCharacterSize(22);
    joinButton.setString("Join a party");
    joinButton.setFont(font);
    joinButton.setOutlineThickness(2);
    joinButton.setOutlineColor(sf::Color::White);
    joinButton.setFillColor(sf::Color::Red);
    joinButton.setPosition(RTYPE_MENU_CENTERED_X(joinButton), 300);
    actions[0] = [this]() {
        if (g_game.service.connected()) {
            PAA_SET_SCENE(connect_room);
        } else {
            text->setText("Cannot connect to room as server is not connected");
        }
    };

    auto& hostButton = buttons[1];
    hostButton.setCharacterSize(22);
    hostButton.setString("Host a party");
    hostButton.setFont(font);
    hostButton.setOutlineThickness(2);
    hostButton.setOutlineColor(sf::Color::White);
    hostButton.setFillColor(sf::Color::Red);
    hostButton.setPosition(RTYPE_MENU_CENTERED_X(hostButton), 350);
    actions[1] = [this]() {
        if (g_game.service.connected()) {
            PAA_SET_SCENE(create_room);
        } else {
            text->setText("Cannot host room as server is not connected");
        }
    };

    auto& settingsButton = buttons[2];
    settingsButton.setCharacterSize(22);
    settingsButton.setString("Settings");
    settingsButton.setFont(font);
    settingsButton.setOutlineThickness(2);
    settingsButton.setOutlineColor(sf::Color::White);
    settingsButton.setFillColor(sf::Color::Red);
    settingsButton.setPosition(RTYPE_MENU_CENTERED_X(settingsButton), 400);
    actions[2] = [this]() {
        isTypingIP = true;
    };

    cursor.setCharacterSize(12);
    cursor.setString("-");
    cursor.setFont(font);
    cursor.setOutlineThickness(2);
    cursor.setOutlineColor(sf::Color::White);
    cursor.setFillColor(sf::Color::Red);
}

PAA_END_CPP(client_connect)
{
    gui.clear();
    logoEntity.kill();
}

PAA_UPDATE_CPP(client_connect)
{
    g_game.use_hud_view();

    rtype::MenuParallax::update();

    if (PAA_INPUT.isKeyPressed(paa::Keyboard::F11)) {
        PAA_APP.setFullscreen(!PAA_APP.isFullscreen());
        g_game.reset_game_view();
        g_game.use_hud_view();
    }

    if (isConnectionPending || isTryingToReconnect || !timer.isFinished()) {
        if (isConnectionPending)
            timer.restart();
        if (timer.isFinished() && connectThread != nullptr && !isThreadJoined && connectionTimeoutTimer.isFinished()) {
            connectThread->interrupt();
            connectThread.reset();
            g_game.service.stop();
            isConnectionPending = false;
        }
        if (connectionPendingTimer.isFinished()) {
            connectionPendingTimer.restart();
            if (connectionPendingFrame < 3)
                connectionPendingFrame++;
            else
                connectionPendingFrame = 0;
        }

        if (connectionPendingFrame == 0)
            connectionPendingText.setString("Connecting o..");
        else if (connectionPendingFrame == 1)
            connectionPendingText.setString("Connecting .o.");
        else if (connectionPendingFrame == 2)
            connectionPendingText.setString("Connecting ..o");
        else if (connectionPendingFrame == 3)
            connectionPendingText.setString("Connecting .o.");

        PAA_SCREEN.draw(connectionPendingText);
        return;
    }

    if (isTypingIP) {

        ipInputBoxManager.setValue(g_game.service.host);
        ipInputBoxManager.update();
        g_game.service.host = ipInputBoxManager.getValue();

        if (ipInputBoxManager.isValidated()) {
            isTypingIP = false;
            ipInputBoxManager.setValidated(false);
            async_connect("refresh");
            text->setText("Trying to connect to server...");
            self->timer.restart();
        }

        ipInputBoxContent.setString(ipInputBoxManager.getValue());
        ipInputBoxContent.setPosition(RTYPE_MENU_CENTERED_X(ipInputBoxContent), 370);
        PAA_SCREEN.draw(ipInputBoxTitle);
        PAA_SCREEN.draw(ipInputBoxContent);
        return;
    }

    if (PAA_INPUT.isKeyPressed(paa::Keyboard::Up)) {
        clickSound.play();
        cursorPos--;
        if (cursorPos < 0)
            cursorPos = buttons.size() - 1;
    }
    if (PAA_INPUT.isKeyPressed(paa::Keyboard::Down)) {
        clickSound.play();
        cursorPos++;
        if (cursorPos >= buttons.size())
            cursorPos = 0;
    }

    for (int i = 0; i < buttons.size(); i++) {
        buttons[i].setFillColor(paa::Color::Blue);
    }
    buttons[cursorPos].setFillColor(paa::Color::Red);
    cursor.setPosition(RTYPE_MENU_CENTERED_X(buttons[cursorPos]) - 30, buttons[cursorPos].getPosition().y + 8);

    for (int i = 0; i < buttons.size(); i++)
        PAA_SCREEN.draw(buttons[i]);
    PAA_SCREEN.draw(cursor);

    if (PAA_INPUT.isKeyPressed(paa::Keyboard::Enter)) {
        clickSound.play();
        actions[cursorPos]();
    }

    gui.update();

    if (!isConnectionPending) {
        if (!g_game.service.connected())
            connect_server_if_not_connected();
        if (g_game.service.connected() && self->connectThread != nullptr && self->connectThread->joinable() && !isThreadJoined) {
            self->connectThread->join();
            isThreadJoined = true;
        }
    }
}
