#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(connect_room)
{
    PAA_SCENE_DEFAULT(connect_room);

    PAA_START;
    PAA_UPDATE;
    PAA_END;

    paa::Gui gui;
    paa::shared_gui<paa::InputText> input
        = paa::GuiFactory::new_input_text("#", "Room token");
    paa::shared_gui<paa::GuiText> text
        = paa::GuiFactory::new_gui_text("No log from server yet");
};