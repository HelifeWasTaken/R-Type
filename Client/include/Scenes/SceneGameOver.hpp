#pragma once

#include "ClientScenes.hpp"
#include "Map.hpp"
#include "PileAA/InputHandler.hpp"

PAA_SCENE(game_over)
{
    PAA_SCENE_DEFAULT(game_over);

    PAA_START;
    PAA_END;

    PAA_EVENTS;
    PAA_UPDATE;

    paa::Font& font = PAA_RESOURCE_MANAGER.get<paa::Font>("font");
    paa::Controller keyboard;

    paa::Text gameOverText;
    paa::Text retryText;
    paa::Text scoreText;
    bool exitGameOver = false;
};
