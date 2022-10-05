#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>
#include "ResourceManager.hpp"
#include "Timer.hpp"
#include "Types.hpp"

namespace paa {

    /**
     * @brief Animation base presentation
     */
    struct Animation {
        std::vector<IntRect> rects;
        double speed;
    };

    using AnimationRegister = std::unordered_map<std::string, Animation>;

    class AnimatedSprite : public BaseSprite {
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

            const IntRect rect(0, 0,
                getTexture()->getSize().x, 
                getTexture()->getSize().y);

            registerAnimation("DEFAULT",
                paa::Animation{ { rect }, 1000000.f }
            );
            useAnimation("DEFAULT");
            setPosition(0, 0);
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
        static inline std::vector<IntRect> determineRects(
            const Vector2u &frameSize,
            const Texture &texture,
            const unsigned int &frames,
            const Vector2u &startPos = Vector2u(0, 0),
            const Vector2u &spacing = Vector2u(0, 0))
        {
            const Vector2u textureSize = texture.getSize();
            std::vector<IntRect> rects;

            for (unsigned int i = 0, x = startPos.x, y = startPos.y; i < frames; i++) {
                rects.push_back(IntRect(x, y, frameSize.x, frameSize.y));
                x += frameSize.x + spacing.x;
                if (x + frameSize.x > textureSize.x) {
                    x = startPos.x;
                    y += frameSize.y + spacing.y;
                }
            }
            return rects;
        }

    };

    using Sprite = AnimatedSprite;
}