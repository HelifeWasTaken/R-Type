#include "PileAA/AnimatedSprite.hpp"
#include "PileAA/App.hpp"
#include "RServer/Client/Client.hpp"
#include "ClientWrapper.hpp"

#include <unordered_map>

using namespace rtype::game;

PAA_SCENE(mystate) {

    // RTYPE_CLIENT client;

    PAA_START(mystate) {
        PAA_ENTITY e = PAA_NEW_ENTITY();
        PAA_SET_SPRITE(e, "spaceship").useAnimation("idle");
        PAA_GET_COMPONENT(e, paa::Sprite).useAnimation("idle");

        //client.run("../Client.conf");
    }

    // std::unordered_map<int, PAA_ENTITY> entities;

    PAA_UPDATE {
        //if (client.restart_if_necessary() == false)
        //    return;
    }
};

PAA_PROGRAM_START(mystate, "../Resources.conf", true);
