
//#include <iostream>

#include "ClientScenes.hpp"
#include "Enemies.hpp"
#include "Bullet.hpp"
#include "Player.hpp"
#include "Map.hpp"

Game g_game;

PAA_SCENE(ecs) {

    PAA_SCENE_DEFAULT(ecs);

    PAA_START {
        PAA_REGISTER_COMPONENTS(rtype::game::Enemy, rtype::game::Bullet,
                                rtype::game::Player, rtype::game::EffectZones::EffectZoneData);

        PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
            for (const auto&& [e, b] : r.view<rtype::game::Bullet>()) {
                b->update();
                if (!b->is_alive())
                    r.kill_entity(e);
            }
        });

        PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
            const auto view = PAA_SCREEN.getView();
            const double left_border = view.getCenter().x - view.getSize().x / 2 - 100;

            unsigned int count = 0;
            for (const auto&& [entity, enemy] : r.view<rtype::game::Enemy>()) {
                enemy->update();
                const auto& hp = PAA_GET_COMPONENT(entity, paa::Health);
                const auto& pos = PAA_GET_COMPONENT(entity, paa::Position);
                if (hp.hp <= 0 || pos.x < left_border) {
                    r.kill_entity(entity);
                }
                count++;
            }

            if (count == 0 && g_game.lock_scroll) {
                g_game.lock_scroll = false;
            }
        });

        PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
            for (auto&& [e, player, id] : r.view<rtype::game::Player, paa::Id>()) {
                player->update();
                if (player->is_dead()) {
                    r.kill_entity(e);
                    // TODO: Send message to kill player By id
                }
            }
        });
    }

    PAA_END {}

    PAA_UPDATE { PAA_SET_SCENE(client_connect); }
};

PAA_MAIN("../Resources.conf", {
    PAA_REGISTER_SCENE(ecs);
    PAA_REGISTER_SCENE(create_room);
    PAA_REGISTER_SCENE(client_connect);
    PAA_REGISTER_SCENE(connect_room);
    PAA_REGISTER_SCENE(game_scene);
    PAA_REGISTER_SCENE(waiting_room);
    PAA_SET_SCENE(ecs);
});