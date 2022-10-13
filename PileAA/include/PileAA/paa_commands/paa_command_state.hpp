#pragma once

/**
 * @brief 
 */
#define PAA_SCENE_DECL(name) struct name

/**
 * @brief Base for a new scene
 */
#define PAA_SCENE(name) PAA_SCENE_DECL(name) : public paa::GameState

/**
 * @brief Entry point of the scene
 */
#define PAA_START(name) name()

/**
 * @brief End point of the scene
 */
#define PAA_END(name) ~name()

/**
 * @brief Default declarations for a scene
 */
#define PAA_SCENE_DEFAULT(name) \
    PAA_START(name); \
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
#define PAA_START_CPP(name) name::name()

/**
 * @brief End point of the scene for cpp
 */
#define PAA_END_CPP(name) name::~name()

/**
 * @brief Update section of the scene for cpp
 */
#define PAA_UPDATE_CPP(name) void name::update()

/**
 * @brief Event section of the scene for cpp
 */
#define PAA_EVENTS_CPP(name) void name::handleEvent()

/**
 * @brief Set the current scene
 */
#define PAA_SET_SCENE(scene_name) paa::scene_change_meta<scene_name>()

/**
 * @brief Push a new scene
 */
#define PAA_PUSH_SCENE(scene_name) paa::scene_push_meta<scene_name>()

/**
 * @brief Pop the current scene
 */
#define PAA_POP_SCENE() paa::scene_pop_meta()