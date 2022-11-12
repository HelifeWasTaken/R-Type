#pragma once

#include "ClientScenes.hpp"
#include "PileAA/DynamicEntity.hpp"

PAA_SCENE(scene_start)
{

    PAA_START;
    PAA_END;
    PAA_UPDATE;

    paa::DynamicEntity logoEntity;
    paa::Timer timer;

    paa::Font& font = PAA_RESOURCE_MANAGER.get<paa::Font>("font");
};
