#include "PileAA/paa_commands/paa_utilities.hpp"
#include "PileAA/InputManager.hpp"

namespace paa {

    void internal_paa_set_sprite(PAA_ENTITY entity, const std::string& name) {
        hl::silva::registry& registry = PAA_ECS;
        registry.emplace<Sprite>(entity, name)
            .emplace_r<Depth>(0);
        PAA_ANIMATION_REGISTER.setAnimationToSpriteIfExist(
            textureName, registry.get<Sprite>(entity));
    }

    ControllerKeyboard& internal_paa_setup_base_keyboard(PAA_ENTITY entity) {
        hl::silva::registry& registry = PAA_ECS;

        static const std::array<Keyboard::Key, 10> keys = {
            Keyboard::Key::Q, Keyboard::Key::Z,
            Keyboard::Key::X, Keyboard::Key::E,
            Keyboard::Key::C, Keyboard::Key::R,
            Keyboard::Key::F, Keyboard::Key::V,
            Keyboard::Key::T, Keyboard::Key::G
        };

        registry.emplace<ControllerKeyboard>(entity);

        auto& controller = registry.get<ControllerKeyboard>(entity);
        for (unsigned int id = 0; id < 10; ++id) {
            controller.setKey(id, keys[id]);
        }
        controller.setAxis(Joystick::Axis::X, Keyboard::Key::Left, Keyboard::Key::Right);
        controller.setAxis(Joystick::Axis::Y, Keyboard::Key::Up, Keyboard::Key::Down);
        return controller;
    }

}