#include "Map.hpp"

#include "PileAA/ResourceManager.hpp"
#include "PileAA/external/nlohmann/json.hpp"
#include "PileAA/QuadTree.hpp"
#include "PileAA/DynamicEntity.hpp"
#include "Collisions.hpp"

#include <filesystem>
#include <fstream>
#include "ClientScenes.hpp"
#include "Enemies.hpp"

namespace rtype {
namespace game {

    void Wave::addWaveData(const std::string& enemy_type,
                    uint64_t enemy_id, float x, float y)
    { _waves.push_back(
        std::make_unique<WaveData>(enemy_type, enemy_id, x, y)); }

    void Wave::activateWave() {
        for (const auto& wave : _waves) {
            paa::DynamicEntity e =
                rtype::game::EnemyFactory::make_enemy_by_type(
                wave->enemy_type, wave->x, wave->y);
            g_game.enemies_to_entities[e.attachId(wave->enemy_id).id] = e.getEntity();
        }
    }

    static void load_map_effect_zones(const nlohmann::json& layer,
                                    EffectZones& zones)
    {
        for (const auto& p : layer["properties"]) {
            const std::string type = p["name"];
            const std::string value = p["value"];
            const std::string name = value.substr(0, value.find(","));
            unsigned int scroll_index = std::stoi(value.substr(value.find(",") + 1, value.size()));
            zones.addEffect(scroll_index, type, name);
        }
    }

    static void load_map_collisions(const nlohmann::json& layer)
    {
        for (const auto& j : layer["objects"]) {
            paa::DynamicEntity e = PAA_NEW_ENTITY();
            auto w = CollisionFactory::makeStaticWallCollision(
                paa::IntRect(j["x"], j["y"], j["width"], j["height"]),
                e.getEntity());
            e.emplaceComponent<paa::SCollisionBox>(w);
        }
    }

    static void load_map_imagelayer(const nlohmann::json& layer)
    {
        paa::ResourceManagerInstance::get().load<sf::Texture>(
            layer["image"], layer["name"]);
        paa::DynamicEntity e = PAA_NEW_ENTITY();
        e.attachSprite(layer["name"]);
        e.getComponent<paa::Depth>().z = -1;
    }

    static void load_map_wave(const nlohmann::json& layer, WaveManager& waveManager)
    {
        std::unique_ptr<Wave> wave(new Wave);

        for (const auto& j : layer["objects"]) {
            wave->addWaveData(j["type"], j["id"], j["x"], j["y"]);
        }
        waveManager.addWave(layer["name"], std::move(wave));
    }

    void Map::loadMap(const std::string& filepath)
    {
        const std::string old_path = std::filesystem::current_path();
        const std::string path = std::filesystem::path(filepath).parent_path();
        const std::string file = std::filesystem::path(filepath).filename();

        std::filesystem::current_path(path);

        std::ifstream ifs(file);
        nlohmann::json json;

        ifs >> json;

        for (int i = 0; json["layers"][i] != nullptr; i++) {
            const auto& layer = json["layers"][i];
            if (layer["name"] == "collisions") {
                load_map_collisions(layer);
            } else if (layer["name"] == "EffectZones") {
                load_map_effect_zones(layer, _zones);
            } else if (layer["type"] == "imagelayer") {
                load_map_imagelayer(layer);
            }
        }

        for (int i = 0; json["layers"][i] != nullptr; i++) {
            const auto& layer = json["layers"][i];

            for (const auto& zone : _zones.getEffects()) {
                if (zone->name == layer["name"]) {
                    load_map_wave(layer, _waves);
                }
            }
        }

        std::filesystem::current_path(old_path);
    }

    void Map::update()
    {
        auto& effects = _zones.getEffects();
        std::vector<int> to_delete;

        for (int i = 0; i < effects.size(); i++) {
            auto& effect = effects[i];
            if (effect->scroll_index <= g_game.scroll) {
                if (effect->type == "activate_wave") {
                    _waves.activateWave(effect->name);
                }
                to_delete.push_back(i - to_delete.size());
            }
        }
        for (int i = 0; i < to_delete.size(); i++) {
            effects.erase(effects.begin() + to_delete[i]);
        }
    }

}
}
