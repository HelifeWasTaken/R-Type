#pragma once

#include "ResourceManager.hpp"
#include "DynamicEntity.hpp"

namespace paa
{
    class Parallax
    {
    private:
        paa::Vector2f _speed;
        paa::Vector2f _offset;

        std::vector<paa::DynamicEntity> _layers;

        bool _noDestroy;

    public:

        Parallax(const paa::Vector2f& speed,
                const paa::Vector2f& offsets,
                const std::vector<std::string>& textures,
                const paa::Vector2f& scale=paa::Vector2f(1, 1),
                bool noDestroy = false)
            : _speed(speed)
            , _offset(offsets)
            , _noDestroy(noDestroy)
        {
            for (const auto& texture : textures) {
                _layers.emplace_back(PAA_NEW_ENTITY()).attachSprite(texture)->setScale(scale);
                PAA_RESOURCE_MANAGER.get<paa::Texture>(texture).setRepeated(true);
            }
        }

        void update()
        {
            const float dt = PAA_DELTA_TIMER.getDeltaTime();
            paa::Vector2f current_offset(0, 0);

            for (auto& layer : _layers) {
                const paa::Vector2f speed(_speed.x * dt + current_offset.x, _speed.y * dt + current_offset.y);
                auto& s = layer.getComponent<paa::Sprite>();

                sf::IntRect r = s->getTextureRect();
                r.left += speed.x;
                if (layer.getId() != _layers[0].getId())
                    r.top += speed.y;
                s->setTextureRect(r);

                current_offset.x += _offset.x * dt;
                if (layer.getId() != _layers[0].getId())
                    current_offset.y += _offset.y * dt;
            }
        }

        void destroy() {
            for (auto& layer : _layers) {
                if (layer.isAlive())
                    layer.kill();
            }
        }

        ~Parallax()
        {
            if (_noDestroy)
                return;
            destroy();
        }
    };
}
