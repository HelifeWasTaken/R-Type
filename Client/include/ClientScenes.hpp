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
    std::unordered_map<int, PAA_ENTITY> enemies_to_entities;

    int old_scroll = 0;
    int scroll = 0;
    bool lock_scroll = false;
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

template<typename T>
class DestroyableElementSerializer : public rtype::net::Serializable {
private:
    T _element;

public:
    DestroyableElementSerializer() = default;
    DestroyableElementSerializer(const T &element) : _element(element) {}
    std::vector<rtype::net::Byte> serialize() const override {
        rtype::net::Serializer serializer;
        serializer << _element;
        return serializer.data;
    }
    void from(const rtype::net::Byte *data, const rtype::net::BufferSizeType size) override {
        rtype::net::Serializer serializer(data, size);
        serializer >> _element;
    }
    T &getElement() { return (_element); }
};

class SerializedEnemyDeath : public DestroyableElementSerializer<paa::u16> {
public:
    SerializedEnemyDeath() = default;
    SerializedEnemyDeath(const paa::u16 &element)
        : DestroyableElementSerializer(element) {}
};

class SerializedPlayerDeath : public DestroyableElementSerializer<rtype::net::ClientID> {
public:
    SerializedPlayerDeath() = default;
    SerializedPlayerDeath(const rtype::net::ClientID &element)
        : DestroyableElementSerializer(element) {}
};

#include "Scenes/SceneClientConnect.hpp"
#include "Scenes/SceneConnectRoom.hpp"
#include "Scenes/SceneCreateRoom.hpp"
#include "Scenes/SceneGame.hpp"
#include "Scenes/SceneRoomWait.hpp"
