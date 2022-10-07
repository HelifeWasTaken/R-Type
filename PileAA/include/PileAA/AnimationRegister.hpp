#pragma once

#include "AnimatedSprite.hpp"
#include "Types.hpp"
#include "meta.hpp"
#include <string>

namespace paa {

class AnimationRegister {
public:
    /**
     * @brief The frames information about the animation.
     */
    struct Frames {
        float duration;
        std::vector<IntRect> frames;
    };

    AnimationRegister() = default;
    ~AnimationRegister() = default;

    /**
     * @brief Register an animation.
     * @param name The name of the animation.
     * @param frames The frames information about the animation.
     */
    void addAnimation(const std::string& spriteSheetName,
        const std::string& animationName, const Frames& frames);

    /**
     * @brief Set the Animation To Sprite If Exist object
     *
     * @param spriteSheetName
     * @param sprite
     */
    void setAnimationToSpriteIfExist(
        const std::string& spriteSheetName, AnimatedSprite& sprite);

private:
    std::unordered_map<std::string, std::unordered_map<std::string, Frames>>
        _animations;
};

/**
 * @brief Construct a new hl singleton impl object
 *
 */
HL_SINGLETON_IMPL(AnimationRegister, AnimationRegisterInstance);

}
