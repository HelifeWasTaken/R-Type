#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(waiting_room) {

    PAA_START(waiting_room);
    PAA_END(waiting_room) = default;

    PAA_UPDATE;

    paa::Gui gui;

    paa::shared_button button;
    paa::shared_gui_text text = paa::GuiFactory::new_gui_text("No log from server yet");
};
