#pragma once

#include "external/HelifeWasTaken/SilvaState"

#include "BaseComponents.hpp"
#include "InputHandler.hpp"
#include "ResourceManager.hpp"
#include "AnimatedSprite.hpp"

#include "paa_commands/paa_getters.hpp"
#include "paa_commands/paa_command_ecs.hpp"
#include "paa_commands/paa_command_state.hpp"
#include "paa_commands/paa_utilities.hpp"
#include "paa_commands/paa_command_main.hpp"

namespace paa {

    HL_SINGLETON_IMPL(hl::silva::registry, EcsInstance);
    HL_SINGLETON_IMPL(hl::silva::StateManager, SceneManager);
    HL_SINGLETON_IMPL(RenderWindow, Screen);

    class App {
    private:
        static inline App *_instance = nullptr;
        App() = default;

    public:
        ~App() = default;

        static App& get();
        static void release();

    private:
        bool isRunning() const;
        void update();

    public:
        void run();
        void stop();
    };

    class GameState : public hl::silva::State {
    protected:
        paa::App& app = PAA_APP;
        galbar::InputHandler &input = PAA_INPUT;
        paa::ResourceManager &resource_manager = PAA_RESOURCE_MANAGER;
        paa::Window &window = PAA_SCREEN;
        hl::silva::registry &ecs = PAA_ECS;

    public:
        GameState() = default;
        ~GameState() = default;

        void update() override {}
        void handleEvent() override {}
    };

    template<typename T>
    static inline void scene_change_meta() { paa::SceneManager::get().changeState<T>(); }

    template<typename T>
    static inline void scene_push_meta() { paa::SceneManager::get().pushState<T>(); }

    template<typename T>
    static inline void scene_pop_meta() { paa::SceneManager::get().popState(); }

    void setup_paa_system();
    void stop_paa_system();
}