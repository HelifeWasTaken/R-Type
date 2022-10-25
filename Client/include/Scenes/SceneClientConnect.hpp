#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(client_connect)
{

    paa::Timer timer;
    paa::Gui gui;
    paa::shared_gui_text text
        = paa::GuiFactory::new_gui_text("No log from server yet");

    PAA_START(client_connect);
    PAA_UPDATE;
    PAA_END(client_connect) = default;
};
