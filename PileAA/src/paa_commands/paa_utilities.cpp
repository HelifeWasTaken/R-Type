#include "PileAA/paa_commands/paa_utilities.hpp"
#include "PileAA/InputHandler.hpp"
#include "PileAA/InputManager.hpp"
#include "PileAA/paa_commands/paa_getters.hpp"

namespace paa {

template <typename T, typename... Args>
static inline T& internal_paa_setup_base_and_get(
    PAA_ENTITY entity, Args&&... args)
{
    hl::silva::registry& registry = PAA_ECS;
    registry.emplace<T>(entity, std::forward<Args>(args)...);
    return registry.get_component<T>(entity);
}

paa::AnimatedSprite& internal_paa_set_sprite(
    PAA_ENTITY entity, const std::string& name)
{
    hl::silva::registry& registry = PAA_ECS;
    registry.emplace<AnimatedSprite>(entity, name).emplace_r<Depth>(0);
    auto& sprite = registry.get_component<Sprite>(entity);
    PAA_ANIMATION_REGISTER.setAnimationToSpriteIfExist(name, sprite);
    return sprite;
}

ControllerKeyboard& internal_paa_setup_base_keyboard(PAA_ENTITY entity)
{
    static const std::array<Keyboard::Key, 10> keys = { Keyboard::Key::Q,
        Keyboard::Key::Z, Keyboard::Key::X, Keyboard::Key::E, Keyboard::Key::C,
        Keyboard::Key::R, Keyboard::Key::F, Keyboard::Key::V, Keyboard::Key::T,
        Keyboard::Key::G };

    auto& controller
        = internal_paa_setup_base_and_get<ControllerKeyboard>(entity);

    for (unsigned int id = 0; id < 10; ++id) {
        controller.setKey(id, keys[id]);
    }
    controller.setAxis(
        Joystick::Axis::X, Keyboard::Key::Left, Keyboard::Key::Right);
    controller.setAxis(
        Joystick::Axis::Y, Keyboard::Key::Up, Keyboard::Key::Down);
    return controller;
}

InputManagement& internal_paa_setup_base_input(PAA_ENTITY entity)
{
    return internal_paa_setup_base_and_get<InputManagement>(entity);
}

}