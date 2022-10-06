#pragma once

#include "paa_getters.hpp"
#include "paa_command_ecs.hpp"

#define PAA_SET_SPRITE(entity, textureName) \
    { \
        PAA_SET_COMPONENT(entity, paa::Sprite, textureName); \
        PAA_SET_COMPONENT(entity, paa::Depth, 0); \
    }
