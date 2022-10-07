#include "PileAA/BaseComponents.hpp"
#include "PileAA/BatchRenderer.hpp"
#include "PileAA/InputManager.hpp"

namespace paa {

static inline void animated_sprite_system(hl::silva::registry& r)
{
    auto& batch = BatchRendererInstance::get();

    for (auto&& [_, sprite, depth] : r.view<AnimatedSprite, Depth>()) {
        sprite.update();
        batch.add(&sprite, depth);
    }
}

static inline void controller_input_manager_system(hl::silva::registry& r)
{
    for (auto&& [e, input, controller] :
        r.view<InputManagement, ControllerJoystick>()) {
        input.update(e, controller);
    }
    for (auto&& [e, input, controller] :
        r.view<InputManagement, ControllerKeyboard>()) {
        input.update(e, controller);
    }
}

void setup_ecs(hl::silva::registry& r)
{
    r.register_component<Position, Velocity, Sprite, AnimatedSprite, Depth>();

    r.add_system(animated_sprite_system);
}

}