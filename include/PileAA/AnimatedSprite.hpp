#pragma once

#include <PileAA/Timer.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>

namespace paa {

    struct Animation {
        std::vector<sf::IntRect> rects;
        double speed;
    };

    using AnimationRegister = std::unordered_map<std::string, Animation>;

    class AnimatedSprite : public sf::Sprite {
    private:
        AnimationRegister _reg;
        Animation* _currentAnimation = nullptr;
        Timer _timer;
        unsigned int _animationIndex = 0;

        void _setRect(const unsigned int& index)
        {
            _animationIndex = index % _currentAnimation->rects.size();
            _timer.restart();
            this->setTextureRect(_currentAnimation->rects[_animationIndex]);
        }

    public:
        AnimatedSprite() = default;
        ~AnimatedSprite() = default;

        void registerAnimation(const std::string& animationName, const Animation& animation)
        {
            _reg[animationName] = animation;
            if (_currentAnimation == nullptr)
                _currentAnimation = &_reg[animationName];
        }

        void useAnimation(const std::string& animationName)
        {
            _currentAnimation = &_reg.at(animationName);
            _timer.setTarget(_currentAnimation->speed);
            _setRect(0);
        }

        void update()
        {
            if (_timer.isFinished()) {
                _setRect(_animationIndex + 1);
            }
        }

        static inline std::vector<sf::IntRect> determineRects(
            const sf::Vector2u &frameSize, const sf::Texture &texture,
            const unsigned int &frames, const sf::Vector2u &startPos = sf::Vector2u(0, 0),
            const sf::Vector2u &spacing = sf::Vector2u(0, 0))
        {
            const sf::Vector2u textureSize = texture.getSize();
            std::vector<sf::IntRect> rects;

            for (unsigned int i = 0, x = startPos.x, y = startPos.y; i < frames; i++) {
                rects.push_back(sf::IntRect(x, y, frameSize.x, frameSize.y));
                x += frameSize.x + spacing.x;
                if (x + frameSize.x > textureSize.x) {
                    x = startPos.x;
                    y += frameSize.y + spacing.y;
                }
            }
            return rects;
        }

    };
}
