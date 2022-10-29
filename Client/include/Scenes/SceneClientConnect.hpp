#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(client_connect)
{

    paa::Timer timer;
    paa::Gui gui;
    paa::shared_gui_text text
        = paa::GuiFactory::new_gui_text("No log from server yet");

    PAA_SCENE_DEFAULT(client_connect);

    PAA_START;
    PAA_END;

    PAA_UPDATE;
};
