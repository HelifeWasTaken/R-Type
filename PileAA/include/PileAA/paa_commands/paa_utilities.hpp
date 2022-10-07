#pragma once

#include "paa_command_ecs.hpp"
#include "paa_getters.hpp"

#define PAA_SET_SPRITE(entity, textureName)                                    \
    {                                                                          \
        PAA_SET_COMPONENT(entity, paa::Sprite, textureName);                   \
        PAA_SET_COMPONENT(entity, paa::Depth, 0);                              \
        PAA_ANIMATION_REGISTER.setAnimationToSpriteIfExist(                    \
            textureName, PAA_GET_COMPONENT(entity, paa::Sprite));              \
    }
