# R-TYPE API Documentation

Welcome to RTYPE

This a section to see all that you can do with the API:

## The ECS

### The registry

The registry is a central place to store all the information about the
components and the connections between them.

It is named `silva::registry`.

#### Create your first Entity:

```cpp
hl::silva::registry registry;
hl::silva::Entity entity = registry.spawn_entity();
```

The use of an entity is to store any types of components.

#### What is a component?

A component is a piece of data that is attached to an entity.

Here is how to attach a component:

```cpp
hl::silva::registry registry;
registry.register_component<int>();
hl::silva::Entity entity = registry.spawn_entity();
registry.emplace<int>(entity, 42);
```

And here is how to get it

```cpp
int value = registry.get<int>(entity);
```

But what if I want to modify the value ?

```cpp
// Note that the reference to the variable is always returned
int& value = registry.get<int>(entity);
value = 6; // Modified the value in the registry
```

### The coroutines (a.k.a Systems)

A system is a function that is called every 1 / 60 seconds.

And applicates it on every entity that matches his requirements.

```cpp
struct Gravity {
    float x, y, z;
};

struct RigidBody {
    float mass;
    float velocity;
    float acceleration;
};

struct Position {
    int x, y, z;
};

```cpp
int main()
{
    silva::registry registry;

    registry.register_component<Position, RigidBody, Gravity>();

    for (int i = 0; i < 100; i++) {
        registry.emplace<Position>(registry.spawn_entity(),rand(), rand(), rand())
            .emplace_r<RigidBody>(5, 0, 0) // Emplace r uses the last used Entity
            .emplace_r<Gravity>(0, -9.8, 0);
    }

    // Only take the entities that have a RigidBody a Position and a Gravity component
    registry.add_system([](hl::silva::registry& registry)
        {
            // Perfect forwarding of a tuple of components
            for (auto&& [entity, position, rigidBody, gravity] : registry.view<Position, RigidBody, Gravity>()) {
                // This is not an accurate Earth system force application
                // It is just here for the example
                position.y += rigidBody.velocity;
                rigidBody.velocity += rigidBody.acceleration;
                position.x += gravity.x;
                position.y += gravity.y;
                position.z += gravity.z;
            }
        }
    );

    registry.update(); // Call the update of every registered Systems
    return 0;
}
```

### The StateMachine

A state machine is a class that is used to manage the state of the game.

It is generally used to avoid big conditional branchings

#### Your first state

```cpp
class MyState : public State {
public:
    MyState()
    {
        std::cout << "Hello World!" << std::endl;
    }

    ~MyState()
    {
        std::cout << "Goodbye World!" << std::endl;
    }

    void update() override {
        std::cout << "Update" << std::endl;
    }
};

int main()
{
    StateMachine stateMachine;
    stateMachine.changeState<MyState>();
    stateMachine.update();
    stateMachine.update();
}
```

Expected output:
```
Init
Update
Update
Goodbye World
```

### Your first stacked state!

When to use a stacked state?

Imagine your are in game and you want to pause it.

You don't want to destroy the current state.

So you create a new state and push it to the stack.

Here is the example:

```cpp
class GameState : public State {

    void update() override {
        std::cout << "Playing" << std::endl;

        if (KeyboardPressed(KEY_ESCAPE)) {
            stateMachine.pushState<PauseState>();
        }

        player.update();
    }

};

class PauseState : public State {

    void update() override {
        std::cout << "Paused" << std::endl;

        if (KeyboardPressed(KEY_ESCAPE)) {
            stateMachine.popState();
        }

        buttons.update();
    }

};
```

This example should make use alternatively the GameState and PauseState classes.

### Behaviours with StateMachine:

```cpp
class RunBehaviour : public State {
private:
    Player& player;

public:
    RunBehaviour(Player& animable) : player(player)
    {
    }

    void update() override {
        // Specific update code

        if (player.notMoving()) {
            player.changeState<IdleBehaviour>(player);
            return;
        }
    }
};

class IdleBehaviour : public State {
private:
    Player& player;

public:
    IdleBehaviour(Player& player) : player(player)
    {
    }

    void update() override {
        // Specific update code
        if (player.isMoving()) {
            player.changeState<RunBehaviour>(player);
            return;
        }
    }
};

class ActionBehaviour : public State {
private:
    Player& player;

public:
    ActionBehaviour(Player& player) : player(player)
    {
    }

    void update() override {
        // Specific update code

        if (player.finishedAction()) {
            player.changeState<IdleBehaviour>(player);
            return;
        }
    }
};

class Player {
private:
    Animable animable;
    StateMachine stateMachine;
    Vector2 velocity;

public:
    Player() { stateMachine.changeState<IdleBehaviour>(player); }

    void update() { stateMachine.update(); }

    bool isMoving() const { return velocity.x != 0 || velocity.y != 0; }

    bool notMoving() const { return !isMoving(); }

    template<typename T>
    void changeState(Player& player) { stateMachine.changeState<T>(player); }
};
```

## The GameEngine (PileAA)

### Automatically animated sprites

With PileAA Engine you can of course animate sprite through sprite sheet nearly automatically

```cpp
#include <PileAA/Timer.hpp>
#include <PileAA/AnimatedSprite.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>

void animate_a_sprite(void)
{
    // The image
    sf::Texture tex;
    tex.loadFromFile("filepath_to_sprite_sheet");

    // The window
    sf::RenderWindow window(sf::VideoMode(800, 800), "This works!");

    // The sprite inherits from sf::Sprite
    paa::AnimatedSprite sprite;

    sprite.setTexture(tex);
    // Set a name to animation and it's rects
    sprite.registerAnimation("speedy",
        // determineRects will determine the rects on a well formed spritesheet given the parameters
        // See API documentation about this function in PileAA/AniamtedSprite.hpp
        { paa::AnimatedSprite::determineRects(sf::Vector2u(33, 36), tex, 16), 100 });
    // Use the given animation
    sprite.useAnimation("speedy");
    // Set the sprite position on the screen
    sprite.setPosition(sf::Vector2f(400, 400));
    // Sets the scale of the sprite to take nearly all the window
    sprite.setScale(8.0, 8.0);

    // basic SFML loop
    while (window.isOpen()) {
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        // update the sprite
        sprite.update();
        // can be draw like a normal sf::Sprite
        window.draw(sprite);
        window.display();
    }
}
```

### The Joystick and Keyboard

```cpp
#include "PileAA/InputHandler.hpp"
#include <iostream>

void joystick_example(void)
{
    // Set the id of the joystick on creation
    // The joystick is already fully mapped on creation
    paa::Joystick joystick(0);

    while (true) {
        // This will print all the current information about the joystick
        // All members function are documented in PileAA/InputHandler.hpp
        std::cout << joystick << std::endl;
    }
}
```

```cpp
#include "PileAA/InputHandler.hpp"
#include <iostream>

void keyboard_example(void)
{
    // Set the id of the joystick on creation
    paa::ControllerKeyboard joystick;

    // The keyboard emulates a joystick
    // and requires mapping
    joystick.setKey(0, sf::Keyboard::Key::A)
        .setKey(1, sf::Keyboard::Key::B)
        .setKey(2, sf::Keyboard::Key::C)
        .setKey(3, sf::Keyboard::Key::D)
        .setKey(4, sf::Keyboard::Key::E);

    while (true) {
        // This will print all the current information about the joystick
        // All members function are documented in PileAA/InputHandler.hpp
        std::cout << joystick << std::endl;
    }
}
```

Note that you can use the `paa::IController` to abstract controller and keyboard the only difference between both of them
is that the joystick must have an ID and the Keyboard must be mapped before both them are transformed to a `paa::IController*`.

### The tileset manager

If you have a compliant Tiled Map exported as json you can draw it like this:

```cpp
void tileset_manager_example(void)
{
    paa::TilesetManager tileset_manager("map.json");

    // The window
    sf::RenderWindow window(sf::VideoMode(800, 800), "This works!");

    // basic SFML loop
    while (window.isOpen()) {
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.draw(tileset_manager); // Note that it will draws all the layers directly
        // If you need to draw ONE specific layer you can do
        tileset_manager.draw(3, window); // This will draw on the window the 4th layer (as indexes start at 0)
        window.display();
    }

}
```

### The resource manager

To manage resources PileAA provides a `ResourceManager`

```cpp
void resource_manager_example(void)
{
    paa::ResourceManager rmanager;

    rmanager.load<sf::Texture>("file.png");

    sf::Texture& tx = rmanager.get<sf::Texture>("file.png");

    // You can give aliases to your resources
    rmanager.load<sf::Sound>("sound.ogg", "dinosaur_sound");
    sf::Sound& s = rmanager.get<sf::Sound>("dinosaur_sound"); // Note here that sound.ogg will not work
}
```

### Simple Timer

If you need a timer here is an example

```cpp
void timer_example(void)
{
    // Inherits from sf::Clock
    paa::Timer timer;

    timer.setTarget(1000); // Target is 1000ms a.k.a 1sec
    while (true) {
        while (!timer.isFinished());
        std::cout << "1 second passed" << std::endl;
        timer.restart();
    }
}
```

## The Server API:

TODO
