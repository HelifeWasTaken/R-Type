
#include "ClientScenes.hpp"
#include <iostream>

#include "Enemies.hpp"

Game g_game;

PAA_SCENE(ecs)
{

    PAA_START(ecs)
    {
        PAA_REGISTER_COMPONENTS(Enemy, Bullet);
        PAA_REGISTER_SYSTEM(
            [](hl::silva::registry& r) {
                for (const auto&& [_, b] : r.view<Bullet>())
                    b->update();
                for (const auto&& [_, e] : r.view<Enemy>())
                    e->update();
            }
        );

        PAA_SET_SCENE(client_connect);
    }

}

PAA_UNSAFE_PROGRAM_START(ecs, "../Resources.conf");
