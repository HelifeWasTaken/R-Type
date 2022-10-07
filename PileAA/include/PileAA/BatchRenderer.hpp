#pragma once

#include "BaseComponents.hpp"
#include "Types.hpp"
#include "meta.hpp"

namespace paa {

class BatchRenderer {
private:
    std::vector<std::pair<Depth, std::vector<Drawable*>>> _drawables;

public:
    BatchRenderer() = default;

    ~BatchRenderer() = default;

    void add(Drawable* drawable, Depth depth);

    void render(RenderWindow& window);
};

HL_SINGLETON_IMPL(BatchRenderer, BatchRendererInstance);

}