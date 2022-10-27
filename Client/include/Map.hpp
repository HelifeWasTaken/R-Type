#pragma once

#include "PileAA/ResourceManager.hpp"
#include "PileAA/external/nlohmann/json.hpp"
#include "PileAA/QuadTree.hpp"
#include "PileAA/DynamicEntity.hpp"
#include "Collisions.hpp"

#include <filesystem>
#include <fstream>

namespace rtype {
namespace game {

class EffectZones {
public:
    struct EffectZoneData {
        paa::SCollisionBox box;
        std::string type;
        nlohmann::json properties;

        EffectZoneData(paa::SCollisionBox box, const std::string &type,
            const nlohmann::json &properties = nlohmann::json())
            : box(box)
            , type(type)
            , properties(properties)
        {
        }
    };

private:
    std::vector<std::unique_ptr<EffectZoneData>> _effects;

public:
    /**
     * @brief  Add an effect zone to the map
     * @param  data: Structure containing the collision box and the type of
     * effect
     * @retval None
     */
    void addEffect(EffectZoneData* data)
    {
        _effects.push_back(std::unique_ptr<EffectZoneData>(data));
    }

    void addEffect(paa::SCollisionBox box, std::string type,
        nlohmann::json properties = nlohmann::json())
    {
        _effects.push_back(std::unique_ptr<EffectZoneData>(
            new EffectZoneData(box, type, properties)));
    }

    // Should launch the good effect if the player is in and remove it
    // TODO
    void launchEffectIfPlayerIn(const paa::SCollisionBox& box) { }

    /**
     * @brief  Get the Effects object
     * @return std::vector<std::unique_ptr<EffectZoneData>>&: vector of
     * EffectZoneData
     */
    std::vector<std::unique_ptr<EffectZoneData>>& getEffects()
    {
        return (_effects);
    }
};

class Wave {
public:
    struct WaveData {
        std::string enemy_type;
        uint64_t enemy_id;
        float x, y;
    };

private:
    std::vector<std::unique_ptr<WaveData>> _effects;

public:
    /**
     * @brief  Add a wave of enemies to the map
     * @param  data: Structure containing enemy type, id and position
     * @retval None
     */
    void addWaveData(WaveData* data)
    {
        _effects.push_back(std::unique_ptr<WaveData>(data));
    }

    void addWaveData(const std::string& enemy_type,
                    uint64_t enemy_id, float x, float y)
    {
        addWaveData(new WaveData { enemy_type, enemy_id, x, y });
    }

    // TODO
    void activateWave() { }
};

class WaveManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Wave>> _wave;

public:
    // Should remove the wave after
    /**
     * @brief  Activate a wave
     * @param  name: Name of the wave
     * @retval None
     */
    void activateWave(const std::string& name)
    {
        _wave[name]->activateWave();
        _wave.erase(name);
    }

    /**
     * @brief  Add a wave to the map
     * @param  name: Name of the wave
     * @param  wave: Wave to add
     * @retval None
     */
    void addWave(const std::string& name, Wave* wave)
    {
        _wave[name] = std::unique_ptr<Wave>(wave);
    }

    /**
     * @brief  Get the Waves object
     * @return std::unordered_map<std::string, std::unique_ptr<Wave>>&: Map of
     * waves
     */
    void addWave(const std::string& name, std::unique_ptr<Wave>&& wave)
    {
        _wave[name] = std::move(wave);
    }
};

// For the collisions use a BoxCollision

class Map {
private:
    WaveManager _waves; // Every waves can be detected
                        // You should load the layer if the EffectZones has it

public:
    /**
     * @brief  Parse the map info and generates entities to match it
     * @retval None
     */
    void loadMap(const std::string& filepath);
};

}
}
