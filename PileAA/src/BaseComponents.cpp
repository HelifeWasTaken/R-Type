#include "PileAA/BaseComponents.hpp"
#include "PileAA/BatchRenderer.hpp"

namespace paa {

    static inline void animated_sprite_system(hl::silva::registry& r)
    {
        auto& batch = BatchRendererInstance::get();

        for (auto&& [_, sprite, depth] : r.view<Sprite, Depth>()) {
            sprite.update();
            batch.add(&sprite, depth);
        }
    }

    void setup_ecs(hl::silva::registry& r)
    {
        r.register_component<Position, Velocity,
                            Sprite, AnimatedSprite, Depth>();

        r.add_system(animated_sprite_system);
    }


}