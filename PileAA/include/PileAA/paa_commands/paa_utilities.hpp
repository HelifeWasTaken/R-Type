#pragma once

#include "paa_command_ecs.hpp"
#include "PileAA/InputManager.hpp"
#include <string>

namespace paa {
    class AnimatedSprite;
    class InputManagement;
}


namespace hl {
    namespace silva {
        class Entity;
        class registry;
    }
}

namespace paa {
    AnimatedSprite& internal_paa_set_sprite(PAA_ENTITY entity, const std::string& name);
    ControllerKeyboard& internal_paa_setup_keyboard(PAA_ENTITY entity);
    InputManagement& internal_paa_setup_input(PAA_ENTITY entity);
}

#define PAA_SET_SPRITE(entity, textureName) paa::internal_paa_set_sprite(entity, textureName)
#define PAA_SET_KEYBOARD(entity) paa::internal_paa_setup_keyboard(entity)
#define PAA_SET_INPUT_MANAGER(entity) paa::internal_paa_setup_input(entity)