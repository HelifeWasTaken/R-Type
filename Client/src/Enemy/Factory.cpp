#include "Enemies.hpp"
#include "Bullet.hpp"

namespace rtype {
    namespace game {

        PAA_ENTITY EnemyFactory::make_enemy_by_type(const std::string& enemy_type,
            const float x, const float y)
        {
            spdlog::info("Creating enemy of type {} at ({}, {})", enemy_type, x, y);
            if (enemy_type == "basic_enemy")
                return make_basic_enemy(x, y);
            else if (enemy_type == "key_enemy")
                return make_key_enemy(x, y);
            else
                throw std::runtime_error(std::string("Enemy ") +
                    enemy_type + " not found");
        }

    }
}