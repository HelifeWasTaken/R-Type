
#include "ClientScenes.hpp"
#include <iostream>

#include "Enemies.hpp"
#include "Player.hpp"

Game g_game;

PAA_SCENE(ecs)
{

    PAA_START(ecs)
    {
        PAA_REGISTER_COMPONENTS(rtype::game::Enemy, rtype::game::Bullet);

        PAA_REGISTER_SYSTEM(
            [](hl::silva::registry& r) {
                for (const auto&& [_, b] : r.view<rtype::game::Bullet>())
                    b->update();
                for (const auto&& [_, e] : r.view<rtype::game::Enemy>())
                    e->update();
            }
        );

        PAA_REGISTER_COMPONENTS(rtype::game::Player);
        PAA_REGISTER_SYSTEM(
            [](hl::silva::registry& r) {
                for (auto&& [e, player, id] : r.view<rtype::game::Player, paa::Id>()) {
                    player->update();
                    if (player->is_dead()) {
                        // TODO: Send message to kill player By id
                        r.kill_entity(e);
                    }
                }
            }
        );
    }

    PAA_UPDATE
    {
        PAA_SET_SCENE(client_connect);
    }

};

PAA_UNSAFE_PROGRAM_START(ecs, "../Resources.conf");
