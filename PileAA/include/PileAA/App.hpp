#pragma once

#include "external/HelifeWasTaken/Silva"
#include "external/HelifeWasTaken/SilvaState"

#include "AnimatedSprite.hpp"
#include "AnimationRegister.hpp"
#include "BaseComponents.hpp"
#include "InputHandler.hpp"
#include "BatchRenderer.hpp"
#include "ResourceManager.hpp"
#include "Timer.hpp"

#include "paa_commands/paa_command_ecs.hpp"
#include "paa_commands/paa_command_main.hpp"
#include "paa_commands/paa_command_state.hpp"
#include "paa_commands/paa_getters.hpp"
#include "paa_commands/paa_utilities.hpp"

namespace paa {

/**
 * @brief Construct a new hl singleton impl object
 *
 */
HL_SINGLETON_IMPL(hl::silva::registry, EcsInstance);

/**
 * @brief Construct a new hl singleton impl object
 *
 */
HL_SINGLETON_IMPL(hl::silva::StateManager, SceneManager);

/**
 * @brief Construct a new hl singleton impl object
 *
 */
HL_SINGLETON_IMPL(RenderWindow, Screen);

/**
 * @brief Construct a new hl singleton impl object
 * 
 */
HL_SINGLETON_IMPL(DeltaTimer, DeltaTimerInstance);

class App {
private:
    static inline App* _instance = nullptr;
    App() = default;

public:
    ~App() = default;

    /**
     * @brief Construct a new App object if not created yet and returns it
     *
     * @return App&
     */
    static App& get();

    /**
     * @brief Frees the App instance if it exists
     *
     * @return App*
     */
    static void release();

    HL_SUB_ERROR_IMPL(Error, AABaseError);

public:
    /**
     * @brief Tells you if the app is running
     *
     * @return true
     * @return false
     */
    bool isRunning() const;

    /**
     * @brief Starts the app
     */
    bool run();

    /**
     * @brief Stops the app
     */
    void stop();
};

class GameState : public hl::silva::State {
protected:
    hl::silva::registry& ecs = PAA_ECS;
    paa::Window& window = PAA_SCREEN;
    paa::App& app = PAA_APP;
    hl::silva::StateManager& scene_manager = PAA_SCENE_MANAGER;
    galbar::InputHandler& input = PAA_INPUT;
    paa::ResourceManager& resource_manager = PAA_RESOURCE_MANAGER;
    paa::AnimationRegister& animation_register = PAA_ANIMATION_REGISTER;
    paa::BatchRenderer& batch_renderer = PAA_BATCH_RENDERER;
    paa::DeltaTimer& delta = PAA_DELTA_TIMER;

public:
    /**
     * @brief Construct a new Game State object
     */
    GameState() = default;

    /**
     * @brief Destroy the Game State object
     */
    ~GameState() = default;

    /**
     * @brief Default update function
     */
    void update() override { }

    /**
     * @brief Default handleEvent function
     */
    void handleEvent() override { }
};

/**
 * @brief Changes the current state of the app
 *
 * @tparam T The next scene
 */
template <typename T> static inline void scene_change_meta()
{
    paa::SceneManager::get().changeState<T>();
}

/**
 * @brief Push a new scene on the current state of the app
 *
 * @tparam T The next scene
 */
template <typename T> static inline void scene_push_meta()
{
    paa::SceneManager::get().pushState<T>();
}

/**
 * @brief Pop the current state of the app
 */
static inline void scene_pop_meta() { paa::SceneManager::get().popState(); }

/**
 * @brief Load a configuration file and setup the system
 *
 * @param configuration_file The name of the file
 */
void setup_paa_system(const std::string& configuration_file);
void stop_paa_system();
}