#pragma once

#include "external/HelifeWasTaken/SilvaState"

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
    static inline void scene_change_meta() { paa::SceneManager::get().changeState<T>(); }

    template<typename T>
    static inline void scene_push_meta() { paa::SceneManager::get().pushState<T>(); }

    template<typename T>
    static inline void scene_pop_meta() { paa::SceneManager::get().popState(); }

    void setup_paa_system();
    void stop_paa_system();
}

#define PAA_STATE(name) struct name : public paa::GameState

#define PAA_START(name) name()

#define PAA_END(name) ~name()

#define PAA_UPDATE void update() override

#define PAA_EVENTS void handleEvent() override

#define PAA_SET_SCENE(scene_name) paa::scene_change_meta<scene_name>()

#define PAA_PUSH_SCENE(scene_name) paa::scene_push_meta<scene_name>()

#define PAA_POP_SCENE() paa::scene_pop_meta()

#define PAA_METHOD(name) void name()

#define PAA_CALL_METHOD(name) name()

#define PAA_ECS paa::EcsInstance::get()

#define PAA_REGISTER_COMPONENTS(...) PAA_ECS.registerComponents<__VA_ARGS__>()

#define PAA_REGISTER_SYSTEM(system) PAA_ECS.add_system(system)

#define PAA_NEW_ENTITY() PAA_ECS.spawn_entity()

#define PAA_SET_COMPONENT(entity, component_type, ...) \
    PAA_ECS.emplace<component_type>(entity, __VA_ARGS__)

#define PAA_REMOVE_COMPONENT(entity, component_type) PAA_ECS.remove<component_type>(entity)

#define PAA_VIEW_COMPONENTS(...) PAA_ECS.view<__VA_ARGS__>()

#define PAA_ITERATE_VIEW(view, ...) for (auto&& [paa_entity_id, __VA_ARGS__] : view)

#define PAA_GET_COMPONENT(entity, component) PAA_ECS.get_component<component>(entity)

#define PAA_OBJECT auto
#define PAA_ENTITY hl::silva::Entity
#define PAA_ENTITY_ID hl::silva::Entity::Id
#define PAA_VIEW auto
#define PAA_COMPONENT auto&

#define PAA_PROGRAM_START(baseScene) \
    int main() \
    { \
        paa::setup_paa_system(); \
        paa::SceneManager::get().changeState<baseScene>(); \
        paa::App::get().run(); \
        paa::stop_paa_system(); \
        return 0; \
    }
