#pragma once

#define PAA_SCENE(name) struct name : public paa::GameState

#define PAA_START(name) name()

#define PAA_END(name) ~name()

#define PAA_UPDATE void update() override

#define PAA_EVENTS void handleEvent() override

#define PAA_SET_SCENE(scene_name) paa::scene_change_meta<scene_name>()

#define PAA_PUSH_SCENE(scene_name) paa::scene_push_meta<scene_name>()

#define PAA_POP_SCENE() paa::scene_pop_meta()

#define PAA_METHOD(name) void name()

#define PAA_USE_METHOD(name) name()