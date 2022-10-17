class EffectZones {
public:
    struct EffectZoneData {
        BoxCollision box;
        std::string type;
    };

private:
    std::vector<std::unique_ptr<EffectZoneData>> _effects;

public:
    void addEffect(EffectZoneData *data);

    // Should launch the good effect if the player is in and remove it
    // TODO
    void launchEffectIfPlayerIn(const BoxCollision& box) {}
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
    void addWaveData(SalvaData *data);

    // TODO
    void activateWave() {}
};

class WaveManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Wave>> _wave;

public:
    // Should remove the wave after
    void activateWave(const std::string& name) {
        _wave[name]->activateWave();
        _wave.erase(name);
    }

    void addWave(const std::string& name, Wave *wave) {
    }
};

// For the collisions use a BoxCollision

class Map {
private:
    EffectZones _zones; // All the effect zones each of them should
                        // have a type in custom properties

    WaveManager _waves; // Every waves can be detected
                        // You should load the layer if the EffectZones has it

    sf::Image _image; // The image layer to load

    std::vector<BoxCollision> _map_collisions; // All the collisions of the map
};
