#pragma once

#include <unordered_map>
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


    class AnimatedSprite : public BaseSprite {
    public:
        using AnimationRegister = std::unordered_map<std::string, Animation>;

    private:
        AnimationRegister _reg;
        Animation* _currentAnimation = nullptr;
        Timer _timer;
        unsigned int _animationIndex = 0;

        void _setRect(const unsigned int& index);

    public:
        AnimatedSprite(const std::string& textureName);

        ~AnimatedSprite() = default;

        /**
         * @brief Register an animation
         * @param animationName Animation name
         * @param animation Animation data
         */
        void registerAnimation(const std::string& animationName, const Animation& animation);

        /**
         * @brief Set the current animation
         * @param animationName Animation name
         */
        void useAnimation(const std::string& animationName);

        /**
         * @brief Update the animation
         */
        void update();

        /**
         * @brief Determines logically the frames on a well formed sprite sheet
         * @param frameSize Size of a frame
         * @param texture Texture of the sprite sheet
         * @param frames Number of frames
         * @param startPos Starting position of the first frame (Optional)
         * @param spacing Spacing between frames (Optional)
         * @return std::vector<sf::IntRect>
         */
        static std::vector<IntRect> determineRects(
            const Vector2u &frameSize,
            const Texture &texture,
            const unsigned int &frames,
            const Vector2u &startPos = Vector2u(0, 0),
            const Vector2u &spacing = Vector2u(0, 0));
    };

    using Sprite = AnimatedSprite;
}