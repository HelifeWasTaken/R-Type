#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(game_scene)
{

    PAA_START(game_scene);
    PAA_END(game_scene) = default;

    PAA_EVENTS;
    PAA_UPDATE;

    paa::Vector2i movement = { 0, 0 };
};