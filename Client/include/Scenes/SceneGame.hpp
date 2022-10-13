#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(game_scene) {

    std::array<PAA_ENTITY, 4> players;

    PAA_START(game_scene);
    PAA_END(game_scene) = default;

    PAA_EVENTS;
    PAA_UPDATE;

};