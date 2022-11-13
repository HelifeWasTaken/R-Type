#pragma once

#include "ClientScenes.hpp"

void Game::reset_game_view()
{
    auto size = PAA_SCREEN.getSize();

    hud_view.setSize(800, 600);

    if (PAA_APP.isFullscreen() || (size.x > 800 && size.y > 600))
        hud_view.setViewport(sf::FloatRect(0, 0, 1 - 800 / size.x, 1 - 600 / size.y));
    else
        hud_view.setViewport(sf::FloatRect(0, 0, 800 / size.x, 600 / size.y));

    game_view = hud_view;
    game_view.setSize(ARCADE_SCREEN_SIZE_X, ARCADE_SCREEN_SIZE_Y);
    game_view.setCenter(ARCADE_SCREEN_VIEW_X, ARCADE_SCREEN_VIEW_Y);
}

void Game::use_game_view()
{
    PAA_SCREEN.setView(game_view);
}

void Game::use_hud_view()
{
    PAA_SCREEN.setView(hud_view);
}

const PAA_ENTITY Game::get_random_player() const
{
    if (everyone_is_dead())
        return PAA_ENTITY();
    int index = paa::Random::rand() % RTYPE_PLAYER_COUNT;

    for (; !is_player_alive_by_id(index);
            index = paa::Random::rand() % RTYPE_PLAYER_COUNT)
        ;
    return players_entities[index];
}

const bool Game::is_player_alive_by_id(const rtype::net::ClientID& id) const
{
    return connected_players[id] && players_alive[id];
}

const bool Game::is_player_alive_by_entity(const PAA_ENTITY& entity) const
{
    auto it = std::find(
            players_entities.begin(), players_entities.end(), entity);
    if (it == players_entities.end())
        return false;
    size_t index = it - players_entities.begin();
    return is_player_alive_by_id(index);
}

const bool Game::is_player_connected(const rtype::net::ClientID& id) const
{
    return connected_players[id];
}

const bool Game::everyone_is_dead() const
{
    for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
        if (is_player_alive_by_id(i))
            return false;
    }
    return true;
}

std::string Game::generate_hud_text_for_players_life() const
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

std::string Game::generate_hud_text_for_score() const
{
    return std::string("Score - ") + std::to_string(score) + " pts";
}

bool Game::in_transition() const
{
    return transition.finished() == false;
}

void Game::launch_transition(bool long_transition)
{
    transition.start(long_transition);
}

void Game::launch_transition_halfway()
{
    transition.start_halfway();
}

bool Game::transition_is_halfway() const
{
    return transition.phase_two();
}
