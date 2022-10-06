#include "PileAA/App.hpp"

#include "PileAA/external/nlohmann/json.hpp"

#include <fstream>

#include <spdlog/spdlog.h>

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

bool App::run()
{
    auto& window = paa::Screen::get();
    auto& input = paa::InputHandler::get();
    auto& ecs = paa::EcsInstance::get();
    auto& scene = paa::SceneManager::get();
    auto& batch = paa::BatchRendererInstance::get();

    spdlog::info("PileAA: Starting main loop");

    while (isRunning()) {
        Event event;
        // input.update();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Key::R &&
                event.key.control) {
                return true;
            }
            // input.handleEvent(event);
            scene.handleEvent();
        }
        window.clear();
        ecs.update();
        scene.update();
        batch.render(window);
        window.display();
    }
    return false;
}

void App::stop()
{
    spdlog::info("PileAA: Stopping app");

    paa::Screen::get().close();
    spdlog::info("PileAA: Window closed");

    paa::SceneManager::get().stop();
    spdlog::info("PileAA: Scene manager stopped");
}

// CONFIGURATION FILE LOADER 

static inline void load_configuration_file_resources(nlohmann::json& json)
{
    ResourceManager& resourceManager = ResourceManagerInstance::get();

    spdlog::info("PileAA: Loading resources from configuration file");

    for (const auto& it : json) {
        try {
            const auto& type = it["type"].get<std::string>();
            const auto& path = it["path"].get<std::string>();
            const auto& name = it["name"].get<std::string>();

            spdlog::info("PileAA: Loading resource: {} {} {}", type, path, name);

            if (type == "texture") resourceManager.load<Texture>(path, name);
            else if (type == "font") resourceManager.load<Font>(path, name);
            else if (type == "sound") resourceManager.load<SoundBuffer>(path, name);
            else if (type == "image") resourceManager.load<Image>(path, name);
            else
                throw App::Error(std::string("resources: load_configuration_file - Invalid type: ") + type);
        } catch (const nlohmann::json::exception& e) {
            throw App::Error(std::string("resources: load_configuration_file - Invalid json file: ") + e.what());
        }
    }
    spdlog::info("PileAA: Resources loaded");
}

static inline void load_configuration_file_animations(nlohmann::json& json)
{
    spdlog::info("PileAA: Loading animations from configuration file");

    AnimationRegister& animationRegister = AnimationRegisterInstance::get();
    ResourceManager& resourceManager = ResourceManagerInstance::get();

    for (const auto& animation : json) {
        try {
            const auto& spriteSheetName = animation["texture"].get<std::string>();
            spdlog::info("PileAA: Loading animation: {}", spriteSheetName);
            try {
                resourceManager.get<Texture>(spriteSheetName); // Check if the texture exists
                spdlog::info("PileAA: Texture {} exists", spriteSheetName);
                for (const auto& frames : animation["frames"]) {
                    const auto& animationName = frames["name"].get<std::string>();
                    const auto& rects = frames["rects"];
                    AnimationRegister::Frames currentFrames;
                    currentFrames.duration = frames["speed"].get<float>();
                    for (const auto& rect : rects) {
                        currentFrames.frames.push_back(IntRect(
                            rect[0].get<int>(), rect[1].get<int>(),
                            rect[2].get<int>(), rect[3].get<int>())
                        );
                    }
                    spdlog::info("PileAA: Loading animation: {} {}", spriteSheetName, animationName);
                    animationRegister.addAnimation(spriteSheetName, animationName, currentFrames);
                }
            } catch (const nlohmann::json::exception& e) {
                throw App::Error(std::string("animations: load_configuration_file - Invalid json file: ") + e.what() + " in " + spriteSheetName + " spritesheet");
            }
        } catch (const nlohmann::json::exception& e) {
            throw App::Error(std::string("animations: load_configuration_file - Invalid json file: ") + e.what());
        }
    }
    spdlog::info("PileAA: Animations loaded");
}

static inline void load_configuration_file(const std::string& filename)
{
    spdlog::info("PileAA: Loading configuration file: {}", filename);

    std::ifstream file(filename);
    if (!file.is_open())
        throw ResourceManagerError("load_configuration_file - Failed to open " + filename);
    try {
        nlohmann::json json;
        file >> json;
        load_configuration_file_resources(json["resources"]);
        load_configuration_file_animations(json["animations"]);
    } catch (const nlohmann::json::exception& e) {
        throw ResourceManagerError("load_configuration_file - Invalid json file: " + std::string(e.what()));
    }
}

void setup_paa_system(const std::string& configuration_filename)
{
    spdlog::info("PileAA: Setting up system");

    InputHandler::get();
    spdlog::info("PileAA: InputHandler created");

    ResourceManagerInstance::get();
    spdlog::info("PileAA: ResourceManager created");

    AnimationRegisterInstance::get();
    spdlog::info("PileAA: AnimationRegister created");

    setup_ecs(EcsInstance::get());
    spdlog::info("PileAA: ECS created");

    SceneManager::get();
    spdlog::info("PileAA: SceneManager created");

    Screen::get().create(VideoMode(800, 600), "PileAA");
    spdlog::info("PileAA: Screen created");

    BatchRendererInstance::get();
    spdlog::info("PileAA: BatchRenderer created");

    App::get();
    spdlog::info("PileAA: App created");

    load_configuration_file(configuration_filename);

    spdlog::info("PileAA: system setup complete");
}

void stop_paa_system()
{
    spdlog::info("PileAA: Stopping PileAA system");

    InputHandler::release();
    spdlog::info("PileAA: InputHandler released");

    ResourceManagerInstance::release();
    spdlog::info("PileAA: ResourceManager released");

    AnimationRegisterInstance::get();
    spdlog::info("PileAA: AnimationRegister released");

    EcsInstance::release();
    spdlog::info("PileAA: ECS released");

    SceneManager::release();
    spdlog::info("PileAA: SceneManager released");

    Screen::release();
    spdlog::info("PileAA: Screen released");

    BatchRendererInstance::release();
    spdlog::info("PileAA: BatchRenderer released");

    App::release();
    spdlog::info("PileAA: App released");
}

}