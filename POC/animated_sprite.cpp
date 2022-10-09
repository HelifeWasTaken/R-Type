#include "poc.hpp"
#include <PileAA/Timer.hpp>
#include <PileAA/AnimatedSprite.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>
#include <PileAA/App.hpp>

PAA_SCENE(poc_scene_animated_sprite) {
    PAA_START(poc_scene_animated_sprite) {
        PAA_ENTITY e = PAA_NEW_ENTITY();
        PAA_SET_SPRITE(e, "spaceship").useAnimation("idle");
    }
};

void poc_animated_sprite(void)
{
    paa::setup_paa_system("../Resources.conf");
    PAA_SCENE_MANAGER.pushState<poc_scene_animated_sprite>();
    PAA_APP.run();
    paa::stop_paa_system();
}
