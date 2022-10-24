
#include "ClientScenes.hpp"

using namespace rtype::net;

static PAA_SCENE_DECL(game_scene) * self = nullptr;

PAA_ENTITY new_player(int id)
{
    PAA_ENTITY e = PAA_NEW_ENTITY();
    paa::AnimatedSprite& s = PAA_SET_SPRITE(e, "spaceship");

    s.setPosition(rand() % 500, rand() % 500);
    s.useAnimation("player" + std::to_string(id + 1));
    return e;
}

PAA_START_CPP(game_scene)
{
    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        if (g_game.connected_players[i]) {
            g_game.players_entities[i] = new_player(i);
        }
    }
}

PAA_UPDATE_CPP(game_scene)
{
    GO_TO_SCENE_IF_CLIENT_DISCONNECTED(g_game.service, client_connect);
}

PAA_EVENTS_CPP(game_scene) { }
