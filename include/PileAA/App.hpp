#pragma once

#include <SilvaState>
#include "InputHandler.hpp"

namespace paa {

    class App {
    private:
        App *_instance = nullptr;
        hl::silva::StateManager scene;
        sf::RenderWindow window;
        sf::Event event;

        App(unsigned int width=800, unsigned int height=600, const std::string& title="PileAA")
            : window(sf::VideoMode(width, height), title)
        {
            paa::InputHandler::get();
        }

    public:
        ~App()
        {
            paa::InputHandler::release();
        }

        static App& get()
        {
            if (_instance == nullptr)
                _instance = new App();
            return *_instance;
        }

        static void release()
        {
            if (_instance != nullptr) {
                delete _instance;
                _instance = nullptr;
            }
        }

        hl::silva::StateManager &scene()
        {
            return scene;
        }

        bool isRunning()
        {
            return window.isOpen() && scene.canBeUpdated();
        }

        void update()
        {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                scene.update();
            }
            scene.update();
        }

        void render()
        {
            window.clear();
            scene.draw();
            window.display();
        }

    };

    using Application = App&;

    class GameState : public hl::silva::State {
    protected:
        paa::App& app = paa::App::get();
        paa::InputHandler &input = paa::InputHandler::get();
        paa::ResourceManager &res = paa::ResourceManagerInstance::get();
        paa::Window &window = app.window();

    public:
        GameState() = default;
        ~GameState() = default;
    };

}

#define NEW_PAA_STATE(name) struct name : public GameState
#define DEFAULT_PAA_STATE_CONFIGURATION(name) \
    name() = default; \
    ~name() = default \
#define NEW_PAA_UPDATE void update() override
#define NEW_PAA_DRAW void draw() override