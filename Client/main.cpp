
//#include <iostream>

#include "Bullet.hpp"
#include "ClientScenes.hpp"
#include "Enemies.hpp"
#include "Map.hpp"
#include "Player.hpp"

#include "RServer/Messages/Messages.hpp"

Game g_game;

static void register_bullet_system()
{
    PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
        for (const auto&& [e, b] : r.view<rtype::game::Bullet>()) {
            b->update();
            if (!b->is_alive()) {
                r.kill_entity(e);
            }
        }
    });
}

static void register_enemy_system()
{
    PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
        const auto view = PAA_SCREEN.getView();
        const double left_border
            = view.getCenter().x - view.getSize().x / 2 - 100;

        unsigned int count = 0;
        for (const auto&& [entity, enemy, id] :
            r.view<rtype::game::Enemy, paa::Id>()) {
            enemy->update();
            const auto& hp = PAA_GET_COMPONENT(entity, paa::Health);
            const auto& pos = PAA_GET_COMPONENT(entity, paa::Position);
            if (hp.hp <= 0 || pos.x < left_border) {
                r.kill_entity(entity);
                g_game.enemies_to_entities.erase(id.id);
                g_game.service.tcp().send(rtype::net::UpdateMessage(g_game.id,
                    SerializedEnemyDeath(id.id),
                    rtype::net::message_code::UPDATE_ENEMY_DESTROYED));
            }
            count++;
        }

        if (count == 0 && g_game.lock_scroll) {
            g_game.lock_scroll = false;
        }
    });
}

static void register_player_system()
{
    PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
        for (auto&& [e, player, id] : r.view<rtype::game::Player, paa::Id>()) {
            player->update();
            if (player->is_dead()) {
                r.kill_entity(e);
                g_game.service.tcp().send(rtype::net::UpdateMessage(g_game.id,
                    SerializedPlayerDeath(id.id),
                    rtype::net::message_code::UPDATE_PLAYER_DESTROYED));
                g_game.players_alive[id.id] = false;
            }
        }
    });
}

PAA_SCENE(ecs)
{

    PAA_SCENE_DEFAULT(ecs);

    PAA_START
    {
        PAA_REGISTER_COMPONENTS(rtype::game::Enemy, rtype::game::Bullet,
            rtype::game::Player, rtype::game::EffectZones::EffectZoneData);

        register_bullet_system();
        register_enemy_system();
        register_player_system();

        g_game.hud_view = PAA_SCREEN.getView();
        g_game.reset_game_view();
    }

    PAA_END { }

    PAA_UPDATE { PAA_SET_SCENE(client_connect); }
};

void init_logging()
{
    #if CMAKE_BUILD_TYPE == Release
        spdlog::set_level(spdlog::level::level_enum::critical);
    #endif
}

#include "PileAA/LuaApi.hpp"

/*
PAA_SCENE(test)
{
    sol::state lua;

    PAA_START
    {
        paa::LuaApi::load_api(lua);
        //
        // Lua example:
        // addSprite(newEntity(), "textureName").useAnimation("animationName")
        //
        lua.script_file("init.lua");
    }

    PAA_UPDATE
    {
        lua.script_file("update.lua");
    }
};
*/

PAA_UNSAFE_MAIN("../Resources.conf", {
    init_logging();
    PAA_REGISTER_SCENE(ecs);
    PAA_REGISTER_SCENE(create_room);
    PAA_REGISTER_SCENE(client_connect);
    PAA_REGISTER_SCENE(connect_room);
    PAA_REGISTER_SCENE(game_scene);
    PAA_REGISTER_SCENE(waiting_room);
    PAA_SET_SCENE(ecs);
});
