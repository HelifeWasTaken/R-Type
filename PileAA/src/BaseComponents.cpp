#include "PileAA/BaseComponents.hpp"
#include "PileAA/BatchRenderer.hpp"
#include "PileAA/InputManager.hpp"

namespace paa {

static inline void animated_sprite_system(hl::silva::registry& r)
{
    auto& batch = BatchRendererInstance::get();

    for (auto&& [_, sprite, depth] : r.view<Sprite, Depth>()) {
        sprite->update();
        batch.add(sprite, depth);
    }
}

static inline void controller_input_manager_system(hl::silva::registry& r)
{
    for (auto&& [e, input, controller] :
        r.view<InputManagement, Controller>()) {
        input.update(e, *controller);
    }
}

static inline void sprite_position_updater(hl::silva::registry& r)
{
    for (auto&& [_, v, s] : r.view<Position, Sprite>()) {
        s->setPosition(v.x, v.y);
    }
}

void setup_ecs(hl::silva::registry& r)
{
    r.register_component<
        Position,
        Sprite, Depth,
        InputManagement,
        Controller
    >().add_system(animated_sprite_system)
        .add_system(controller_input_manager_system)
        .add_system(sprite_position_updater);
}

}
