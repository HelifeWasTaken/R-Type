#pragma once

/**
 * @brief Gets the ecs
 */
#define PAA_ECS paa::EcsInstance::get()

/**
 * @brief Gets the window
 */
#define PAA_SCREEN paa::Screen::get()

/**
 * @brief Gets the App
 */
#define PAA_APP paa::App::get()

/**
 * @brief Gets the scene manager
 */
#define PAA_SCENE_MANAGER paa::SceneManager::get()

/**
 * @brief Gets the input handler
 */
#define PAA_INPUT paa::InputHandler::get()

/**
 * @brief Gets the resource manager
 */
#define PAA_RESOURCE_MANAGER paa::ResourceManagerInstance::get()

/**
 * @brief Gets the animation register
 */
#define PAA_ANIMATION_REGISTER paa::AnimationRegisterInstance::get()

/**
 * @brief Gets the batch renderer
 */
#define PAA_BATCH_RENDERER paa::BatchRendererInstance::get()

/**
 * @brief Gets the delta timer
 */
#define PAA_DELTA_TIMER paa::DeltaTimerInstance::get()