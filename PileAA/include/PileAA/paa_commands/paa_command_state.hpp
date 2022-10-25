#pragma once

#define PAA_FUN_NAMESPACE(name) paa_scene_##name

/**
 * @brief
 */
#define PAA_SCENE_DECL(name) struct PAA_FUN_NAMESPACE(name)

/**
 * @brief Base for a new scene
 */
#define PAA_SCENE(name)                                                        \
    PAA_SCENE_DECL(name)                                                       \
        : public paa::GameState

/**
 * @brief Entry point of the scene
 */
#define PAA_START void start() override

/**
 * @brief End point of the scene
 */
#define PAA_END void end() override

/**
 * @brief Default declarations for a scene
 */
#define PAA_SCENE_DEFAULT(name)                                              \
    PAA_FUN_NAMESPACE(name)() = default;                                       \
    ~PAA_FUN_NAMESPACE(name)() = default

/**
 * @brief Update section of the scene
 */
#define PAA_UPDATE void update() override

/**
 * @brief Event section of the scene
 */
#define PAA_EVENTS void handleEvent() override

/**
 * @brief Entry point of the scene for cpp
 */
#define PAA_START_CPP(name) void PAA_FUN_NAMESPACE(name)::start()

/**
 * @brief End point of the scene for cpp
 */
#define PAA_END_CPP(name) void PAA_FUN_NAMESPACE(name)::end()

/**
 * @brief Update section of the scene for cpp
 */
#define PAA_UPDATE_CPP(name) void PAA_FUN_NAMESPACE(name)::update()

/**
 * @brief Event section of the scene for cpp
 */
#define PAA_EVENTS_CPP(name) void PAA_FUN_NAMESPACE(name)::handleEvent()

/**
 * @brief Register a scene
 */
#define PAA_REGISTER_SCENE(type) \
    PAA_SCENE_MANAGER.registerState(#type, new PAA_SCENE_DECL(type)())

/**
 * @brief Set a scene
 */
#define PAA_SET_SCENE(type) \
    PAA_SCENE_MANAGER.changeState(#type)

/**
 * @brief Push a scene
 */
#define PAA_PUSH_SCENE(type) \
    PAA_SCENE_MANAGER.pushState(#type)

/**
 * @brief Pop a scene
 */
#define PAA_POP_SCENE \
    PAA_SCENE_MANAGER.popState
