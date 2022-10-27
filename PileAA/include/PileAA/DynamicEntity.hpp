#pragma once

#include "App.hpp"
#include "BaseComponents.hpp"
#include "external/HelifeWasTaken/Silva"

namespace paa {

/**
 * @brief The dynamic Entity is a way to use the entity not anymore as an ID
 * but as a real object that can be updated.
 *
 * Technically this is just a wrapper around the entity that will be used
 * to update the entity.
 *
 * instead of doing this: registry.get<Position>(entity);
 * you can do this: dynamicEntity.get<Position>();
 *
 * The dynamic entity will always use the registry of the app.
 */
class DynamicEntity {
public:
    using registry_t = hl::silva::registry;
    using entity_t = hl::silva::Entity;
    using entity_id_t = hl::silva::Entity::Id;
    using component_t = hl::silva::registry::anonymous_t;

private:
    hl::silva::registry& _registry = EcsInstance::get();
    hl::silva::Entity _entity;

public:
    /**
     * @brief Construct a new Dynamic Entity object
     */
    DynamicEntity() = default;
    /**
     * @brief Construct a new Dynamic Entity object
     * @param id
     */
    DynamicEntity(const entity_id_t& id)
        : _entity(entity_t(id))
    {
    }
    /**
     * @brief Construct a new Dynamic Entity object
     * @param entity
     */
    DynamicEntity(const entity_t& entity)
        : _entity(entity)
    {
    }

    /**
     * @brief Destroy the Dynamic Entity object
     */
    ~DynamicEntity() = default;

    /**
     * @brief Set the Entity object
     * @param entity
     */
    void setEntity(const entity_t& entity) { _entity = entity; }

    /**
     * @brief Get the Id object
     * @return const entity_id_t&
     */
    const entity_id_t getId() const { return _entity.get_id(); }

    /**
     * @brief Get the Entity object
     * @return const entity_t&
     */
    const entity_t& getEntity() const { return _entity; }

    /**
     * @brief Tells if the entity has a component
     * @tparam Component
     * @return true
     * @return false
     */
    template <typename Component> bool hasComponent() const
    {
        return _registry.has_component<Component>(_entity);
    }

    /**
     * @brief Get the Component object
     *
     * @tparam Component The component to get
     * @return T& The component
     */
    template <typename Component> Component& getComponent()
    {
        return _registry.get_component<Component>(_entity);
    }

    /**
     * @brief Get the Component object (const)
     *
     * @tparam Component The component to get
     * @return const T& The component
     */
    template <typename Component> const Component& getComponent() const
    {
        return _registry.get_component<Component>(_entity);
    }

    /**
     * @brief Add a component to the entity
     *
     * @tparam Component The component to add
     * @param args The arguments to construct the component
     * @return T& The component
     */
    template <typename Component, typename... Params>
    void emplaceComponent(Params&&... p)
    {
        _registry.emplace<Component>(_entity, std::forward<Params>(p)...);
    }

    /**
     * @brief Insert a component to the entity by moving it
     *
     * @tparam Component The component to add
     * @param c The component to move
     * @return T& The component
     */
    template <typename Component> void insertComponent(Component&& c)
    {
        _registry.insert(_entity, std::move(c));
    }

    /**
     * @brief Remove a component from the entity
     *
     * @tparam Component The component to remove
     */
    template <typename T> void removeComponent()
    {
        _registry.remove_component<T>(_entity);
    }

    /**
     * @brief Remove all the components from the entity and set it as reusable
     * in the registry.
     * The entity will be destroyed at the start of the next frame.
     */
    void kill()
    {
        _registry.kill_entity(_entity);
        _entity = entity_t();
    }

    /**
     * @brief Tells if the entity is valid
     */
    bool isAlive() const { return _registry.is_alive(_entity); }

    /**
     * @brief Attach sprite component to the entity
     *        (This also attach a Depth component of 0 for the z-index)
     * @return The sprite
     */
    Sprite& attachSprite(const std::string& path)
    {
        PAA_SET_SPRITE(_entity, path);
        return getComponent<paa::Sprite>();
    }

    /**
     * @brief Attach a position component to the entity
     * @return The position
     */
    Position& attachPosition(const Position& pos)
    {
        emplaceComponent<Position>(pos);
        return getComponent<Position>();
    }

    /**
     * @brief Attach a position component to the entity
     * @return The Velocity
     */
    Velocity& attachVelocity(const Velocity& vel)
    {
        emplaceComponent<Velocity>(vel);
        return getComponent<Velocity>();
    }

    /**
     * @brief Attach a health component to the entity
     * @return The health
     */
    Health& attachHealth(const Health& health)
    {
        emplaceComponent<Health>(health);
        return getComponent<Health>();
    }

    /**
     * @brief Attach an id component to the entity
     * @return The id
     */
    Id& attachId(const Id& id)
    {
        emplaceComponent<Id>(id);
        return getComponent<Id>();
    }

    /**
     * @brief Attach a CollisionBox component to the entity
     * @return The collision
     */
    SCollisionBox& attachCollision(const SCollisionBox& collision)
    {
        emplaceComponent<SCollisionBox>(collision);
        return getComponent<SCollisionBox>();
    }

    operator PAA_ENTITY()
    {
        return _entity;
    }
};
}