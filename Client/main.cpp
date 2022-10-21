
#include "ClientScenes.hpp"
#include <iostream>

#include "Enemies.hpp"

#include "PileAA/QuadTree.hpp"

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

        PAA_SET_SCENE(client_connect);

        paa::Quadtree e(1, 1, 1, 1);
    }

};

PAA_UNSAFE_PROGRAM_START(ecs, "../Resources.conf");
