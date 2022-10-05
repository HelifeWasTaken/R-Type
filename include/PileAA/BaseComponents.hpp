#pragma once

#include <Silva>
#include "AnimatedSprite.hpp"

namespace paa {

struct Vec2 { float x, y; };
struct Position : public Vec2 {};
struct Velocity : public Vec2 {};

static inline void animated_sprite_system(hl::silva::registry& r)
{
    for (auto&& [_, sprite] : r.view<Sprite>()) {
        sprite.update();
    }
}

}