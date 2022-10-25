
//#include <iostream>

#include "ClientScenes.hpp"
#include "Enemies.hpp"
#include "Player.hpp"

Game g_game;

PAA_SCENE(ecs) {

    PAA_SCENE_DEFAULT(ecs);

    PAA_START {
        PAA_REGISTER_COMPONENTS(rtype::game::Enemy, rtype::game::Bullet,
                                rtype::game::Player);

        PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
            for (const auto&& [_, b] : r.view<rtype::game::Bullet>())
                b->update();
            for (const auto&& [_, e] : r.view<rtype::game::Enemy>())
                e->update();
        });

        PAA_REGISTER_SYSTEM([](hl::silva::registry& r) {
            for (auto&& [e, player, id] : r.view<rtype::game::Player, paa::Id>()) {
                player->update();
                if (player->is_dead()) {
                    // TODO: Send message to kill player By id
                    r.kill_entity(e);
                }
            }
        });
    }

    PAA_END {}

    PAA_UPDATE { PAA_SET_SCENE(client_connect); }
};

int main(int argc, char** argv)
{
    try {
        std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());
        paa::setup_paa_system("../Resources.conf");

        PAA_REGISTER_SCENE(ecs);
        PAA_REGISTER_SCENE(create_room);
        PAA_REGISTER_SCENE(client_connect);
        PAA_REGISTER_SCENE(connect_room);
        PAA_REGISTER_SCENE(game_scene);
        PAA_REGISTER_SCENE(waiting_room);

        PAA_SET_SCENE(ecs);

        bool res = PAA_APP.run();
        paa::stop_paa_system();
        return 0;
    } catch (const paa::AABaseError& e) {
        spdlog::critical("paa::AABaseError: Error: {}", e.what());
    } catch (const std::exception& e) {
        spdlog::critical("std::exception: Error: {}", e.what());
    } catch (...) {
        spdlog::critical("Unknown error");
    }
    return 1;
}
