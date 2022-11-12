#include "ClientScenes.hpp"
#include "RServer/Messages/Messages.hpp"
#include <boost/algorithm/string/case_conv.hpp>
#include "MenuParallax.hpp"

using namespace rtype::net;

static PAA_SCENE_DECL(connect_room) * self = nullptr;

static void set_action_text(const std::string& msg) {
    self->actionText.setString(msg);
    self->actionText.setPosition(RTYPE_MENU_CENTERED_X(self->actionText), 500);
}

static void show_status_message(const std::string& msg) {
    self->statusText.setString(msg);
    self->isStatusShown = true;
    self->statusText.setPosition(RTYPE_MENU_CENTERED_X(self->statusText), 250);
}

static void check_connect_room_reply(shared_message_t& msg)
{
    try {
        auto rep = parse_message<RequestConnectRoomReply>(msg.get());

        // Go back to choice connect or host if
        // failed to parse message or if player id is invalid
        if (!rep) {
            spdlog::error("Client: Failed to parse CONNECT_ROOM_REPLY message");
            show_status_message("Internal server error");
        } else if (rep->playerID() == RTYPE_INVALID_PLAYER_ID) {
            spdlog::error("Client: Failed to connect to room {}", g_game.room_token);
            show_status_message("The room is full or does not exist");
        } else {
            spdlog::info("Client: Connected to room as {}", rep->playerID());
            g_game.id = rep->playerID();
            show_status_message("Loading game...");
            PAA_SET_SCENE(waiting_room);
        }
    } catch (...) {
        show_status_message("Internal server error");
    }
}

static void manage_server_events(bool& isWaitingForReply)
{
    auto& tcp = g_game.service.tcp();
    shared_message_t msg;

    if (!tcp.poll(msg))
        return;

    switch (msg->code()) {
    case message_code::CONNECT_ROOM_REQ_REP:
        isWaitingForReply = false;
        check_connect_room_reply(msg);
        break;
    default:
        spdlog::info("Client connect_room: Received message of type {}",
            (int)msg->code());
        break;
    }
}

PAA_START_CPP(connect_room)
{
    self = this;

    isWaitingForReply = false;
    isStatusShown = false;

    inputBoxManager.setMaxLength(6);
    inputBoxManager.setValue("");

    inputBoxTitle.setCharacterSize(20);
    inputBoxTitle.setString("Enter room code");
    inputBoxTitle.setFont(font);
    inputBoxTitle.setOutlineThickness(2);
    inputBoxTitle.setOutlineColor(sf::Color::White);
    inputBoxTitle.setFillColor(sf::Color::Red);
    inputBoxTitle.setPosition(RTYPE_MENU_CENTERED_X(inputBoxTitle), 200);

    inputBoxContent.setCharacterSize(40);
    inputBoxContent.setString("_");
    inputBoxContent.setFont(font);
    inputBoxContent.setOutlineThickness(2);
    inputBoxContent.setOutlineColor(sf::Color::White);
    inputBoxContent.setFillColor(sf::Color::Red);
    
    statusText.setCharacterSize(15);
    statusText.setString("_");
    statusText.setFont(font);
    statusText.setOutlineThickness(2);
    statusText.setOutlineColor(sf::Color::White);
    statusText.setFillColor(sf::Color::Red);
    
    actionText.setCharacterSize(15);
    actionText.setString("_");
    actionText.setFont(font);
    actionText.setOutlineThickness(2);
    actionText.setOutlineColor(sf::Color::White);
    actionText.setFillColor(sf::Color::Blue);
}

PAA_END_CPP(connect_room) { gui.clear(); }

PAA_UPDATE_CPP(connect_room)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);
    rtype::MenuParallax::update();

    bool wasWaitingForReply = isWaitingForReply;
    manage_server_events(isWaitingForReply);
    
    if (!isWaitingForReply && wasWaitingForReply) {
        inputBoxManager.setValidated(false);
    }

    if (isStatusShown) {

        if (PAA_INPUT.isKeyPressed(paa::Keyboard::Enter) || PAA_INPUT.isKeyPressed(paa::Keyboard::Escape)) {
            inputBoxManager.setValue("");
            isStatusShown = false;
        }
        set_action_text("Press Enter to continue");
        PAA_SCREEN.draw(statusText);

    } else {
        
        inputBoxManager.update();

        inputBoxContent.setString(inputBoxManager.getValue());
        inputBoxContent.setPosition(RTYPE_MENU_CENTERED_X(inputBoxContent), 230);

        if (PAA_INPUT.isKeyPressed(paa::Keyboard::Escape)) {
            g_game.service.stop();
            PAA_SET_SCENE(client_connect);
        }
        if (!isWaitingForReply && inputBoxManager.isValidated()) {
            g_game.room_token = "#" + boost::to_lower_copy(inputBoxManager.getValue());
            spdlog::info("Joining room: {}", g_game.room_token);
            g_game.service.tcp().send(RequestConnectRoom(g_game.room_token));
            isWaitingForReply = true;
        }

        if (isWaitingForReply) {
            set_action_text("Please wait...");
        } else {
            set_action_text("Type and press Enter to confirm");
        }

        PAA_SCREEN.draw(inputBoxTitle);
        PAA_SCREEN.draw(inputBoxContent);

    }

    PAA_SCREEN.draw(actionText);

}
