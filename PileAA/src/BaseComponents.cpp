#include "PileAA/BaseComponents.hpp"

namespace paa {

    static inline void animated_sprite_system(hl::silva::registry& r)
    {
        for (auto&& [_, sprite] : r.view<Sprite>()) {
            sprite.update();
        }
    }

    void setup_ecs(hl::silva::registry& r)
    {
        r.register_component<Position, Velocity, Sprite, AnimatedSprite>();
        r.add_system(animated_sprite_system);
    }


}