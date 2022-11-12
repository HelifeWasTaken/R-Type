#include "Bullet.hpp"
#include "Enemies.hpp"
#include <functional>
#include <unordered_map>

namespace rtype {
namespace game {

    static std::unordered_map<std::string, std::function<PAA_ENTITY(const float, const float)>> ENEMY_CREATOR = {
        { "basic_enemy", EnemyFactory::make_basic_enemy },
        { "key_enemy", EnemyFactory::make_key_enemy },
        { "mastodonte_enemy", EnemyFactory::make_mastodonte_enemy },
        { "dumby_boy_enemy", EnemyFactory::make_dumby_boy_enemy },
        { "skeleton_boss", EnemyFactory::make_skeleton_boss },
        { "centipede_boss", EnemyFactory::make_centipede_boss },
        { "robot_boss", EnemyFactory::make_robot_boss },
        { "mattis_boss", EnemyFactory::make_mattis_boss }
    };

    PAA_ENTITY EnemyFactory::make_enemy_by_type(
        const std::string& enemy_type, const float x, const float y)
    {
        spdlog::info("Creating enemy of type {} at ({}, {})", enemy_type, x, y);

        auto it = ENEMY_CREATOR.find(enemy_type);
        if (it == ENEMY_CREATOR.end()) {
            throw std::runtime_error("Unknown enemy type: " + enemy_type);
        }
        return it->second(x, y);
    }

}
}
