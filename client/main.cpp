#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow w(sf::VideoMode(800, 600), "SFML works!");

    while (w.isOpen()) {
        sf::Event e;
        while (w.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                w.close();
        }
        w.clear();
        w.display();
    }
    return 0;
}
