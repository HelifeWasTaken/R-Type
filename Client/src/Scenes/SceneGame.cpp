
#include "ClientScenes.hpp"

using namespace rtype::net;

#include "RServer/Messages/Types.hpp"
#include "Bullet.hpp"

static PAA_SCENE_DECL(game_scene) *self = nullptr;

static void update_vector2_movement_other_player(const shared_message_t& msg)
{
    auto rep = parse_message<UpdateMessage>(msg.get());

    if (!rep) {
        spdlog::error("Failed to parse UpdateMessage");
        return;
    }
    const vector2i pos(rep->data());
    spdlog::info("Received update from player {} at position ({}, {})", rep->sid(), pos.x, pos.y);
    PAA_GET_COMPONENT(g_game.players_entities[rep->sid()], paa::Sprite)->move(pos.x, pos.y);
}

static void update_user_disconnect(const shared_message_t& msg)
{
    auto rep = parse_message<UserDisconnectFromRoom>(msg.get());

    if (!rep) {
        spdlog::error("Failed to parse UserDisconnectFromRoom");
        return;
    }
    spdlog::info("Client: Player {} disconnected from room new host is {}", rep->get_disconnected_user_id(), rep->get_new_host_id());
    if (rep->get_new_host_id() == g_game.id) {
        spdlog::info("Client: I'm the new host");
        g_game.is_host = true;
    }
    PAA_DESTROY_ENTITY(g_game.players_entities[rep->get_disconnected_user_id()]);
    g_game.connected_players[rep->get_disconnected_user_id()] = false;
}

static void update_client_shoot(const shared_message_t& msg)
{
    auto rep = parse_message<UpdateMessage>(msg.get());

    if (!rep) {
        spdlog::error("Failed to parse ClientShoot");
        return;
    }
    BulletQuery bq(rep->data());
    spdlog::info("Client: Player {} shoot", rep->sid());
    BulletFactory::create(bq);
}

static void server_event_update(void)
{
    auto& tcp = g_game.service.tcp();
    auto& udp = g_game.service.udp();
    shared_message_t msg;

    while (tcp.poll(msg) || udp.poll(msg)) {
        spdlog::info("Received message from server of code: {}", (int)msg->code());
        switch (msg->code()) {
        case message_code::UPDATE_VECTOR2_MOVEMENT: update_vector2_movement_other_player(msg); break;
        case message_code::ROOM_CLIENT_DISCONNECT:  update_user_disconnect(msg); break;
        case message_code::PLAYER_SHOOT:            update_client_shoot(msg); break;
        default:                                    spdlog::info("Client game_scene: Unknown message code {}", (int)msg->code()); break;
        }
    }
}

PAA_ENTITY new_player(int id)
{
    PAA_ENTITY e = PAA_NEW_ENTITY();
    paa::AnimatedSprite& s = PAA_SET_SPRITE(e, "spaceship");

    s.setPosition(rand() % 500, rand() % 500);
    s.useAnimation("player" + std::to_string(id + 1));
    return e;
}

bool space = false;
paa::Timer timer_shooter;

PAA_START_CPP(game_scene)
{
    self = this;

    BulletFactory::setup_systems();
    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        if (g_game.connected_players[i]) {
            g_game.players_entities[i] = new_player(i);
        }
    }
    timer_shooter.setTarget(20); // wait 20ms for each
}


PAA_UPDATE_CPP(game_scene)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);

    server_event_update();
    if (movement.x != 0 || movement.y != 0) {
        g_game.service.udp().send(UpdateMessage(g_game.id, vector2i(movement.x, movement.y), message_code::UPDATE_VECTOR2_MOVEMENT));
        PAA_GET_COMPONENT(g_game.players_entities[g_game.id], paa::Sprite)->move(movement.x, movement.y);
    }
    if (space && timer_shooter.isFinished()) {
        BulletQuery query(BulletType::BASIC_BULLET, g_game.id);
        timer_shooter.restart();
        query.create_then_send_message();
    }
}

PAA_EVENTS_CPP(game_scene)
{
    movement.x = input.isKeyDown(paa::Keyboard::Key::Right) - input.isKeyDown(paa::Keyboard::Key::Left);
    movement.y = input.isKeyDown(paa::Keyboard::Key::Down) - input.isKeyDown(paa::Keyboard::Key::Up);
    space = input.isKeyDown(paa::Keyboard::Key::Space) || input.isKeyPressed(paa::Keyboard::Key::Space);
}