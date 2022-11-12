#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(waiting_room)
{

    PAA_START;
    PAA_END;
    PAA_UPDATE;

    paa::Text roomCode;
    paa::Text actionText;
    paa::Text playersCount;
    paa::Gui gui;

    bool isWaitingForServer;

    paa::Font& font = PAA_RESOURCE_MANAGER.get<paa::Font>("font");

    paa::shared_gui_text server_log
        = paa::GuiFactory::new_gui_text("No log from server yet");
};
