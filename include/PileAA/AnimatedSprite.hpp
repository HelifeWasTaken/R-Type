#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>
#include "ResourceManager.hpp"
#include "Timer.hpp"

namespace paa {

    /**
     * @brief Animation base presentation
     */
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
        AnimatedSprite(const std::string& textureName)
        {
            this->setTexture(ResourceManagerInstance::get().get<sf::Texture>(textureName));
        }

        ~AnimatedSprite() = default;

        /**
         * @brief Register an animation
         * @param animationName Animation name
         * @param animation Animation data
         */
        void registerAnimation(const std::string& animationName, const Animation& animation)
        {
            _reg[animationName] = animation;
            if (_currentAnimation == nullptr)
                _currentAnimation = &_reg[animationName];
        }

        /**
         * @brief Set the current animation
         * @param animationName Animation name
         */
        void useAnimation(const std::string& animationName)
        {
            _currentAnimation = &_reg.at(animationName);
            _timer.setTarget(_currentAnimation->speed);
            _setRect(0);
        }

        /**
         * @brief Update the animation
         */
        void update()
        {
            if (_timer.isFinished()) {
                _setRect(_animationIndex + 1);
            }
        }

        /**
         * @brief Determines logically the frames on a well formed sprite sheet
         * @param frameSize Size of a frame
         * @param texture Texture of the sprite sheet
         * @param frames Number of frames
         * @param startPos Starting position of the first frame (Optional)
         * @param spacing Spacing between frames (Optional)
         * @return std::vector<sf::IntRect>
         */
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