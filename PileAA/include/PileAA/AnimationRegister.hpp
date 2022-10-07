#pragma once

#include "AnimatedSprite.hpp"
#include "Types.hpp"
#include "meta.hpp"
#include <string>

namespace paa {

class AnimationRegister {
public:
    struct Frames {
        float duration;
        std::vector<IntRect> frames;
    };

    AnimationRegister() = default;
    ~AnimationRegister() = default;

    void addAnimation(const std::string& spriteSheetName,
        const std::string& animationName, const Frames& frames);

    void setAnimationToSpriteIfExist(
        const std::string& spriteSheetName, AnimatedSprite& sprite);

private:
    std::unordered_map<std::string, std::unordered_map<std::string, Frames>>
        _animations;
};

HL_SINGLETON_IMPL(AnimationRegister, AnimationRegisterInstance);

}
