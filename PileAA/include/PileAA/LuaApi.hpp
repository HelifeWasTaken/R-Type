#pragma once

#include "external/sol/sol.hpp"
#include "AnimatedSprite.hpp"
#include "App.hpp"

namespace paa
{
    class LuaApi
    {
    public:
        static void load_api(sol::state& lua)
        {
            lua.new_usertype<AnimatedSprite>("AnimatedSprite",
                sol::constructors<AnimatedSprite(const std::string&)>(),
                "registerAnimation", &AnimatedSprite::registerAnimation,
                "useAnimation", &AnimatedSprite::useAnimation,
                "update", &AnimatedSprite::update,
                "setColor", &AnimatedSprite::setColor
                /*
                "setPosition", &AnimatedSprite::setPosition,
                "setRotation", &AnimatedSprite::setRotation,
                "setScale", &AnimatedSprite::setScale,
                "setOrigin", &AnimatedSprite::setOrigin,
                "move", &AnimatedSprite::move,
                "rotate", &AnimatedSprite::rotate
                */
            );

            lua.new_usertype<paa::Vec2>("Vec2",
                sol::constructors<paa::Vec2(const double&, const double&)>(),
                "x", &paa::Vec2::x,
                "y", &paa::Vec2::y
            );

            lua.new_usertype<paa::Position>("Position",
                sol::constructors<paa::Position(const double&, const double&)>(),
                "x", &paa::Position::x,
                "y", &paa::Position::y
            );

            lua.new_usertype<paa::Velocity>("Velocity",
                sol::constructors<paa::Velocity(const double&, const double&)>(),
                "x", &paa::Velocity::x,
                "y", &paa::Velocity::y
            );

            lua.new_usertype<paa::Depth>("Depth",
                sol::constructors<paa::Depth(const double&)>(),
                "z", &paa::Depth::z
            );

            lua.new_usertype<paa::Color>("Color",
                sol::constructors<paa::Color(const double&, const double&, const double&, const double&)>(),
                "r", &paa::Color::r,
                "g", &paa::Color::g,
                "b", &paa::Color::b,
                "a", &paa::Color::a
            );

            lua.new_usertype<paa::Id>("Id",
                sol::constructors<paa::Id(const int&)>(),
                "id", &paa::Id::id
            );

            lua.new_usertype<paa::Health>("Health",
                sol::constructors<paa::Health(const double&)>(),
                "health", &paa::Health::hp
            );

            lua.new_usertype<paa::ResourceManager>("ResourceManager",
                "loadTexture", &paa::ResourceManager::load<Texture>,
                "loadFont", &paa::ResourceManager::load<Font>,
                "loadSound", &paa::ResourceManager::load<SoundBuffer>,
                "loadImage", &paa::ResourceManager::load<Image>,
                "getTexture", &paa::ResourceManager::get<Texture>,
                "getFont", &paa::ResourceManager::get<Font>,
                "getSound", &paa::ResourceManager::get<SoundBuffer>,
                "getImage", &paa::ResourceManager::get<Image>,
                "remove", &paa::ResourceManager::remove,
                "clear", &paa::ResourceManager::clear
            );

            lua.set_function("getResourceManager", []() -> paa::ResourceManager& {
                return PAA_RESOURCE_MANAGER;
            });

            lua.new_usertype<hl::silva::Entity>("EntityTag",
                "getId", &hl::silva::Entity::get_id
            );

            lua.new_usertype<DynamicEntity>("Entity",
                sol::constructors<DynamicEntity(const hl::silva::Entity&)>(),
                "setEntity", &DynamicEntity::setEntity,
                "getEntity", &DynamicEntity::getEntity,
                "getId", &DynamicEntity::getId,
                "isAlive", &DynamicEntity::isAlive,
                "kill", &DynamicEntity::kill,
                "attachSprite", &DynamicEntity::attachSprite,
                "attachPosition", &DynamicEntity::attachPosition
            );

            lua.set_function("newEntity", []() -> DynamicEntity {
                return DynamicEntity(PAA_NEW_ENTITY());
            });

            lua.set_function("addSprite", [](DynamicEntity& entity, const std::string& texture) -> AnimatedSprite {
                return *entity.attachSprite(texture);
            });
        }
    };

}