#pragma once

#include "ClientScenes.hpp"
#include "PileAA/InputBox.hpp"

PAA_SCENE(connect_room)
{
    PAA_SCENE_DEFAULT(connect_room);
    
    paa::Text inputBoxTitle;
    paa::Text inputBoxContent;
    paa::InputBox inputBoxManager;
    paa::Text statusText;
    paa::Text actionText;
    bool isWaitingForReply;
    bool isStatusShown;

    PAA_START;
    PAA_UPDATE;
    PAA_END;

    paa::Font& font = PAA_RESOURCE_MANAGER.get<paa::Font>("font");

    paa::Gui gui;
    paa::shared_gui<paa::InputText> input
        = paa::GuiFactory::new_input_text("#", "Room token");
    paa::shared_gui<paa::GuiText> text
        = paa::GuiFactory::new_gui_text("No log from server yet");
};