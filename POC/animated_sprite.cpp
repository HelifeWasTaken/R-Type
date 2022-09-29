#include "poc.hpp"
#include <PileAA/Timer.hpp>
#include <PileAA/AnimatedSprite.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>

void poc_animated_sprite(void)
{
    sf::Texture tex;
    tex.loadFromFile("../assets/r-typesheet5.png");
    sf::RenderWindow window(sf::VideoMode(800, 800), "lol");

    paa::AnimatedSprite sprite;

    sprite.setTexture(tex);
    sprite.registerAnimation("speedy",
        { paa::AnimatedSprite::determineRects(sf::Vector2u(33, 36), tex, 16), 100 });
    sprite.useAnimation("speedy");
    sprite.setPosition(sf::Vector2f(400, 400));
    sprite.setScale(8.0, 8.0);

    while (window.isOpen()) {
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        sprite.update();
        window.draw(sprite);
        window.display();
    }
}
