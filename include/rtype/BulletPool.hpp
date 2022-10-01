#pragma once

#include <memory>
#include <vector>

#ifdef RTYPE_BUILDING_CLIENT
    #include <SFML/Graphics.hpp>
#endif

#include "IBullet.hpp"

namespace rtype {
/**
 * @brief A class to store every bullet effectively
 */
class BulletPool {
private:
    /**
     * @brief A vector of bullets
     */
    std::vector<std::unique_ptr<IBullet>> _bullets;

public:
    /**
     * @brief A constructor
     * @param bullet A bullet to add to the pool
     */
    void add_bullet(IBullet* bullet)
    {
        _bullets.push_back(std::unique_ptr<IBullet>(bullet));
    }

    /**
     * @brief A method to update every bullet
     * @param sf::IntRect A rectangle to check if the bullet is out of the screen
     */
    void update(const unsigned int& top, const unsigned int& left, const unsigned int& width, const unsigned int& height)
    {
        const auto f = [&](IBullet &bullet) -> bool {
            return bullet.oob(top, left, width, height) || bullet.timed_out();
        };
        size_t i = 0;

        while (i < _bullets.size()) {
            if (f(*_bullets[i])) {
                size_t j = i+1;
                for (; j < _bullets.size() && f(*_bullets[j]); ++j);
                _bullets.erase(_bullets.begin() + i, _bullets.begin() + j - 1);
                i -= j - i;
            } else {
                i++;
            }
        }

        for (auto& bullet : _bullets) {
            bullet->update();
        }
    }

#ifdef RTYPE_BUILDING_CLIENT
    /**
     * @brief A method to draw every bullet
     * @param sf::RenderWindow A window to draw the bullets
     */
    void draw(sf::RenderWindow& window)
    {
        for (auto& bullet : _bullets) {
            bullet->draw(window);
        }
    }
#endif

    /**
     * @brief A method to know if the given rect is colliding with a bullet
     *        It will also remove the bullet if it is colliding and the bullet
     *        has the remove_on_collision flag set to true
     * @param sf::IntRect A rectangle to check if it is colliding with a bullet
     * @param bool A boolean to force the bullet to be removed
     * @return bool True if the given rect is colliding with a bullet
     */
    bool collides(const unsigned int& top, const unsigned int& left, const unsigned int& width, const unsigned int& height, const bool& force_destruction=false)
    {
        for (size_t i = 0; i < _bullets.size(); ++i) {
            IBullet& it = *_bullets[i];
            if (it.collides(top, left, width, height)) {
                if (it.destroy_on_collision() || force_destruction) {
                    _bullets.erase(_bullets.begin() + i);
                }
                return true;
            }
        }
        return false;
    }
};
}

