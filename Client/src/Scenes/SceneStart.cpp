#include "ClientScenes.hpp"

PAA_START_CPP(scene_start)
{
    g_game.launch_transition_halfway();

    timer.setTarget(2000);
    timer.restart();

    logoEntity = PAA_NEW_ENTITY();
    logoEntity.attachSprite("logo_pileaa");
    auto& s = logoEntity.getComponent<paa::Sprite>();
    s->setPosition(RTYPE_MENU_CENTERED_X(*s), RTYPE_MENU_CENTERED_Y(*s));
}

PAA_END_CPP(scene_start) { }

PAA_UPDATE_CPP(scene_start)
{
    g_game.use_hud_view();
    if (g_game.in_transition()) {
        g_game.transition.update();
        if (timer.isFinished() && g_game.transition_is_halfway()) {
            PAA_SET_SCENE(client_connect);
        }
    } else if (timer.isFinished()) {
        g_game.launch_transition();
    }
    g_game.transition.draw();
}
