#include "PileAA/App.hpp"
#include "PileAA/BaseComponents.hpp"
#include "PileAA/BatchRenderer.hpp"

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
    auto& window = paa::Screen::get();
    auto& input = paa::InputHandler::get();
    auto& ecs = paa::EcsInstance::get();
    auto& scene = paa::SceneManager::get();
    auto& batch = paa::BatchRendererInstance::get();

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
    batch.render(window);
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
    paa::Screen::get().close();
    paa::SceneManager::get().stop();
}

void setup_paa_system()
{
    InputHandler::get();
    ResourceManagerInstance::get();
    setup_ecs(EcsInstance::get());
    SceneManager::get();
    Screen::get().create(VideoMode(800, 600), "PileAA");
    BatchRendererInstance::get();
    App::get();
}

void stop_paa_system()
{
    InputHandler::release();
    ResourceManagerInstance::release();
    EcsInstance::release();
    SceneManager::release();
    Screen::release();
    BatchRendererInstance::release();
    App::release();
}

}