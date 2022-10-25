#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(game_scene)
{
    PAA_SCENE_DEFAULT(game_scene);

    PAA_START;
    PAA_END;

    PAA_EVENTS;
    PAA_UPDATE;

    paa::Vector2i movement = { 0, 0 };
};