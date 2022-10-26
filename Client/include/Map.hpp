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
    void loadMap(const std::string& filepath)
    {
        EffectZones zones; // All the effect zones each of them should
                            // have a type in custom properties

        std::string old_path = std::filesystem::current_path();
        std::string path = std::filesystem::path(filepath).parent_path();
        std::string file = std::filesystem::path(filepath).filename();

        std::cout << "Loading map: " << filepath << std::endl;
        std::cout << "Path: " << path << std::endl;
        std::cout << "File: " << file << std::endl;

        std::filesystem::current_path(path);

        std::ifstream ifs(file);
        nlohmann::json json;

        ifs >> json;

        for (int i = 0; json["layers"][i] != nullptr; i++) {
            const auto& layer = json["layers"][i];
            if (layer["name"] == "collisions") {
                for (const auto& j : layer["objects"]) {
                    paa::DynamicEntity e = PAA_NEW_ENTITY();
                    auto w = CollisionFactory::makeStaticWallCollision(
                        paa::IntRect(j["x"], j["y"], j["width"], j["height"]),
                        e.getEntity());
                    e.emplaceComponent<paa::SCollisionBox>(w);
                }
            } else if (layer["name"] == "EffectZones") {
                for (const auto& j : layer["objects"]) {
                    nlohmann::json properties = j.find("properties") != j.end()
                        ? j["properties"]
                        : nlohmann::json();

                    paa::DynamicEntity e = PAA_NEW_ENTITY();
                    auto w = CollisionFactory::makeEffectZoneCollision(
                            paa::IntRect(j["x"], j["y"], j["width"], j["height"]),
                            e.getEntity());
                    e.emplaceComponent<paa::SCollisionBox>(w);
                    e.emplaceComponent<EffectZones::EffectZoneData>(
                        EffectZones::EffectZoneData(
                            w,
                            j["type"],
                            properties
                        )
                    );
                    zones.addEffect(w, j["type"], properties);
                }
            } else if (layer["type"] == "imagelayer") {
                paa::ResourceManagerInstance::get().load<sf::Texture>(
                    layer["image"], layer["name"]);
                paa::DynamicEntity e = PAA_NEW_ENTITY();
                e.attachSprite(layer["name"]);
                e.getComponent<paa::Depth>().z = -1;
            }
        }

        for (const auto& zone : zones.getEffects()) {
            if (zone->type != "wave")
                continue;
            for (const auto& layers : json["layers"]) {
                if (layers["name"] != zone->properties["value"])
                    continue;
                std::unique_ptr<Wave> wave = std::make_unique<Wave>();
                for (const auto& j : layers["objects"]) {
                    wave->addWaveData(j["type"], j["id"], j["x"], j["y"]);
                }
                _waves.addWave(zone->properties["value"], std::move(wave));
            }
        }

        std::filesystem::current_path(old_path);
    }
};

}
}