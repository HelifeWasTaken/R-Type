#pragma once

#include "ClientWrapper.hpp"
#include "PileAA/AnimatedSprite.hpp"
#include "PileAA/App.hpp"
#include "PileAA/GUI.hpp"
#include "RServer/Client/Client.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <unordered_map>

struct Game {
    RTYPE_CLIENT service;
    rtype::net::Bool is_host = false;
    rtype::net::ClientID id = 0;
    std::array<rtype::net::Bool, RTYPE_PLAYER_COUNT> connected_players = { false };
    std::array<PAA_ENTITY, RTYPE_PLAYER_COUNT> players_entities;
    std::string room_token = "";

    int old_scroll = 0;
    int scroll = 0;
};

extern Game g_game;

#define GO_TO_SCENE_IF_CLIENT_DISCONNECTED(client, scene)                      \
    {                                                                          \
        if ((client).is_service_on() == false) {                               \
            spdlog::error("Client: A service is down");                        \
            PAA_SET_SCENE(scene);                                              \
            return;                                                            \
        }                                                                      \
    }

#include "Scenes/SceneClientConnect.hpp"
#include "Scenes/SceneConnectRoom.hpp"
#include "Scenes/SceneCreateRoom.hpp"
#include "Scenes/SceneGame.hpp"
#include "Scenes/SceneRoomWait.hpp"
