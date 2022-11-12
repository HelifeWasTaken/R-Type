#pragma once

#include "ClientScenes.hpp"
#include <boost/atomic.hpp>
#include "PileAA/InputBox.hpp"
#include "PileAA/Parallax.hpp"
#include "PileAA/DynamicEntity.hpp"
#include <functional>

PAA_SCENE(client_connect)
{

    paa::Timer timer;
    paa::Gui gui;
    paa::shared_gui_text text
        = paa::GuiFactory::new_gui_text("No log from server yet");

    boost::shared_ptr<boost::thread> connectThread;
    boost::atomic_bool isConnectionPending;
    bool isTryingToReconnect;
    bool isThreadJoined;

    int connectionPendingFrame;
    paa::Timer connectionPendingTimer;
    paa::Timer connectionTimeoutTimer;
    paa::Text connectionPendingText;

    paa::Sound clickSound =
            paa::Sound(PAA_RESOURCE_MANAGER.get<paa::SoundBuffer>("menu_click"));

    bool isTypingIP;
    paa::Text ipInputBoxTitle;
    paa::Text ipInputBoxContent;
    paa::InputBox ipInputBoxManager;
    paa::DynamicEntity logoEntity;

    std::array<paa::Text, 3> buttons;
    std::array<std::function<void()>, 3> actions;
    paa::Text cursor;

    int cursorPos;

    paa::shared_gui<paa::InputText> inputIP;

    paa::Font& font = PAA_RESOURCE_MANAGER.get<paa::Font>("font");

    PAA_SCENE_DEFAULT(client_connect);

    PAA_START;
    PAA_END;

    PAA_UPDATE;
};
