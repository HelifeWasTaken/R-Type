#pragma once

#include <SilvaState>

#include "BaseComponents.hpp"
#include "InputHandler.hpp"
#include "ResourceManager.hpp"
#include "AnimatedSprite.hpp"

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

    private:
        bool isRunning() const
        {
            bool screenIsOpen = Screen::get().isOpen();
            bool canBeUpdated = SceneManager::get().canBeUpdated();
            std::cout << "screenIsOpen: " << screenIsOpen << std::endl;
            std::cout << "canBeUpdated: " << canBeUpdated << std::endl;
            return screenIsOpen && canBeUpdated;
        }

        void update()
        {
            auto& window = Screen::get();
            auto& input = InputHandler::get();
            auto& ecs = EcsInstance::get();
            auto& scene = SceneManager::get();

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

    public:
        void run()
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

        void stop()
        {
            Screen::get().close();
            SceneManager::get().stop();
        }
    };

    class GameState : public hl::silva::State {
    protected:
        paa::App& app = paa::App::get();
        galbar::InputHandler &input = paa::InputHandler::get();
        paa::ResourceManager &resource_manager = paa::ResourceManagerInstance::get();
        paa::Window &window = paa::Screen::get();
        hl::silva::registry &ecs = paa::EcsInstance::get();

    public:
        GameState() = default;
        ~GameState() = default;

        void update() override {}
        void handleEvent() override {}
    };

    template<typename T>
    static inline void scene_change_meta()
    {
        paa::SceneManager::get().changeState<T>();
    }

    template<typename T>
    static inline void scene_push_meta()
    {
        paa::SceneManager::get().pushState<T>();
    }

    template<typename T>
    static inline void scene_pop_meta()
    {
        paa::SceneManager::get().popState();
    }

    static inline void setup_ecs(hl::silva::registry& r)
    {
        r.register_component<Position, Velocity, Sprite, AnimatedSprite>();
        r.add_system(animated_sprite_system);
    }

    static inline void setup_system()
    {
        InputHandler::get();
        ResourceManagerInstance::get();

        setup_ecs(EcsInstance::get());

        SceneManager::get();
        Screen::get().create(VideoMode(800, 600), "PileAA");
        App::get();
    }

    static inline void stop_system()
    {
        App::release();
        InputHandler::release();
        ResourceManagerInstance::release();
        EcsInstance::release();
        SceneManager::release();
    }

}

#define PAA_STATE(name) \
    struct name : public paa::GameState

#define PAA_START(name) \
    name()

#define PAA_END(name) \
    ~name()

#define PAA_UPDATE \
    void update() override

#define PAA_EVENTS \
    void handleEvent() override

#define PAA_SET_SCENE(scene_name) \
    paa::scene_change_meta<scene_name>()

#define PAA_PUSH_SCENE(scene_name) \
    paa::scene_push_meta<scene_name>()

#define PAA_POP_SCENE() \
    paa::scene_pop_meta()

#define PAA_PROGRAM_START(baseScene) \
    int main() \
    { \
        paa::setup_system(); \
        paa::SceneManager::get().changeState<baseScene>(); \
        paa::SceneManager::get().update(); \
        paa::App::get().run(); \
        paa::stop_system(); \
        return 0; \
    }

#define PAA_METHOD(name) \
    void name()

#define PAA_CALL_METHOD(name) \
    name()

#define PAA_REGISTER_COMPONENTS(...) \
    paa::EcsInstance::get().registerComponents<__VA_ARGS__>()

#define PAA_REGISTER_SYSTEM(system) \
    paa::EcsInstance::get().add_system(system)

#define PAA_NEW_ENTITY() \
    paa::EcsInstance::get().spawn_entity()

#define PAA_SET_COMPONENT(entity, component_type, ...) \
    paa::EcsInstance::get().emplace<component_type>(entity, __VA_ARGS__)

#define PAA_REMOVE_COMPONENT(entity, component_type) \
    paa::EcsInstance::get().remove<component_type>(entity)

#define PAA_VIEW_COMPONENTS(...) \
    paa::EcsInstance::get().view<__VA_ARGS__>()

#define PAA_ITERATE_VIEW(view, ...) \
    for (auto&& [paa_entity_id, __VA_ARGS__] : view)

#define PAA_OBJECT auto

#define PAA_ENTITY PAA_OBJECT
#define PAA_VIEW PAA_OBJECT

#define PAA_COMPONENT PAA_OBJECT&

#define PAA_METHOD_PRO(return_type, name, ...) \
    return_type name(__VA_ARGS__)

#define PAA_CALL_METHOD_PRO(name, ...) \
    name(__VA_ARGS__)
