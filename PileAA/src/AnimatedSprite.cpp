#include "PileAA/AnimatedSprite.hpp"
#include "PileAA/Math.hpp"
#include "PileAA/ResourceManager.hpp"
#include <iostream>
namespace paa {

void AnimatedSprite::_setRect(const unsigned int& index)
{
    _animationIndex = index % _currentAnimation->rects.size();
    _timer.restart();
    this->setTextureRect(_currentAnimation->rects[_animationIndex]);
}

AnimatedSprite::AnimatedSprite(const std::string& textureName)
{
    auto& tx = ResourceManagerInstance::get().get<sf::Texture>(textureName);
    const auto& def = ResourceManagerInstance::get().getDefaultTexture();

    this->setTexture(tx);

    const IntRect rect(
        0, 0, getTexture()->getSize().x, getTexture()->getSize().y);

    _uses_default = &tx == &def;
    registerAnimation("DEFAULT", paa::Animation { { rect }, 1000000.f });
    useAnimation("DEFAULT");
    setPosition(0, 0);
}

void AnimatedSprite::registerAnimation(
    const std::string& animationName, const Animation& animation)
{
    _reg[animationName] = animation;
}

AnimatedSprite& AnimatedSprite::useAnimation(const std::string& animationName,
                                                                    bool loop)
{
    try {
        if (_uses_default)
            _currentAnimation = &_reg.at("DEFAULT");
        else
            _currentAnimation = &_reg.at(animationName);
    } catch (...) {
        spdlog::warn("PileAA::AnimatedSprite: Could not find animtion: [{}] "
                     "using default",
            animationName);
        useAnimation("DEFAULT");
    }
    _timer.setTarget(_currentAnimation->speed);
    _setRect(0);
    _loop = loop;
    return *this;
}

bool AnimatedSprite::isLastFrame() const
{
    return _animationIndex == _currentAnimation->rects.size() - 1;
}

void AnimatedSprite::update()
{
    if (!_loop && isLastFrame()) {
        return;
    }
    if (_timer.isFinished()) {
        _setRect(_animationIndex + 1);
    }
}

AnimatedSprite& AnimatedSprite::setColor(const Color& color)
{
    BaseSprite::setColor(color);
    return *this;
}

AnimatedSprite& AnimatedSprite::setPosition(float x, float y)
{
    BaseSprite::setPosition(x, y);
    return *this;
}

AnimatedSprite& AnimatedSprite::setPosition(const Vector2f& position)
{
    BaseSprite::setPosition(position);
    return *this;
}

AnimatedSprite& AnimatedSprite::setScale(float x, float y)
{
    BaseSprite::setScale(x, y);
    return *this;
}

AnimatedSprite& AnimatedSprite::setScale(const Vector2f& scale)
{
    BaseSprite::setScale(scale);
    return *this;
}

AnimatedSprite& AnimatedSprite::setOrigin(float x, float y)
{
    BaseSprite::setOrigin(x, y);
    return *this;
}

AnimatedSprite& AnimatedSprite::setOrigin(const Vector2f& origin)
{
    BaseSprite::setOrigin(origin);
    return *this;
}

AnimatedSprite& AnimatedSprite::setRotation(float angle, bool is_radians)
{
    if (is_radians)
        angle = paa::Math::toDegrees(angle);
    BaseSprite::setRotation(angle);
    return *this;
}

AnimatedSprite& AnimatedSprite::move(float x, float y)
{
    BaseSprite::move(x, y);
    return *this;
}

AnimatedSprite& AnimatedSprite::move(const Vector2f& offset)
{
    BaseSprite::move(offset);
    return *this;
}

std::vector<IntRect> AnimatedSprite::determineRects(const Vector2u& frameSize,
    const Texture& texture, const unsigned int& frames,
    const Vector2u& startPos, const Vector2u& spacing)
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
