#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(client_connect) {

    paa::Timer _timer;
    std::string choice;

    PAA_START(client_connect);
    PAA_UPDATE;
    PAA_END(client_connect) = default;
};
