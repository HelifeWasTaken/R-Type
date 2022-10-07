#pragma once

#include "BaseComponents.hpp"
#include "Types.hpp"
#include "meta.hpp"

namespace paa {

/**
 * @brief The batch renderer is used to render all the Drawables in the game.
 */
class BatchRenderer {
private:
    std::vector<std::pair<Depth, std::vector<Drawable*>>> _drawables;

public:
    /**
     * @brief Construct a new Batch Renderer object
     */
    BatchRenderer() = default;

    /**
     * @brief Destroy the Batch Renderer object
     */
    ~BatchRenderer() = default;

    /**
     * @brief Add a drawable to the batch renderer.
     * @param drawable The drawable to add.
     * @param depth The depth of the drawable.
     */
    void add(Drawable* drawable, Depth depth);

    /**
     * @brief Draw all the drawables in the batch renderer.
     * @param window The window to draw to.
     */
    void render(RenderWindow& window);
};

/**
 * @brief Construct a new hl singleton impl object
 */
HL_SINGLETON_IMPL(BatchRenderer, BatchRendererInstance);

}