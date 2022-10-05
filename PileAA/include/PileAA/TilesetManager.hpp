#pragma once

#include <memory>
#include <vector>
#include <utility>

#include "Types.hpp"

namespace paa {

struct Tileset {
    Texture texture;
    int tileWidth;
    int tileHeight;
    int firstGid;
    int tileCount;
};

class TilesetManager : public Drawable, public Transformable {
private:
    std::vector<std::unique_ptr<Tileset>> _tilesets;
    std::vector<std::unique_ptr<std::pair<Texture&, VertexArray>>> _layers;

public:
    static inline const uint64_t FLIP_H_FLAG = 0x80000000;
    static inline const uint64_t FLIP_V_FLAG = 0x40000000;
    static inline const uint64_t FLIP_D_FLAG = 0x20000000;

public:
    /**
     * @brief Construct a TilesetManager from a json file using Tiled
     * @param filename The path to the json file
     */
    TilesetManager(const std::string& filename);

    /**
     * @brief Draws the map
     * @param target The target to draw to
     * @param states The render states
     */
    virtual void draw(RenderTarget& target, RenderStates states) const override;

    /**
     * @brief Draw one of the layers
     * @param window The window to draw to
     * @param layerIndex The nth Layer
     */
    void drawLayer(RenderWindow& window, size_t layerIndex) const;

    /**
     * @brief Get the number of layers in the map
     */
    size_t layerCount() const;

    /**
     * @brief Destroy the TilesetManager
     */
    ~TilesetManager() = default;
};

}
