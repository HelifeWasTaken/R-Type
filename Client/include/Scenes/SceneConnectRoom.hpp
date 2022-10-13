#pragma once

#include "ClientScenes.hpp"

PAA_SCENE(connect_room) {

    std::string _roomToken = "";

    PAA_START(connect_room);
    PAA_UPDATE;
    PAA_END(connect_room) = default;
};