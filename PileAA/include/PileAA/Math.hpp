#pragma once
#include "Types.hpp"
#include <cmath>

namespace paa {

    #ifndef M_PI
    #define M_PI       3.14159265358979323846
    #endif

    struct Math {
        static float toRadians(float degrees) { return degrees * (M_PI / 180); }
        static float toDegrees(float radians) { return radians * (180 / M_PI); }

        static paa::Vector2f angle_to_direction(float angle, const double scale=1)
        {
            return paa::Vector2f(std::cos(angle) * scale, std::sin(angle) * scale);
        }

        template<typename V>
        static float direction_to_angle(V const& pos)
        {
            return toDegrees(std::atan2(pos.y, pos.x));
        }

        template<typename V1, typename V2>
        static float direction_to_angle(V1 const& origin, V2 const& direction)
        {
            const paa::Vector2f fpos = distance(origin, direction);
            return direction_to_angle(fpos);
        }

        template<typename V1, typename V2>
        static paa::Vector2f distance(V1 const& origin, V2 const& direction)
        {
            return paa::Vector2f(
                origin.x - direction.x, origin.y - direction.y);
        }

        template<typename V1, typename V2>
        static paa::Vector2f linear_interpolation(V1 const& origin,
            V2 const& direction, float t, float scale=1)
        {
            return paa::Vector2f(
                origin.x + (direction.x - origin.x) * t * scale,
                origin.y + (direction.y - origin.y) * t * scale);
        }

        template<typename V1, typename V2>
        static float distance_between_squared(V1 const& origin, V2 const& direction)
        {
            const paa::Vector2f fpos = distance(origin, direction);
            return fpos.x * fpos.x + fpos.y * fpos.y;
        }

        template<typename V1, typename V2>
        static float distance_between(V1 const& origin, V2 const& direction)
        {
            return std::sqrt(distance_between_squared(origin, direction));
        }

        template<typename V1, typename V2>
        static paa::Vector2f normalize(V1 const& origin, V2 const& direction)
        {
            return distance_between(origin, direction);
        }

        static paa::Vector2f linear(const float x)
        {
            return paa::Vector2f(x, x);
        }

        static paa::Vector2f reciprocal(const float x)
        {
            return paa::Vector2f(x, 1 / x);
        }

        static paa::Vector2f logarithmic(const float x)
        {
            return paa::Vector2f(std::log(x), std::log10(x));
        }

        static paa::Vector2f exponential(const float x)
        {
            return paa::Vector2f(std::exp(x), std::exp2(x));
        }

        static paa::Vector2f trigonometric(const float x)
        {
            return paa::Vector2f(std::sin(x), std::cos(x));
        }

        static paa::Vector2f hyperbolic(const float x)
        {
            return paa::Vector2f(std::sinh(x), std::cosh(x));
        }

        static paa::Vector2f sine(const float x)
        {
            return paa::Vector2f(std::sin(x), std::asin(x));
        }

        static paa::Vector2f cosine(const float x)
        {
            return paa::Vector2f(std::cos(x), std::acos(x));
        }

        static paa::Vector2f tangent(const float x)
        {
            return paa::Vector2f(std::tan(x), std::atan(x));
        }

        static paa::Vector2f easeInSine(const float x)
        {
            return paa::Vector2f(x, 1 - std::cos((x * M_PI) / 2));
        }

        static paa::Vector2f easeOutSine(const float x)
        {
            return paa::Vector2f(x, std::sin((x * M_PI) / 2));
        }

        static paa::Vector2f easeInOutSine(const float x)
        {
            return paa::Vector2f(x, -(std::cos(M_PI * x) - 1) / 2);
        }

        static paa::Vector2f easeInQuad(const float x)
        {
            return paa::Vector2f(x, x * x);
        }

        static paa::Vector2f easeOutQuad(const float x)
        {
            return paa::Vector2f(x, 1 - (1 - x) * (1 - x));
        }

        static paa::Vector2f easeInOutQuad(const float x)
        {
            return paa::Vector2f(x, x < 0.5 ? 2 * x * x : 1 - std::pow(-2 * x + 2, 2) / 2);
        }

        static paa::Vector2f easeInCubic(const float x)
        {
            return paa::Vector2f(x, x * x * x);
        }

        static paa::Vector2f easeOutCubic(const float x)
        {
            return paa::Vector2f(x, 1 - std::pow(1 - x, 3));
        }

        static paa::Vector2f easeInOutCubic(const float x)
        {
            return paa::Vector2f(x, x < 0.5 ? 4 * x * x * x : 1 - std::pow(-2 * x + 2, 3) / 2);
        }

        static paa::Vector2f easeInQuart(const float x)
        {
            return paa::Vector2f(x, x * x * x * x);
        }

        static paa::Vector2f easeOutQuart(const float x)
        {
            return paa::Vector2f(x, 1 - std::pow(1 - x, 4));
        }

        static paa::Vector2f easeInOutQuart(const float x)
        {
            return paa::Vector2f(x, x < 0.5 ? 8 * x * x * x * x : 1 - std::pow(-2 * x + 2, 4) / 2);
        }

        static paa::Vector2f easeInQuint(const float x)
        {
            return paa::Vector2f(x, x * x * x * x * x);
        }

        static paa::Vector2f easeOutQuint(const float x)
        {
            return paa::Vector2f(x, 1 - std::pow(1 - x, 5));
        }

        static paa::Vector2f easeInOutQuint(const float x)
        {
            return paa::Vector2f(x, x < 0.5 ? 16 * x * x * x * x * x : 1 - std::pow(-2 * x + 2, 5) / 2);
        }

        static paa::Vector2f easeInOutExpotential(const float x)
        {
            return paa::Vector2f(x, x == 0 ? 0 : x == 1 ? 1 : x < 0.5 ? std::pow(2, 20 * x - 10) / 2 : (2 - std::pow(2, -20 * x + 10)) / 2);
        }

        static paa::Vector2f easeInCirc(const float x)
        {
            return paa::Vector2f(x, 1 - std::sqrt(1 - std::pow(x, 2)));
        }

        static paa::Vector2f easeOutCirc(const float x)
        {
            return paa::Vector2f(x, std::sqrt(1 - std::pow(x - 1, 2)));
        }

        static paa::Vector2f easeInOutCirc(const float x)
        {
            return paa::Vector2f(x, x < 0.5 ? (1 - std::sqrt(1 - std::pow(2 * x, 2))) / 2 : (std::sqrt(1 - std::pow(-2 * x + 2, 2)) + 1) / 2);
        }

        static paa::Vector2f easeInBack(const float x)
        {
            static constexpr float c1 = 1.70158f;
            static constexpr float c3 = c1 + 1;

            return paa::Vector2f(x, c3 * x * x * x - c1 * x * x);
        }

        static paa::Vector2f easeOutBack(const float x)
        {
            static constexpr float c1 = 1.70158f;
            static constexpr float c3 = c1 + 1;

            return paa::Vector2f(x, 1 + c3 * std::pow(x - 1, 3) + c1 * std::pow(x - 1, 2));
        }

        static paa::Vector2f easeInOutBack(const float x)
        {
            static constexpr float c1 = 1.70158f;
            static constexpr float c2 = c1 * 1.525f;

            return paa::Vector2f(x, x < 0.5 ? (std::pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2 : (std::pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2);
        }

        static paa::Vector2f easeInElastic(const float x)
        {
            static constexpr float c4 = (2 * M_PI) / 3;

            return paa::Vector2f(x, x == 0 ? 0 : x == 1 ? 1 : -std::pow(2, 10 * x - 10) * std::sin((x * 10 - 10.75) * c4));
        }

        static paa::Vector2f easeOutElastic(const float x)
        {
            static constexpr float c4 = (2 * M_PI) / 3;

            return paa::Vector2f(x, x == 0 ? 0 : x == 1 ? 1 : std::pow(2, -10 * x) * std::sin((x * 10 - 0.75) * c4) + 1);
        }

        static paa::Vector2f easeInOutElastic(const float x)
        {
            static constexpr float c5 = (2 * M_PI) / 4.5f;

            return paa::Vector2f(x, x == 0 ? 0 : x == 1 ? 1 : x < 0.5 ? -(std::pow(2, 20 * x - 10) * std::sin((20 * x - 11.125) * c5)) / 2 : (std::pow(2, -20 * x + 10) * std::sin((20 * x - 11.125) * c5)) / 2 + 1);
        }

        static paa::Vector2f easeInBounce(const float x)
        {
            return paa::Vector2f(x, 1 - easeOutBounce(1 - x).y);
        }

        static paa::Vector2f easeOutBounce(float x)
        {
            static constexpr float n1 = 7.5625f;
            static constexpr float d1 = 2.75f;

            if (x < 1 / d1)
            {
                return paa::Vector2f(x, n1 * x * x);
            }
            else if (x < 2 / d1)
            {
                return paa::Vector2f(x, n1 * (x -= 1.5f / d1) * x + 0.75f);
            }
            else if (x < 2.5 / d1)
            {
                return paa::Vector2f(x, n1 * (x -= 2.25f / d1) * x + 0.9375f);
            }
            else
            {
                return paa::Vector2f(x, n1 * (x -= 2.625f / d1) * x + 0.984375f);
            }
        }

        static paa::Vector2f easeInOutBounce(const float x)
        {
            return paa::Vector2f(x, x < 0.5 ? (1 - easeOutBounce(1 - 2 * x).y) / 2 : (1 + easeOutBounce(2 * x - 1).y) / 2);
        }
    };
}