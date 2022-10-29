#pragma once

#include "PileAA/InputManager.hpp"
#include "paa_command_ecs.hpp"
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
/**
 * @brief Attach to an entity a texture attaches all the associated animtions
 * and give it a depth of 0
 * @return AnimatedSprite&
 */
AnimatedSprite& internal_paa_set_sprite(
    PAA_ENTITY entity, const std::string& name);

/**
 * @brief Setups a keyboard to default
 */
ControllerKeyboard& internal_paa_setup_keyboard(PAA_ENTITY entity);

/**
 * @brief Setups an input manager to use any controller
 */
InputManagement& internal_paa_setup_input(PAA_ENTITY entity);
}

/**
 * @brief c.f paa::internal_paa_set_sprite
 */
#define PAA_SET_SPRITE(entity, textureName)                                    \
    paa::internal_paa_set_sprite(entity, textureName)

/**
 * @brief c.f paa::internal_paa_setup_keyboard
 */
#define PAA_SET_KEYBOARD(entity) paa::internal_paa_setup_keyboard(entity)

/**
 * @brief c.f paa::internal_paa_setup_input
 */
#define PAA_SET_INPUT_MANAGER(entity) paa::internal_paa_setup_input(entity)
