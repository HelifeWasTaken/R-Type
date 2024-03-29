#pragma once

#include "ClientWrapper.hpp"
#include "PileAA/AnimatedSprite.hpp"
#include "PileAA/App.hpp"
#include "PileAA/GUI.hpp"
#include "PileAA/Rand.hpp"
#include "RServer/Client/Client.hpp"
#include "BlackScreenTransition.hpp"
#include "utils.hpp"
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <string>

#define DEFAULT_SCROLL_SPEED 1

struct Game {
    RTYPE_CLIENT service;
    rtype::net::Bool is_host = false;
    std::string server_address = "";
    rtype::net::ClientID id = 0;

    std::array<rtype::net::Bool, RTYPE_PLAYER_COUNT> connected_players
        = { false };
    std::array<PAA_ENTITY, RTYPE_PLAYER_COUNT> players_entities;
    std::array<rtype::net::Bool, RTYPE_PLAYER_COUNT> players_alive = { false };

    std::string room_token = "";
    std::unordered_map<int, PAA_ENTITY> enemies_to_entities;

    sf::View game_view;
    sf::View hud_view;

    void reset_game_view();

    void use_game_view();

    void use_hud_view();

    const PAA_ENTITY get_random_player() const;

    const bool is_player_alive_by_id(const rtype::net::ClientID& id) const;

    const bool is_player_alive_by_entity(const PAA_ENTITY& entity) const;

    const bool is_player_connected(const rtype::net::ClientID& id) const;

    const bool everyone_is_dead() const;

    std::string generate_hud_text_for_players_life() const;

    std::string generate_hud_text_for_score() const;

    BlackScreenTransition transition;

    bool in_transition() const;

    void launch_transition(bool long_transition=false);

    void launch_transition_halfway();

    bool transition_is_halfway() const;

    int score = 0;

    float old_scroll = 0;
    float scroll_speed = DEFAULT_SCROLL_SPEED;
    float scroll = 0;
    bool lock_scroll = false;
    bool show_gui = true;
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
#include "Scenes/SceneGameOver.hpp"
#include "Scenes/SceneGameWin.hpp"
#include "Scenes/SceneRoomWait.hpp"
#include "Scenes/SceneStart.hpp"
