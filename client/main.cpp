#include <SFML/Graphics.hpp>
int main()
{
    sf::RenderWindow w(sf::VideoMode(800, 600), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    while (w.isOpen())
    {
        sf::Event e;
        while (w.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                w.close();
        }
        w.clear();
        w.draw(shape);
        w.display();
    }
    return 0;
}
