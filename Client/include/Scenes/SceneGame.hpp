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

    unsigned int map_index = 0;

    std::unique_ptr<rtype::game::Map> map;

    paa::Font& font = PAA_RESOURCE_MANAGER.get<paa::Font>("font");

    paa::Text lifeText;
    paa::Text scoreText;
};
