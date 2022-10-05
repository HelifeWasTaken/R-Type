#include "PileAA/BaseComponents.hpp"

namespace paa {

    void animated_sprite_system(hl::silva::registry& r)
    {
        for (auto&& [_, sprite] : r.view<Sprite>()) {
            sprite.update();
        }
    }

}