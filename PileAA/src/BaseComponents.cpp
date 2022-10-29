#include "PileAA/BaseComponents.hpp"
#include "PileAA/BatchRenderer.hpp"
#include "PileAA/InputManager.hpp"
#include "PileAA/QuadTree.hpp"

namespace paa {

static inline void sys_animated_sprite_system(hl::silva::registry& r)
{
    auto& batch = BatchRendererInstance::get();

    for (auto&& [_, sprite, depth] : r.view<Sprite, Depth>()) {
        sprite->update();
        batch.add(sprite, depth);
    }
}

static inline void sys_controller_input_manager_system(hl::silva::registry& r)
{
    for (auto&& [e, input, controller] :
        r.view<InputManagement, Controller>()) {
        input.update(e, *controller);
    }
}

static inline void sys_sprite_position_updater(hl::silva::registry& r)
{
    for (auto&& [_, v, s] : r.view<Position, Sprite>()) {
        s->setPosition(v.x, v.y);
    }
}

static inline void sys_collision_box_sync(hl::silva::registry& r)
{
    for (auto&& [_, c, s] : r.view<SCollisionBox, Sprite>()) {
        const auto r = s->getGlobalBounds();
        c->set_position(Vector2i(r.left, r.top));
        c->set_size(Vector2i(r.width, r.height));
    }
}

static inline void sys_collision_check(hl::silva::registry& r)
{
    const auto& screen = PAA_SCREEN;
    const auto view = screen.getView();
    const auto center = view.getCenter();
    const auto size = view.getSize();
    Quadtree q(center.x - size.x / 2, center.y - size.y / 2, size.x, size.y);

    for (auto&& [_, c] : r.view<SCollisionBox>()) {
        q.insert_collision(c.get());
    }
    q.check_collision();
}

void setup_ecs(hl::silva::registry& r)
{
    r.register_component<Position, Velocity, Sprite, Depth, Health,
         SCollisionBox, InputManagement, Controller, Id>()
        .add_system(sys_animated_sprite_system)
        .add_system(sys_controller_input_manager_system)
        .add_system(sys_sprite_position_updater)
        .add_system(sys_collision_box_sync)
        .add_system(sys_collision_check);
}

}
