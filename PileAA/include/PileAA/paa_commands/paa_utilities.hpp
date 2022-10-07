#pragma once

#include "paa_command_ecs.hpp"
#include "paa_getters.hpp"

namespace paa {

    void internal_paa_set_sprite(PAA_ENTITY entity, const std::string& name);
    paa::KeyboardController internal_paa_setup_keyboard(PAA_ENTITY entity);

    template<typename T, typename ...Args>
    T& internal_paa_setup_base_and_get(PAA_ENTITY entity, Args&&... args) {
        hl::silva::registry& registry = PAA_ECS;
        registry.emplace<T>(entity, std::forward<Args>(args)...);
        return registry.get<T>(entity);
    }

}

#define PAA_SET_SPRITE(entity, textureName) paa::internal_paa_set_sprite(entity, textureName)
#define PAA_SET_KEYBOARD(entity) paa::internal_paa_setup_keyboard(entity)
#define PAA_SET_CONTROLLER(entity, id) paa::internal_paa_setup_base_and_get<ControllerJoystick>(entity, id)
#define PAA_SET_INPUT_MANAGER(entity) paa::internal_paa_setup_base_and_get<InputManager>(entity)