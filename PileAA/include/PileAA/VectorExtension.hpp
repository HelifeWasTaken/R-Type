#pragma once

#include "Types.hpp"
#include <SFML/Graphics.hpp>
#include <ostream>

/**
 * @brief Writes on ostream any Vector2<T>
 */
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const paa::Vector2<T>& v)
{
    return os << "Vector2(" << v.x << ", " << v.y << ")";
}

/**
 * @brief Writes on ostream any Vector3<T>
 */
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const paa::Vector3<T>& v)
{
    return os << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
}

/**
 * @brief Writes on ostream any Rect<T>
 */
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const paa::Rect<T>& r)
{
    return os << "Rect(" << r.left << ", " << r.top << ", " << r.width << ", "
              << r.height << ")";
}

namespace paa {
/**
 * @brief Transform a Vector2<T> to Vector2<U>
 */
template <typename T, typename U>
inline Vector2<T> vecTo(const Vector2<U>& other)
{
    return { static_cast<T>(other.x), static_cast<T>(other.y) };
}

/**
 * @brief Transform a Rect<T> to Rect<U>
 */
template <typename T, typename U> inline Rect<T> recTo(const Rect<U>& other)
{
    return { static_cast<T>(other.left), static_cast<T>(other.top),
        static_cast<T>(other.width), static_cast<T>(other.height) };
}

}