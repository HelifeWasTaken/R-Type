#pragma once

#include "ClientScenes.hpp"
#include "Map.hpp"

PAA_SCENE(game_scene)
{
    PAA_SCENE_DEFAULT(game_scene);

    PAA_START;
    PAA_END;

    PAA_EVENTS;
    PAA_UPDATE;

    std::unique_ptr<rtype::game::Map> map;
};