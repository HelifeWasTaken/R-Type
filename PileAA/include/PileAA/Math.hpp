#pragma once
#include "Types.hpp"
#include <cmath>

namespace paa {

    #ifndef M_PI
    #define M_PI       3.14159265358979323846
    #endif

    struct Math {
        static paa::Vector2f angle_to_direction(float angle, const double scale=1)
        {
            return paa::Vector2f(std::cos(angle) * scale, std::sin(angle) * scale);
        }

        static float direction_to_angle(paa::Position const& pos)
        {
            return std::atan2(pos.y, pos.x) * 180 / M_PI;
        }

        template<typename T, typename U>
        static float direction_to_angle(T const& origin, U const& direction, const double scale=1)
        {
            const paa::Position fpos = paa::Position(
                origin.x - direction.x, origin.y - direction.y);
            return direction_to_angle(fpos);
        }
    };
}