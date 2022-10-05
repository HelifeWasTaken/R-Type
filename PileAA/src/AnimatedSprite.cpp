#include "PileAA/AnimatedSprite.hpp"
#include "PileAA/ResourceManager.hpp"

namespace paa {

void AnimatedSprite::_setRect(const unsigned int& index)
{
    _animationIndex = index % _currentAnimation->rects.size();
    _timer.restart();
    this->setTextureRect(_currentAnimation->rects[_animationIndex]);
}

AnimatedSprite::AnimatedSprite(const std::string& textureName)
{
    this->setTexture(
        ResourceManagerInstance::get().get<sf::Texture>(textureName));

    const IntRect rect(
        0, 0, getTexture()->getSize().x, getTexture()->getSize().y);

    registerAnimation("DEFAULT", paa::Animation { { rect }, 1000000.f });
    useAnimation("DEFAULT");
    setPosition(0, 0);
}

void AnimatedSprite::registerAnimation(
    const std::string& animationName, const Animation& animation)
{
    _reg[animationName] = animation;
}

void AnimatedSprite::useAnimation(const std::string& animationName)
{
    _currentAnimation = &_reg.at(animationName);
    _timer.setTarget(_currentAnimation->speed);
    _setRect(0);
}

void AnimatedSprite::update()
{
    if (_timer.isFinished()) {
        _setRect(_animationIndex + 1);
    }
}

std::vector<IntRect> AnimatedSprite::determineRects(
    const Vector2u& frameSize, const Texture& texture,
    const unsigned int& frames, const Vector2u& startPos,
    const Vector2u& spacing)
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

}