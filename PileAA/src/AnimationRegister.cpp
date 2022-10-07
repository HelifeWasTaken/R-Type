#include "PileAA/AnimationRegister.hpp"

namespace paa {

void AnimationRegister::addAnimation(const std::string& spriteSheetName,
    const std::string& animationName, const Frames& frames)
{
    _animations[spriteSheetName][animationName] = frames;
}

void AnimationRegister::setAnimationToSpriteIfExist(
    const std::string& spriteSheetName, AnimatedSprite& sprite)
{
    if (_animations.find(spriteSheetName) == _animations.end()) {
        return;
    }

    for (const auto& animation : _animations[spriteSheetName]) {
        sprite.registerAnimation(animation.first,
            { animation.second.frames, animation.second.duration });
    }
}

}
