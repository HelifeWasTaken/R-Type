#include "PileAA/AnimatedSprite.hpp"
#include "PileAA/App.hpp"
#include "RServer/Client/Client.hpp"
#include "ClientWrapper.hpp"

PAA_SCENE(mystate) {

    RTYPE_CLIENT client;

    PAA_START(mystate) {
        PAA_ENTITY e = PAA_NEW_ENTITY();
        PAA_SET_SPRITE(e, "spaceship");
        PAA_GET_COMPONENT(e, paa::Sprite).useAnimation("idle");

        client.run("../Client.conf");
    }

    PAA_UPDATE {
        client.restart_if_necessary();
    }
};

PAA_PROGRAM_START(mystate, "../Resources.conf", true);
