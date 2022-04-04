#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "components.h"
#include "entity.h"
#include "input.h"
#include "shared.h"
#include "game.h"

#define MAX_TOWERS 1
#define MAX_TURRETS 3
#define MAX_BULLETS 10
#define MAX_UNITS 10

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

Scenes scenes;

typedef struct
{
  GameMemory memory;
  PlatformAPI api;

  // Window meta
  int screen_w, screen_h;

  bool initOnlyOnce;

  SceneUpdateFn *scene;

  // Entities
  Entity tower[MAX_TOWERS];
  int tower_count;
  Entity turret[MAX_TURRETS];
  int turret_count;
  Entity bullet[MAX_BULLETS];
  int bullet_count;
  Entity unit[MAX_UNITS];
  int unit_count;

  KeyboardAndMouseInputHandler kbm_input_handler;
  ControllerInputHandler controller_input_handler;

  bool game_paused;
  unsigned int current_turret;
} GameState;

static GameState *state;

static DrawComponent tower_draw_component = {
    .color = {.r = 0.886f, .g = 0.843f, .b = 0.773f, .a = 1.0f},
    .w = 50,
    .h = 50,
    .rotation = 0};

static DrawComponent turret_draw_component = {
    .color = {.r = 0.671f, .g = 0.486f, .b = 0.322f, .a = 1.0f},
    .w = 25,
    .h = 25,
    .rotation = 0};

/*
static DrawComponent bullet_draw_component = {
    .color = {.r = 0.663f, .g = 0.682f, .b = 0.471f, .a = 1.0f},
    .w = 5,
    .h = 5,
    .rotation = 0};

static DrawComponent unit_draw_component = {
    .color = {.r = 0.518, .g = 0.306f, .b = 0.165f, .a = 1.0f},
    .w = 15,
    .h = 15,
    .rotation = 0};
*/

// Input Handling

INPUT_COMMAND(pause_game)
{
  state->game_paused = !state->game_paused;
}

INPUT_COMMAND(next_turret)
{
  if (state->current_turret == state->turret_count)
    state->current_turret = 0;
  else
    state->current_turret++;
}

INPUT_COMMAND(rotate_left) {
  e->draw->rotation--;
  e->draw->rotation = MAX(e->draw->rotation, 0.0f);
}

INPUT_COMMAND(rotate_right) {
  e->draw->rotation++;
  e->draw->rotation = MIN(e->draw->rotation, 360.0f);
}

INPUT_COMMAND(increase_spread) {
  
}

INPUT_COMMAND(decrease_spread) {
  
}

INPUT_COMMAND(null_command) {}

// Scenes

SCENE_UPDATE(starting_scene) { state->scene = scenes.GoingScene; }

SCENE_UPDATE(ready_scene) { return; }

SCENE_UPDATE(going_scene) {
  static bool do_init = 0;
  Position half = {.x=state->screen_w/2.0f,.y=state->screen_h/2.0f};
  Position tower_half;
  Position turret_half;
  
  if (do_init) {
    do_init = 1;
    
    // TODO: Prototype the data in a data model for easier configuration
    // Init
    state->tower_count = 0;
    state->turret_count = 0;
    state->unit_count = 0;
    state->bullet_count = 0;
  
    // Place tower
    state->tower[0].draw = &tower_draw_component;
    tower_half.x = state->tower[0].draw->w/2.0f;
    tower_half.y = state->tower[0].draw->h/2.0f;
    state->tower[0].position.x = half.x - tower_half.x;
    state->tower[0].position.y = half.y - tower_half.y;
    state->tower_count++;

    // Place turrets
    state->turret[0].draw = &turret_draw_component;
    turret_half.x = state->turret[0].draw->w/2.0f;
    turret_half.y = state->turret[0].draw->h/2.0f;
    state->turret[0].position.x = half.x - tower_half.x - turret_half.x;
    state->turret[0].position.y = half.y + tower_half.y - turret_half.y;
    state->turret_count++;

    state->turret[1].draw = &turret_draw_component;
    // using the same draw, so do not need to recalc half, but if they differed...
    state->turret[1].position.x = half.x + tower_half.x - turret_half.x;
    state->turret[1].position.y = half.y + tower_half.y - turret_half.y;
    state->turret_count++;

    state->turret[2].draw = &turret_draw_component;
    state->turret[2].position.x = half.x - turret_half.x;
    state->turret[2].position.y = half.y - tower_half.y - turret_half.y;
    state->turret_count++;
    // TODO: missing boundary checks above
  }
}

SCENE_UPDATE(win_scene) { return; }

SCENE_UPDATE(lose_scene) { return; }

SCENE_UPDATE(credits_scene) { return; }

SCENE_UPDATE(exit_game) { return; }

extern GAME_INIT(GameInit)
{ 
  state = GameAllocateStruct(&memory, GameState);
  state->api = api;
  if(state->memory.ptr == 0) {
    state->memory = memory;
  }
  
  state->screen_w = screen_w;
  state->screen_h = screen_h;

  // state->api.PlatformSetBackgroundColor(0.545f, 0.525f, 0.298f, 1.0f);
  state->api.PlatformSetBackgroundColor(0.369f, 0.333f, 0.18f, 1.0f);

  // State machine for overall state
  scenes.StartingScene = starting_scene;
  scenes.ReadyScene = ready_scene;
  scenes.GoingScene = going_scene;
  scenes.WinScene = win_scene;
  scenes.LoseScene = lose_scene;
  scenes.CreditsScene = credits_scene;
  scenes.ExitGame = exit_game;
  state->scene = scenes.StartingScene;
}

extern GAME_UPDATE(GameUpdate)
{
  state->scene();
}

extern GAME_RENDER(GameRender)
{
  // Units
  for (int i=0; i<state->unit_count; i++){
    state->api.PlatformDrawBox(state->unit[i].position.x,
			       state->unit[i].position.y,
			       state->unit[i].draw->w,
			       state->unit[i].draw->h,
			       state->unit[i].draw->color.r,
			       state->unit[i].draw->color.g,
			       state->unit[i].draw->color.b,
			       state->unit[i].draw->color.a);
  }
  // Bullets
  for (int i=0; i<state->bullet_count; i++){
    state->api.PlatformDrawBox(state->bullet[i].position.x,
			       state->bullet[i].position.y,
			       state->bullet[i].draw->w,
			       state->bullet[i].draw->h,
			       state->bullet[i].draw->color.r,
			       state->bullet[i].draw->color.g,
			       state->bullet[i].draw->color.b,
			       state->bullet[i].draw->color.a);
  }
  // Turrets
  for (int i=0; i<state->turret_count; i++){
    state->api.PlatformDrawBox(state->turret[i].position.x,
			       state->turret[i].position.y,
			       state->turret[i].draw->w,
			       state->turret[i].draw->h,
			       state->turret[i].draw->color.r,
			       state->turret[i].draw->color.g,
			       state->turret[i].draw->color.b,
			       state->turret[i].draw->color.a);
  }
  // Towers
  for (int i=0; i<state->tower_count; i++){
    state->api.PlatformDrawBox(state->tower[i].position.x,
			       state->tower[i].position.y,
			       state->tower[i].draw->w,
			       state->tower[i].draw->h,
			       state->tower[i].draw->color.r,
			       state->tower[i].draw->color.g,
			       state->tower[i].draw->color.b,
			       state->tower[i].draw->color.a);
  }
}

extern GAME_KEYBOARD_INPUT(GameKeyboardInput)
{
}

extern GAME_CHANNEL_HALTED(GameAudioChannelHalted)
{
  printf("%d audio channel halted\n", channel);
}

extern GAME_WINDOW_RESIZED(GameWindowResized)
{
  printf("window(%d) resized", window);
  state->screen_w = width;
  state->screen_h = height;
}

extern GAME_QUIT(GameQuit)
{
}

