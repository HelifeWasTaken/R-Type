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
#define PAA_START(name) PAA_FUN_NAMESPACE(name)()

/**
 * @brief End point of the scene
 */
#define PAA_END(name) ~PAA_FUN_NAMESPACE(name)()

/**
 * @brief Default declarations for a scene
 */
#define PAA_SCENE_DEFAULT(name)                                                \
    PAA_START(name);                                                           \
    PAA_END(name) = default;

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
#define PAA_START_CPP(name) PAA_FUN_NAMESPACE(name)::PAA_FUN_NAMESPACE(name)()

/**
 * @brief End point of the scene for cpp
 */
#define PAA_END_CPP(name) PAA_FUN_NAMESPACE(name)::~PAA_FUN_NAMESPACE(name)()

/**
 * @brief Update section of the scene for cpp
 */
#define PAA_UPDATE_CPP(name) void PAA_FUN_NAMESPACE(name)::update()

/**
 * @brief Event section of the scene for cpp
 */
#define PAA_EVENTS_CPP(name) void PAA_FUN_NAMESPACE(name)::handleEvent()

/**
 * @brief User defined method
 */
#define PAA_METHOD(name, ret, ...) ret name(__VA_ARGS__)

/**
 * @brief User defined method for cpp
 */
#define PAA_METHOD_CPP(scene_name, fun_name, ret, ...)                         \
    ret PAA_FUN_NAMESPACE(scene_name)::fun_name(__VA_ARGS__)

/**
 * @brief Defined call method
 */
#define PAA_CALL_METHOD(name, ...) PAA_FUN_NAMESPACE(name)(__VA_ARGS__)

/**
 * @brief Set the current scene
 */
#define PAA_SET_SCENE(scene_name)                                              \
    paa::scene_change_meta<PAA_FUN_NAMESPACE(scene_name)>()

/**
 * @brief Push a new scene
 */
#define PAA_PUSH_SCENE(scene_name)                                             \
    paa::scene_push_meta<PAA_FUN_NAMESPACE(scene_name)>()

/**
 * @brief Pop the current scene
 */
#define PAA_POP_SCENE() paa::scene_pop_meta()
