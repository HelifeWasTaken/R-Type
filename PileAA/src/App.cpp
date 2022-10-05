#include "PileAA/App.hpp"
#include "PileAA/BaseComponents.hpp"

namespace paa {

App& App::get()
{
    if (_instance == nullptr)
        _instance = new App();
    return *_instance;
}

void App::release()
{
    if (_instance != nullptr) {
        delete _instance;
        _instance = nullptr;
    }
}

bool App::isRunning() const
{
    return Screen::get().isOpen() &&
            SceneManager::get().canBeUpdated();
}

void App::update()
{
    auto& window = Screen::get();
    auto& input = InputHandler::get();
    auto& ecs = EcsInstance::get();
    auto& scene = SceneManager::get();

    // TODO Update handleEvent

    Event event;
    // input.update();
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        // input.handleEvent(event);
        scene.handleEvent();
    }
    window.clear();
    ecs.update();
    scene.update();
    window.display();
}

void App::run()
{
    static bool already_running = false;
    if (already_running)
        return;
    already_running = true;
    while (isRunning()) {
        update();
    }
    already_running = false;
}

void App::stop()
{
    Screen::get().close();
    SceneManager::get().stop();
}

void setup_paa_system()
{
    InputHandler::get();
    ResourceManagerInstance::get();

    setup_ecs(EcsInstance::get());

    SceneManager::get();
    Screen::get().create(VideoMode(800, 600), "PileAA");
    App::get();
}

void stop_paa_system()
{
    App::release();
    Screen::release();
    SceneManager::release();
    ResourceManagerInstance::release();
    InputHandler::release();
}

}