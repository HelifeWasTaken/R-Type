#pragma once

#define ARCADE_SCREEN_SIZE_X (384.F)
#define ARCADE_SCREEN_SIZE_Y (256.F)

#define ARCADE_SCREEN_VIEW_X (ARCADE_SCREEN_SIZE_X / 2.F)
#define ARCADE_SCREEN_VIEW_Y (ARCADE_SCREEN_SIZE_Y / 2.F)

#define RTYPE_PLAYFIELD_LEFT (0.F)
#define RTYPE_PLAYFIELD_TOP (0.F)
#define RTYPE_PLAYFIELD_WIDTH (ARCADE_SCREEN_SIZE_X)
#define RTYPE_PLAYFIELD_HEIGHT (205.F)
#define RTYPE_PLAYFIELD_RECT(t) (paa::recTo<T>(paa::FloatRect(RTYPE_PLAYFIELD_LEFT, RTYPE_PLAYFIELD_TOP, RTYPE_PLAYFIELD_WIDTH, RTYPE_PLAYFIELD_HEIGHT)))

#define RTYPE_HUD_LEFT (0.F)
#define RTYPE_HUD_TOP (500)
#define RTYPE_HUD_HEIGHT (RTYPE_HUD_TOP - 600)
#define RTYPE_HUD_WIDTH (ARCADE_SCREEN_SIZE_X)
#define RTYPE_HUD_RECT (paa::recTo<T>(paa::FloatRect(RTYPE_HUD_LEFT, RTYPE_HUD_TOP, RTYPE_HUD_HEIGHT, RTYPE_HUD_WIDTH)))

#define RTYPE_MENU_CENTER_X (400)
#define RTYPE_MENU_CENTER_Y (300)

#define RTYPE_MENU_CENTERED_X(element) (RTYPE_MENU_CENTER_X - (int)((element).getGlobalBounds().width / 2))
#define RTYPE_MENU_CENTERED_Y(element) (RTYPE_MENU_CENTER_Y - (int)((element).getGlobalBounds().height / 2))

#define RTYPE_CLAMP(t, v, l, h) \
    std::clamp(static_cast<t>(v), static_cast<t>(l), static_cast<t>(h))

#define MUSIC_COMBAT_IS_OVER "../assets/a_combat_is_over.ogg"
