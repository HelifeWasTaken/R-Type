#pragma once

#include "paa_getters.hpp"

/**
 * @brief Register n components to the ECS registry.
 */
#define PAA_REGISTER_COMPONENTS(...) PAA_ECS.registerComponents<__VA_ARGS__>()

/**
 * @brief Register a new system to the ECS registry.
 */
#define PAA_REGISTER_SYSTEM(system) PAA_ECS.add_system(system)

/**
 * @brief Returns a new entity
 */
#define PAA_NEW_ENTITY() PAA_ECS.spawn_entity()

/**
 * @brief Destroy an entity
 */
#define PAA_DESTROY_ENTITY(entity) PAA_ECS.kill_entity(entity)

/**
 * @brief Set a component to an entity
 */
#define PAA_SET_COMPONENT(entity, component_type, ...)                         \
    PAA_ECS.emplace<component_type>(entity, __VA_ARGS__)

/**
 * @brief Remove a component from an entity
 */
#define PAA_REMOVE_COMPONENT(entity, component_type)                           \
    PAA_ECS.remove<component_type>(entity)

/**
 * @brief View all entities with the given components
 */
#define PAA_VIEW_COMPONENTS(...) PAA_ECS.view<__VA_ARGS__>()

/**
 * @brief Iterate all entities on the view
 */
#define PAA_ITERATE_VIEW(view, ...)                                            \
    for (auto&& [paa_entity_id, __VA_ARGS__] : view)

/**
 * @brief Get the component of an entity
 */
#define PAA_GET_COMPONENT(entity, component)                                   \
    PAA_ECS.get_component<component>(entity)

#define PAA_ENTITY hl::silva::Entity
#define PAA_VIEW auto
#define PAA_COMPONENT auto&
