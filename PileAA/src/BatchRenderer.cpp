#include "PileAA/BaseComponents.hpp"
#include "PileAA/BatchRenderer.hpp"

namespace paa {

    void BatchRenderer::add(Drawable *drawable, Depth depth)
    {
        auto it = std::find_if(_drawables.begin(), _drawables.end(),
                            [&depth](const auto &pair) { return pair.first.z == depth.z; });

        if (it == _drawables.end()) {
            _drawables.emplace_back(depth, std::vector<Drawable *>{drawable});
            std::sort(_drawables.begin(), _drawables.end(),
                    [](auto &pair1, auto &pair2) {
                        return pair1.first.z < pair2.first.z;
                    });
        } else {
            it->second.push_back(drawable);
        }
    }

    void BatchRenderer::render(RenderWindow& window)
    {
        for (auto&& [_, drawables] : _drawables) {
            for (auto&& drawable : drawables) {
                window.draw(*drawable);
            }
        }
        _drawables.clear();
    }

}