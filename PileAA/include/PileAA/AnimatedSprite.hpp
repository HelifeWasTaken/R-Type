#pragma once

#include "Error.hpp"
#include "Timer.hpp"
#include "Types.hpp"
#include "meta.hpp"
#include <memory>
#include <unordered_map>

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
    bool _uses_default;

    void _setRect(const unsigned int& index);

public:
    HL_SUB_ERROR_IMPL(Error, AABaseError);

    AnimatedSprite(const std::string& textureName);

    ~AnimatedSprite() = default;

    /**
     * @brief Register an animation
     * @param animationName Animation name
     * @param animation Animation data
     */
    void registerAnimation(
        const std::string& animationName, const Animation& animation);

    /**
     * @brief Set the current animation
     * @param animationName Animation name
     * @return *this
     */
    AnimatedSprite& useAnimation(const std::string& animationName);

    /**
     * @brief Update the animation
     */
    void update();

    AnimatedSprite& setColor(const Color& color);
    AnimatedSprite& setPosition(float x, float y);
    AnimatedSprite& setPosition(const paa::Vector2f& position);
    AnimatedSprite& setScale(float x, float y);
    AnimatedSprite& setScale(const paa::Vector2f& scale);
    AnimatedSprite& setOrigin(float x, float y);
    AnimatedSprite& setOrigin(const paa::Vector2f& origin);
    AnimatedSprite& setRotation(float angle, bool is_radians = false);
    AnimatedSprite& move(float x, float y);
    AnimatedSprite& move(const paa::Vector2f& offset);

    /**
     * @brief Determines logically the frames on a well formed sprite sheet
     * @param frameSize Size of a frame
     * @param texture Texture of the sprite sheet
     * @param frames Number of frames
     * @param startPos Starting position of the first frame (Optional)
     * @param spacing Spacing between frames (Optional)
     * @return std::vector<sf::IntRect>
     */
    static std::vector<IntRect> determineRects(const Vector2u& frameSize,
        const Texture& texture, const unsigned int& frames,
        const Vector2u& startPos = Vector2u(0, 0),
        const Vector2u& spacing = Vector2u(0, 0));
};

using Sprite = std::shared_ptr<AnimatedSprite>;

}
