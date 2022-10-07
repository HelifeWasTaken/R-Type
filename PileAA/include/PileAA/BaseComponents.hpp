#pragma once

#include "AnimatedSprite.hpp"
#include "external/HelifeWasTaken/Silva"

namespace paa {

struct Vec2 {
    float x, y;
};
struct Position : public Vec2 {
};
struct Velocity : public Vec2 {
};
struct Depth {
    int z;
};

void setup_ecs(hl::silva::registry& r);

}