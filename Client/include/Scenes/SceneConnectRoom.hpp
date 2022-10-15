#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(connect_room) {
    PAA_START(connect_room);
    PAA_UPDATE;
    PAA_END(connect_room) = default;

    paa::Gui gui;
    paa::shared_gui<paa::InputText> input = paa::GuiFactory::new_input_text("#", "Room token");
    paa::shared_gui<paa::GuiText> text = paa::GuiFactory::new_gui_text("No log from server yet");
};