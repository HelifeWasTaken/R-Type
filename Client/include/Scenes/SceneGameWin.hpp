#pragma once

#include "ClientScenes.hpp"
#include "Map.hpp"

PAA_SCENE(game_win)
{
    PAA_SCENE_DEFAULT(game_win);

    PAA_START;
    PAA_END;

    PAA_EVENTS;
    PAA_UPDATE;

    unsigned int map_index = 0;

    std::unique_ptr<rtype::game::Map> map;

    paa::Font& font = PAA_RESOURCE_MANAGER.get<paa::Font>("font");

    paa::Text endText;

    int localPlayerTargetId = 0;

    paa::DeltaTimer deltaTimer;
    paa::Timer showMoreShipsTimer;
    paa::Timer doubleShipsAmountTimer;
    paa::Timer cinematicEndTimer;
    std::vector<hl::silva::Entity> backgroundShips;
};
