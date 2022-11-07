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

    sf::View game_view;
    sf::View hud_view;

    void reset_game_view()
    {
        game_view = hud_view;
        game_view.setSize(ARCADE_SCREEN_SIZE_X, ARCADE_SCREEN_SIZE_Y);
        game_view.setCenter(ARCADE_SCREEN_VIEW_X, ARCADE_SCREEN_VIEW_Y);
    }

    void use_game_view()
    {
        PAA_SCREEN.setView(game_view);
    }

    void use_hud_view()
    {
        PAA_SCREEN.setView(hud_view);
    }

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

    std::string generate_hud_text_for_players_life() const
    {
        std::string s = "";

        for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
            s += "P" + std::to_string(i + 1) + " - ";
            if (is_player_connected(i)) {
                if (is_player_alive_by_entity(players_entities[i])) {
                    try {
                        const auto hp = PAA_GET_COMPONENT(players_entities[i], paa::Health).hp;
                        s += std::to_string(hp) + " hp";
                    } catch (...) {
                        s += "dead";
                    }
                } else {
                    s += "dead";
                }
            } else {
                s += "disconnected";
            }
            s += "\n\n";
        }
        return s;
    }

    std::string generate_hud_text_for_score() const
    {
        return std::string("Score - ") + std::to_string(score) + " pts";
    }

    BlackScreenTransition transition;

    bool in_transition() const
    {
        return transition.finished() == false;
    }

    void launch_transition()
    {
        transition.start();
    }

    bool transition_is_halfway() const
    {
        return transition.phase_two();
    }

    unsigned int score = 0;

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
#include "Scenes/SceneGameOver.hpp"
#include "Scenes/SceneRoomWait.hpp"
