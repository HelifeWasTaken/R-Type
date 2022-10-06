#pragma once

#include "paa_getters.hpp"

#define PAA_REGISTER_COMPONENTS(...) PAA_ECS.registerComponents<__VA_ARGS__>()

#define PAA_REGISTER_SYSTEM(system) PAA_ECS.add_system(system)

#define PAA_NEW_ENTITY() PAA_ECS.spawn_entity()

#define PAA_SET_COMPONENT(entity, component_type, ...) \
    PAA_ECS.emplace<component_type>(entity, __VA_ARGS__)

#define PAA_REMOVE_COMPONENT(entity, component_type) PAA_ECS.remove<component_type>(entity)

#define PAA_VIEW_COMPONENTS(...) PAA_ECS.view<__VA_ARGS__>()

#define PAA_ITERATE_VIEW(view, ...) for (auto&& [paa_entity_id, __VA_ARGS__] : view)

#define PAA_GET_COMPONENT(entity, component) PAA_ECS.get_component<component>(entity)

#define PAA_ENTITY hl::silva::Entity
#define PAA_VIEW auto
#define PAA_COMPONENT auto&

