#pragma once
#include <fstream>
#include <iostream>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "../../../PileAA/include/PileAA/external/nlohmann/json.hpp"

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
    void getCollisionData(BoxCollision* box)
    {
        std::cout << "x:\t" << box->getX() << "\n"
                  << "y:\t" << box->getY() << "\n"
                  << "width:\t" << box->getWidth() << "\n"
                  << "height:\t" << box->getHeight() << "\n"
                  << "▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓\n";
    }
    /**
     * @brief Get the X object
     * @return float: x
     */
    float getX() { return (_x); }
    /**
     * @brief Get the Y object
     * @return float: y
     */
    float getY() { return (_y); }
    /**
     * @brief Get the Width object
     * @return float: width
     */
    float getWidth() { return (_width); }
    /**
     * @brief Get the Height object
     * @return float: height
     */
    float getHeight() { return (_height); }

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
    };

private:
    std::vector<std::unique_ptr<EffectZoneData>> _effects;

public:
    /**
     * @brief  Add an effect zone to the map
     * @param  data: Structure containing the collision box and the type of effect
     * @retval None
     */
    void addEffect(EffectZoneData* data)
    {
        _effects.push_back(std::unique_ptr<EffectZoneData>(data));
    }

    // Should launch the good effect if the player is in and remove it
    // TODO
    void launchEffectIfPlayerIn(const BoxCollision& box) { }
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
};

// For the collisions use a BoxCollision

class Map {
public:
    /**
     * @brief  Parse the collision data from the json file and stock it in vector
     * @retval None
     */
    void parse_collision_data()
    {
        int i = 0;
        std::ifstream ifs("../../../assets/maps/BydoEmpire/BydoMap.json");
        nlohmann::json json;

        ifs >> json;

        while (true) {
            if (json["layers"][i]["name"] == "collisions") {
                for (auto& i : json["layers"][i]["objects"]) {
                    BoxCollision* new_box = new BoxCollision(
                        i["x"], i["y"], i["width"], i["height"]);
                    this->_map_collisions.push_back(new_box);
                }
                break;
            }
            i++;
        }
    }
    /**
     * @brief  Get the std::vector of the parsed map
     * @retval std::vector<BoxCollision*>: The vector of the parsed map
     */
    const std::vector<BoxCollision*> getMapParsed()
    {
        return (_map_collisions);
    }
    /**
     * @brief  Load Image Background from JSON file
     * @retval None
     */
    void loadImageBackground()
    {
        std::ifstream ifs("../../../assets/maps/BydoEmpire/BydoMap.json");
        nlohmann::json json;

        ifs >> json;
        _image.loadFromFile(json["layers"][0]["image"]);
    }

private:
    EffectZones _zones; // All the effect zones each of them should
                        // have a type in custom properties

    WaveManager _waves; // Every waves can be detected
                        // You should load the layer if the EffectZones has it

    sf::Image _image; // The image layer to load
    std::vector<BoxCollision*> _map_collisions; // All the collisions of the map
};