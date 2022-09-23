#pragma once

#ifdef RTYPE_BUILDING_CLIENT
    #include <SFML/Graphics.hpp>
#endif

namespace rtype {
class IBullet {
public:
    /**
     * @brief Know if the bullet is still alive
     * @return true if the bullet is still alive, false otherwise
     */
    virtual bool timed_out() const = 0;

    /**
     * @brief Know if the bullet should disappear when colliding
     * @return true if the bullet should disappear when colliding, false otherwise
     */
    virtual bool destroy_on_collision() const = 0;

    /**
     * @brief Know if the bullet is out of the given surface
     * @return true if the bullet is out of the surface, false otherwise
     */
    virtual bool oob(const unsigned int& top, const unsigned int& left, const unsigned int& width, const unsigned& height) const = 0;

    /**
     * @brief Know if the bullet is colliding with the given rectangle
     * @return true if the bullet is colliding with the rectangle, false otherwise
     */
    virtual bool collides(const unsigned int& top, const unsigned int& left, const unsigned int& width, const unsigned& height) const = 0;

#ifdef RTYPE_BUILDING_CLIENT
    /**
     * @brief Update the bullet behaviour
     */
    virtual void draw(sf::RenderWindow& window) = 0;
#endif

    /**
     * @brief Update the bullet behaviour
     */
    virtual void update() = 0;

    /**
     * @brief Constructor
     */
    IBullet() = default;

    /**
     * @brief Destructor
     */
    virtual ~IBullet() = default;
};
}
