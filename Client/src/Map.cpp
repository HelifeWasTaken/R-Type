#include "Map.hpp"

namespace rtype {
namespace game {

    void Map::loadMap(const std::string& filepath)
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
                    std::cout << "Emplacing Collision: " << *w << std::endl;
                    e.emplaceComponent<paa::SCollisionBox>(w);
                    // e.attachSprite("yo");
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

}
}
