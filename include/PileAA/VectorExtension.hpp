#include <ostream>
#include <SFML/Graphics.hpp>

/**
 * @brief Writes on ostream any Vector2<T>
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v)
{
    return os << "Vector2(" << v.x << ", " << v.y << ")";
}

/**
 * @brief Writes on ostream any Vector3<T>
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const sf::Vector3<T>& v)
{
    return os << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
}

/**
 * @brief Writes on ostream any Rect<T>
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const sf::Rect<T>& r)
{
    return os << "Rect(" << r.left << ", " << r.top << ", " << r.width << ", " << r.height << ")";
}
