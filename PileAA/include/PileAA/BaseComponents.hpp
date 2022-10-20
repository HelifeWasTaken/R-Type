#pragma once

#include "AnimatedSprite.hpp"
#include "external/HelifeWasTaken/Silva"

namespace paa {

struct Vec2 {
    double x;
    double y;

    Vec2(const double& x=0, const double& y=0) : x(x), y(y) {}
    Vec2(const Vec2& other) = default;
    Vec2(const Vec2&& other) = default;

    ~Vec2() = default;

    Vec2 operator+(const Vec2& other) { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(const Vec2& other) { return Vec2(x * other.x, y * other.y); }
    Vec2 operator/(const Vec2& other) { return Vec2(x / other.x, y / other.y); }
    Vec2 operator*(const double& f) { return *this * (Vec2(f, f)); }
    Vec2 operator/(const double& f) { return *this / (Vec2(f, f)); }

    Vec2& operator-=(const Vec2& other) { return *this = *this - other; }
    Vec2& operator*=(const Vec2& other) { return *this = *this * other; }
    Vec2& operator/=(const Vec2& other) { return *this = *this / other; }
    Vec2& operator+=(const Vec2& other) { return *this = *this + other; }
    Vec2& operator*=(const double& f) { return *this = *this * f; }
    Vec2& operator/=(const double& f) { return *this = *this / f; }

    void move(const Vec2& offset) { *this = *this + offset; }
    void setPosition(const Vec2& other) { *this = other; }
};

struct Position : public Vec2 {
    Position(const double& x=0, const double& y=0)
        : Vec2(x, y)
    {}

    HL_AUTO_COMPLETE_CANONICAL_FORM(Position);
};

struct Depth {
    int z;
};

struct Id {
    int id;
};

/**
 * @brief Setup the given ECS registry with the base components needed for any
 * game.
 */
void setup_ecs(hl::silva::registry& r);

}