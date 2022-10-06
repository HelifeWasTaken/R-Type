#pragma once

#include "Types.hpp"
#include "meta.hpp"
#include <string>
#include "AnimatedSprite.hpp"

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
                    const std::string& animationName,
                    const Frames& frames)
    {
        _animations[spriteSheetName][animationName] = frames;
    }

    void setAnimationToSpriteIfExist(const std::string& spriteSheetName,
        AnimatedSprite& sprite)
    {
        if (_animations.find(spriteSheetName) == _animations.end()) {
            return;
        }

        for (const auto& animation : _animations[spriteSheetName]) {
            sprite.registerAnimation(animation.first, {
                animation.second.frames,
                animation.second.duration
            });
        }
    }

private:
    std::unordered_map<std::string, std::unordered_map<std::string, Frames>> _animations;
};

HL_SINGLETON_IMPL(AnimationRegister, AnimationRegisterInstance);

}