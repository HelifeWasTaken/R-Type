#pragma once

#include "ClientWrapper.hpp"
#include "PileAA/AnimatedSprite.hpp"
#include "PileAA/App.hpp"
#include "PileAA/GUI.hpp"
#include "PileAA/Rand.hpp"
#include "RServer/Client/Client.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <unordered_map>

struct Game {
    RTYPE_CLIENT service;
    rtype::net::Bool is_host = false;
    rtype::net::ClientID id = 0;

    std::array<rtype::net::Bool, RTYPE_PLAYER_COUNT> connected_players
        = { false };
    std::array<PAA_ENTITY, RTYPE_PLAYER_COUNT> players_entities;
    std::array<rtype::net::Bool, RTYPE_PLAYER_COUNT> players_alive = { false };

    std::string room_token = "";
    std::unordered_map<int, PAA_ENTITY> enemies_to_entities;

    const PAA_ENTITY get_random_player() const
    {
        if (everyone_is_dead())
            return PAA_ENTITY();
        int index = paa::Random::rand() % RTYPE_PLAYER_COUNT;

        for (; !is_player_alive_by_id(index);
             index = paa::Random::rand() % RTYPE_PLAYER_COUNT)
            ;
        return players_entities[index];
    }

    const bool is_player_alive_by_id(const rtype::net::ClientID& id) const
    {
        return connected_players[id] && players_alive[id];
    }

    const bool is_player_alive_by_entity(const PAA_ENTITY& entity) const
    {
        auto it = std::find(
            players_entities.begin(), players_entities.end(), entity);
        if (it == players_entities.end())
            return false;
        size_t index = it - players_entities.begin();
        return is_player_alive_by_id(index);
    }

    const bool is_player_connected(const rtype::net::ClientID& id) const
    {
        return connected_players[id];
    }

    const bool everyone_is_dead() const
    {
        for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
            if (is_player_alive_by_id(i))
                return false;
        }
        return true;
    }

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

template <typename T>
class DestroyableElementSerializer : public rtype::net::Serializable {
private:
    T _element;

public:
    DestroyableElementSerializer() = default;
    DestroyableElementSerializer(const T& element)
        : _element(element)
    {
    }
    std::vector<rtype::net::Byte> serialize() const override
    {
        rtype::net::Serializer serializer;
        serializer << _element;
        return serializer.data;
    }
    void from(const rtype::net::Byte* data,
        const rtype::net::BufferSizeType size) override
    {
        rtype::net::Serializer serializer(data, size);
        serializer >> _element;
    }
    T& getElement() { return (_element); }
};

class SerializedEnemyDeath : public DestroyableElementSerializer<paa::u16> {
public:
    SerializedEnemyDeath() = default;
    SerializedEnemyDeath(const paa::u16& element)
        : DestroyableElementSerializer(element)
    {
    }
};

class SerializedPlayerDeath
    : public DestroyableElementSerializer<rtype::net::ClientID> {
public:
    SerializedPlayerDeath() = default;
    SerializedPlayerDeath(const rtype::net::ClientID& element)
        : DestroyableElementSerializer(element)
    {
    }
};

class SerializedScroll : public DestroyableElementSerializer<paa::i32> {
public:
    SerializedScroll() = default;
    SerializedScroll(const paa::i32& element)
        : DestroyableElementSerializer(element)
    {
    }
};

#include "Scenes/SceneClientConnect.hpp"
#include "Scenes/SceneConnectRoom.hpp"
#include "Scenes/SceneCreateRoom.hpp"
#include "Scenes/SceneGame.hpp"
#include "Scenes/SceneRoomWait.hpp"
