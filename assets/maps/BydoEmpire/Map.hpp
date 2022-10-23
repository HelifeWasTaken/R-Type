#pragma once
#include "PileAA/ResourceManager.hpp"
#include "PileAA/external/nlohmann/json.hpp"
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <fstream>
#include <ostream>

class BoxCollision {
public:
    /**
     * @brief  Default Constructor
     * @param  x: x position of the box
     * @param  y: y position of the box
     * @param  width: width of the box
     * @param  height: height of the box
     */
    BoxCollision(float x, float y, float width, float height)
        : _x(x)
        , _y(y)
        , _width(width)
        , _height(height)
    {
    }
    /**
     * @brief  Default Destructor
     */
    ~BoxCollision() = default;
    /**
     * @brief  Display data colision
     * @note
     * @param  box: x, y, width, height values displayed
     * @retval None
     */
    void printCollisionData(std::ostream& os, const BoxCollision& box) const
    {
        os << "x:\t" << box.getX() << "\n"
           << "y:\t" << box.getY() << "\n"
           << "width:\t" << box.getWidth() << "\n"
           << "height:\t" << box.getHeight() << "\n"
           << "▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓\n";
    }
    /**
     * @brief Get the X object
     * @return float: x
     */
    float getX() const { return (_x); }
    /**
     * @brief Get the Y object
     * @return float: y
     */
    float getY() const { return (_y); }
    /**
     * @brief Get the Width object
     * @return float: width
     */
    float getWidth() const { return (_width); }
    /**
     * @brief Get the Height object
     * @return float: height
     */
    float getHeight() const { return (_height); }

private:
    float _x = 0.0;
    float _y = 0.0;
    float _width = 0.0;
    float _height = 0.0;
};

class EffectZones {
public:
    struct EffectZoneData {
        BoxCollision box;
        std::string type;
        nlohmann::json properties;

        EffectZoneData(BoxCollision box, std::string type,
            nlohmann::json properties = nlohmann::json())
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

    void addEffect(BoxCollision box, std::string type,
        nlohmann::json properties = nlohmann::json())
    {
        _effects.push_back(std::unique_ptr<EffectZoneData>(
            new EffectZoneData(box, type, properties)));
    }

    // Should launch the good effect if the player is in and remove it
    // TODO
    void launchEffectIfPlayerIn(const BoxCollision& box) { }

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

    void addWaveData(
        std::string enemy_type, uint64_t enemy_id, float x, float y)
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
public:
    using BackgroundList = std::vector<sf::Image*>;
    using CollisionList = std::vector<std::shared_ptr<BoxCollision>>;

private:
    EffectZones _zones; // All the effect zones each of them should
                        // have a type in custom properties

    WaveManager _waves; // Every waves can be detected
                        // You should load the layer if the EffectZones has it

    BackgroundList _images; // TODO Check if this is safe
    CollisionList _map_collisions; // All the collisions of the map

    /**
     * @brief  Parse the collision data from the json file and stock it in
     * vector
     * @retval None
     */
    void parseEffects(const std::string& filepath)
    {
        std::ifstream ifs(filepath);
        nlohmann::json json;

        ifs >> json;

        for (int i = 0; json["layers"][i] != nullptr; i++) {
            const auto& layer = json["layers"][i];
            if (layer["name"] == "collisions") {
                for (const auto& j : layer["objects"]) {
                    this->_map_collisions.push_back(
                        std::make_shared<BoxCollision>(
                            j["x"], j["y"], j["width"], j["height"]));
                }
            } else if (layer["name"] == "EffectZones") {
                for (const auto& j : layer["objects"]) {
                    nlohmann::json properties = j.find("properties") != j.end()
                        ? j["properties"]
                        : nlohmann::json();
                    _zones.addEffect(
                        BoxCollision(j["x"], j["y"], j["width"], j["height"]),
                        j["type"], properties);
                }
            } else if (layer["type"] == "imagelayer") {
                paa::ResourceManagerInstance::get().load<sf::Image>(
                    layer["image"], layer["name"]);
                auto& image
                    = paa::ResourceManagerInstance::get().get<sf::Image>(
                        layer["name"]);
                _images.push_back(&image);
            }

            for (const auto& zone : _zones.getEffects()) {
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
        }
    }

public:
    /**
     * @brief Loads the ma
     * @param  filepath: The path to the file
     * @retval None
     */
    void loadMap(const std::string& filepath) { parseEffects(filepath); }

    /**
     * @brief  Get the std::vector of the parsed map
     * @retval std::vector<BoxCollision*>: The vector of the parsed map
     */
    const CollisionList& getMapParsed() { return (_map_collisions); }
};