
//#include <iostream>

#include "Bullet.hpp"
#include "ClientScenes.hpp"
#include "Enemies.hpp"
#include "Map.hpp"
#include "Player.hpp"

#include "RServer/AsciiTitle.hpp"
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

    PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
        for (const auto&& [_, b] : r.view<rtype::game::BulletExplosion>()) {
            b.update();
        }
    });
}

static void register_enemy_system()
{
    PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
        const auto view = PAA_SCREEN.getView();
        const double left_border = view.getCenter().x - view.getSize().x / 2 - 100;
        unsigned int count = 0;

        for (const auto&& [entity, enemy, id] : r.view<rtype::game::Enemy, paa::Id>()) {
            enemy->update();
            const auto& hp = PAA_GET_COMPONENT(entity, paa::Health);
            const auto& pos = PAA_GET_COMPONENT(entity, paa::Position);
            if (enemy->is_alive() == false || hp.hp <= 0 || enemy->dies_when_leave_screen() && pos.x < left_border) {
                if (pos.x > left_border) {
                    g_game.score += 10;
                }
                r.kill_entity(entity);
                if (id.id != -1) {
                    g_game.enemies_to_entities.erase(id.id);
                    if (g_game.service.is_service_on())
                        g_game.service.tcp().send(rtype::net::UpdateMessage(g_game.id,
                            SerializedEnemyDeath(id.id),
                            rtype::net::message_code::UPDATE_ENEMY_DESTROYED));
                }
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
                if (g_game.service.is_service_on())
                    g_game.service.tcp().send(rtype::net::UpdateMessage(g_game.id,
                        SerializedPlayerDeath(id.id),
                        rtype::net::message_code::UPDATE_PLAYER_DESTROYED));
                g_game.players_alive[id.id] = false;
            }
        }
    });
}

/*
#include "PileAA/Parallax.hpp"

PAA_SCENE(test) {
    paa::Parallax parallax = paa::Parallax(
        paa::Vector2f(200, 30),
        paa::Vector2f(100, 80),
        { "parallax_menu_1", "parallax_menu_2", "parallax_menu_3" },
        paa::Vector2f(4, 4));

    PAA_START {}

    PAA_UPDATE {
        parallax.update();
    }

    PAA_END {}
};
*/

PAA_MAIN("../Resources.conf", {
    // PAA_REGISTER_SCENE(test);
    // PAA_SET_SCENE(test);

    AsciiTitle::print("Powered by PileAA");

    PAA_REGISTER_SCENE(create_room);
    PAA_REGISTER_SCENE(client_connect);
    PAA_REGISTER_SCENE(connect_room);
    PAA_REGISTER_SCENE(game_scene);
    PAA_REGISTER_SCENE(game_over);
    PAA_REGISTER_SCENE(game_win);
    PAA_REGISTER_SCENE(waiting_room);
    PAA_REGISTER_SCENE(scene_start);

    PAA_REGISTER_COMPONENTS(rtype::game::Enemy, rtype::game::Bullet,
        rtype::game::BulletExplosion,
        rtype::game::Player, rtype::game::EffectZones::EffectZoneData);

    rtype::game::RobotBossEye::register_robot_components();
    register_bullet_system();
    register_enemy_system();
    register_player_system();

    g_game.hud_view = PAA_SCREEN.getView();
    g_game.reset_game_view();

    PAA_SET_SCENE(scene_start);
});
