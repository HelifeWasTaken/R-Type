#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace rtype {
namespace game {

    class EffectZones {
    public:
        struct EffectZoneData {
            unsigned int scroll_index;
            std::string type;
            std::string name;

            EffectZoneData(unsigned int scroll_index, const std::string& type,
                const std::string& name)
                : scroll_index(scroll_index)
                , type(type)
                , name(name)
            {
            }
        };

        using EffectZoneList = std::vector<std::unique_ptr<EffectZoneData>>;

    private:
        EffectZoneList _effects;

    public:
        void addEffect(const unsigned int scroll_index, const std::string& type,
            const std::string& name)
        {
            _effects.emplace_back(
                std::make_unique<EffectZoneData>(scroll_index, type, name));
        }

        EffectZoneList& getEffects() { return (_effects); }
    };

    class Wave {
    public:
        struct WaveData {
            std::string enemy_type;
            uint64_t enemy_id;
            float x, y;

            WaveData(const std::string& enemy_type, uint64_t enemy_id, float x,
                float y)
                : enemy_type(enemy_type)
                , enemy_id(enemy_id)
                , x(x)
                , y(y)
            {
            }
        };

    private:
        std::vector<std::unique_ptr<WaveData>> _waves;

    public:
        Wave() = default;

        void addWaveData(
            const std::string& enemy_type, uint64_t enemy_id, float x, float y);
        void activateWave();
    };

    class WaveManager {
    private:
        std::unordered_map<std::string, std::unique_ptr<Wave>> _wave;

    public:
        WaveManager() = default;

        void activateWave(const std::string& name);

        void addWave(const std::string& name, std::unique_ptr<Wave>&& wave);
    };

    // For the collisions use a BoxCollision

    class Map {
    private:
        WaveManager
            _waves; // Every waves can be detected
                    // You should load the layer if the EffectZones has it
        EffectZones _zones;

        /**
         * @brief  Parse the map info and generates entities to match it
         * @retval None
         */
        void loadMap(const std::string& filepath);

    public:
        /**
         * @brief  Constructor
         * @param  filepath: Path to the map file
         * @retval None
         */
        Map(const std::string& filepath) { loadMap(filepath); }

        /**
         * @brief  Update the map
         * @retval None
         */
        void update();
    };

}
}
