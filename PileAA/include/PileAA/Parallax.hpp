#pragma once

#include "ResourceManager.hpp"

namespace paa
{
    class Parallax
    {
    private:
        paa::Vector2f _speed;
        paa::Vector2f _offset;

        std::vector<paa::DynamicEntity> _layers;

    public:
        Parallax(const paa::Vector2f& speed,
                const paa::Vector2f& offsets,
                const std::vector<std::string>& textures,
                const paa::Vector2f& scale=paa::Vector2f(1, 1))
            : _speed(speed)
            , _offset(offsets)
        {
            for (const auto& texture : textures) {
                _layers.emplace_back(PAA_NEW_ENTITY());
                _layers.back().attachSprite(texture)->setScale(scale);
                PAA_RESOURCE_MANAGER.get<paa::Texture>(texture).setRepeated(true);
            }
        }

        void update()
        {
            const float dt = PAA_DELTA_TIMER.getDeltaTime();
            paa::Vector2f current_offset(0, 0);

            for (auto& layer : _layers) {
                const paa::Vector2f speed(speed.x * dt + current_offset.x, speed.y * dt + current_offset.y);
                auto& s = layer.getComponent<paa::Sprite>();

                sf::IntRect r = s->getTextureRect();
                r.left += speed.x;
                r.top += speed.y;
                s->setTextureRect(r);

                current_offset.x += _offset.x * dt;
                current_offset.y += _offset.y * dt;
            }
        }

        ~Parallax()
        {
            for (auto& layer : _layers) {
                layer.kill();
            }
        }
    };
}
