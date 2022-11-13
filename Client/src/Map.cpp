#include "Map.hpp"

#include "Collisions.hpp"
#include "PileAA/DynamicEntity.hpp"
#include "PileAA/QuadTree.hpp"
#include "PileAA/ResourceManager.hpp"
#include "PileAA/MusicPlayer.hpp"
#include "PileAA/external/nlohmann/json.hpp"

#include "ClientScenes.hpp"
#include "Enemies.hpp"
#include "Player.hpp"
#include <filesystem>
#include <fstream>

namespace rtype {
namespace game {

    void Wave::addWaveData(
        const std::string& enemy_type, uint64_t enemy_id, float x, float y)
    {
        _waves.push_back(
            std::make_unique<WaveData>(enemy_type, enemy_id, x, y));
    }

    void Wave::activateWave()
    {
        static const float ratios[] = { 1., 1.25, 1.75, 2 };
        int connected_players = -1;

        for (int i = 0; i < 4; i++) {
            connected_players += g_game.is_player_connected(i);
        }

        for (const auto& wave : _waves) {
            paa::DynamicEntity e
                = rtype::game::EnemyFactory::make_enemy_by_type(
                    wave->enemy_type, wave->x, wave->y);
            g_game.enemies_to_entities[e.attachId(wave->enemy_id).id]
                = e.getEntity();
            if (connected_players == -1)
                continue;
            if (e.hasComponent<paa::Health>()) {
                auto& hp = e.getComponent<paa::Health>().hp;
                int to_add = static_cast<float>(hp) * ratios[connected_players];
            }
        }
    }

    static void load_map_effect_zones(
        const nlohmann::json& layer, EffectZones& zones)
    {
        for (const auto& p : layer["properties"]) {
            const std::string name = p["name"];
            const std::string value = p["value"];
            const std::string type = value.substr(0, value.find(","));
            const std::string scroll_index = value.substr(value.find(",") + 1);
            zones.addEffect(std::stoi(scroll_index), type, name);
            spdlog::info("Effect zone: ({}) ({}) ({})", type, name, scroll_index);
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

    static void load_map_wave(
        const nlohmann::json& layer, WaveManager& waveManager)
    {
        std::unique_ptr<Wave> wave(new Wave);

        for (const auto& j : layer["objects"]) {
                try {
                wave->addWaveData(j.at("type"), j["id"], j["x"], j["y"]);
            } catch (...) {
                wave->addWaveData(j.at("class"), j["id"], j["x"], j["y"]);
            }
        }
        waveManager.addWave(layer["name"], std::move(wave));
    }

    void Map::loadMap(const std::string& filepath)
    {
        const std::string old_path = std::filesystem::current_path().string();
        const std::string path = std::filesystem::path(filepath).parent_path().string();
        const std::string file = std::filesystem::path(filepath).filename().string();

        g_game.scroll_speed = DEFAULT_SCROLL_SPEED;

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

    static void activate_wave_event(WaveManager& wave, const std::string& name)
    {
        wave.activateWave(name);
    }

    static void activate_play_music_event(EffectZones::EffectZoneData& effect)
    {
        // "name|{"loop": true, "offset": 42, "length": 69}"

        bool loop = true;
        double offset = -1;
        double length = -1;

        auto index = effect.name.find('|');
        if (index == std::string::npos) {
            paa::GMusicPlayer::play(effect.name);
            return;
        }
        std::string name = effect.name.substr(0, index);
        std::string json = effect.name.substr(index + 1);

        try {
            auto j = nlohmann::json::parse(json);
            try {
                loop = j.at("loop").get<bool>();
            } catch (...) {}
            try {
                offset = j.at("offset").get<double>();
            } catch (...) {}
            try {
                length = j.at("length").get<double>();
            } catch (...) {}
        } catch (...) {
        }
        if (offset != -1 || length != -1) {
            if (offset < 0 || length < 0) {
                spdlog::warn("Invalid offset/length for music {} ignoring length/offset", name);
            } else {
                paa::GMusicPlayer::setLoopPoints(offset, length);
            }
        }
        paa::GMusicPlayer::play(name, loop);
    }

    static void lock_scroll_event()
    {
        g_game.lock_scroll = true;
    }

    static void set_health(int health)
    {
        for (int i = 0; i < RTYPE_PLAYER_COUNT; i++) {
            if (g_game.connected_players[i] && g_game.players_alive[i]) {
                PAA_GET_COMPONENT(g_game.players_entities[i], paa::Health).hp = health;
            }
        }
    }

    void Map::update()
    {
        if (g_game.everyone_is_dead()) {
            return;
        }

        auto& effects = _zones.getEffects();
        std::vector<int> to_delete;

        for (int i = 0; i < effects.size(); i++) {
            auto& effect = effects[i];
            if (effect->scroll_index <= g_game.scroll) {
                if (effect->type == "activate_wave") {
                    activate_wave_event(_waves, effect->name);
                } else if (effect->type == "lock_scroll") {
                    lock_scroll_event();
                } else if (effect->type.starts_with("end")) {
                    g_game.launch_transition(effect->type.find("short") == std::string::npos);
                    _changes = true;
                } else if (effect->type == "launch_music") {
                    activate_play_music_event(*effect);
                } else if (effect->type == "show_gui") {
                    g_game.show_gui = true;
                } else if (effect->type == "hide_gui") {
                    g_game.show_gui = false;
                } else if (effect->type == "reset_health") {
                    set_health(APlayer::MAX_HEALTH);
                } else if (effect->type.starts_with("set_health=")) {
                    set_health(std::atoi(effect->type.c_str() + 11));
                } else if (effect->type.starts_with("scroll_speed=")) {
                    g_game.scroll_speed = std::atoi(effect->type.c_str() + 13);
                }
                to_delete.push_back(i - to_delete.size());
                spdlog::info("effect {} of type {} activated", effect->name,
                    effect->type);
            }
        }

        // Sync everyone when effects zones are activated
        if (to_delete.size() && g_game.is_host) {
            g_game.service.tcp().send(
                net::UpdateMessage(g_game.id, SerializedScroll(static_cast<int>(g_game.scroll)),
                net::message_code::UPDATE_SCROLL)
            );
        }
        for (int i = 0; i < to_delete.size(); i++) {
            effects.erase(effects.begin() + to_delete[i]);
        }
    }

    bool Map::changes()
    {
        return _changes;
    }

    void WaveManager::activateWave(const std::string& name)
    {
        spdlog::info("Activating wave {}", name);
        _wave[name]->activateWave();
        _wave.erase(name);
    }

    void WaveManager::addWave(
        const std::string& name, std::unique_ptr<Wave>&& wave)
    {
        _wave[name] = std::move(wave);
    }

}
}
