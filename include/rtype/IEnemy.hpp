#pragma once

#ifdef RTYPE_BUILDING_CLIENT
    #include <SFML/Graphics.hpp>
#endif

#include <algorithm>

#include "IShooter.hpp"
#include "BulletPool.hpp"

namespace rtype {

class LivingObject {
public:

    /**
     * @brief Tells whether the object is alive or not
     * @return true if the object is alive, false otherwise
     */
    bool is_alive() const { return health > 0; }

    /**
     * @brief Damages the object
     */
    void take_damage(const unsigned int& damage) { health = std::abs(0, health - damage); }

    /**
     * @brief Heals the object
     */
    void heal(const unsigned int& amount) { health += amount; }

    /**
     * @brief Gets the heal amount
     */
    unsigned int get_health() const { return health; }

    /**
     * @brief Set the position of the object
     */
    void set_position(const double& x, const double& y) { position = {x, y}; }
    /**
     * @brief Set the position of the object
     */
    void set_position(const std::pair<double, double>& pos) { position = pos; }
    /**
     * @brief Get the position of the object
     */
    const std::pair<double, double> &get_position() const { return position; }
    /**
     * @brief Get the position of the object x
     */
    double get_position_x() const { return position.first; }
    /**
     * @brief Get the position of the object y
     */
    double get_position_y() const { return position.second; }

private:
    std::pair<double, double> position;
    unsigned int health = 0;
};

class IEnemy : public LivingObject {
public:
    /**
     * @brief Updates the current enemy
     */
    virtual void update() = 0;

    /**
     * @brief Set the shooter of the enemy
     */
    virtual void set_shooter(IShooter *shooter) = 0;

    /**
     * @brief Get the sprite of the enemy
     */
    virtual bool collide() = 0;

#ifdef RTYPE_BUILDING_CLIENT
    /**
     * @brief Draw the enemy
     */
    virtual void draw(sf::RenderWindow& window) = 0;
#endif

    /**
     * @brief Constructor
     */
    IEnemy() = default;

    /**
     * @brief Destructor
     */
    virtual ~IEnemy() = default;
};
}
