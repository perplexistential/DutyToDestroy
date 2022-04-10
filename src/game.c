#include <bits/types/struct_timeval.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "components.h"
#include "entity.h"
#include "gunsandammo.h"
#include "input.h"
#include "shared.h"
#include "game.h"
#include "state.h"

#define MAX_TOWERS 1
#define MAX_TURRETS 3
#define MAX_BULLETS 10
#define MAX_UNITS 10

typedef struct
{
  GameMemory memory;
  PlatformAPI api;

  // Window meta
  int screen_w, screen_h;

  bool init_only_once;
  bool init_done;

  Scenes scenes;
  Scene *scene;

  // Entities
  Entity tower;
  int turret_count;
  Turret turret[MAX_TURRETS];
  ClipComponent clip[MAX_TURRETS];
  DrawComponent tower_draw_component;
  DrawComponent turret_draw_component;
  DrawComponent bullet_draw_component;
  PhysicsComponent bullet_phys_component;
  AmmoKind ammo0;
  Bullet bullet0;
  DrawComponent unit_draw_component;
  

  KeyboardAndMouseInputHandler kbm_input_handler;
  ControllerInputHandler controller_input_handler;

  bool game_paused;
  unsigned int current_turret;
} GameState;

static GameState *state;

static DrawComponent tower_draw_component = {
    .dim = {.x = 50, .y = 50},
    .color = {.r = 0.886f, .g = 0.843f, .b = 0.773f, .a = 1.0f},
    .rotation = {.x = 0, .y = 0},
};

static DrawComponent turret_draw_component = {
    .dim = {.x = 25, .y = 25},
    .color = {.r = 0.671f, .g = 0.486f, .b = 0.322f, .a = 1.0f},
    .rotation = {.x = 0, .y = 0},
};

static DrawComponent bullet_draw_component = {
    .dim = {.x = 5, .y = 5},
    .color = {.r = 0.663f, .g = 0.682f, .b = 0.471f, .a = 1.0f},
    .rotation = {.x = 0, .y = 0},
};

static PhysicsComponent bullet_phys_component = {
    .accel = {0},
    .veloc = {1.0f},
};

/*
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

INPUT_COMMAND(prev_turret)
{
  if (state->current_turret == 0)
    state->current_turret = state->turret_count;
  else
    state->current_turret--;
}

INPUT_COMMAND(rotate_left) {
  Turret *t = (Turret *)e;
  t->rotation--;
  t->rotation = MAX(t->rotation, 0.0f);
}

INPUT_COMMAND(rotate_right) {
  Turret *t = (Turret *)e;
  t->rotation++;
  t->rotation = MIN(t->rotation, 360.0f);
}

INPUT_COMMAND(increase_spread) {
  // TODO allow spreading fire
}

INPUT_COMMAND(decrease_spread) {
  // TODO allow spreading fire
}

INPUT_COMMAND(null_command) {}


HANDLE_INPUT(keyboard_and_mouse_handler) {  
  return &null_command;
}

HANDLE_INPUT(controller_handler) {
  return &null_command;
}

static const InputHandler ih_kbm = {.kind = INPUT_KIND_KEYBOARD,
                              .handleInput = keyboard_and_mouse_handler};
static const InputHandler ih_cntlr = {.kind = INPUT_KIND_CONTROLLER,
                                .handleInput = controller_handler};


static const KeyboardAndMouseInputHandler kbm_input_handler = {
    .key_w = &increase_spread,
    .key_s = &decrease_spread,
    .key_a = &rotate_left,
    .key_d = &rotate_right,
    .key_j = &prev_turret,
    .key_i = &next_turret,
    .key_l = &next_turret,
    .key_up_arrow = &prev_turret,
    .key_down_arrow = &next_turret,
    .key_left_arrow = &prev_turret,
    .key_right_arrow = &next_turret,
    .key_escape = &pause_game,
};

static const ControllerInputHandler controller_input_handler = {
    .dpad_up = &increase_spread,
    .dpad_down = &decrease_spread,
    .dpad_left = &rotate_left,
    .dpad_right = &rotate_right,
    .button_a = &prev_turret,
    .button_b = &next_turret,
    .button_start = &pause_game,
};

typedef struct {
  Position position;
  Vec2D *dim;
} BoundingBox;

bool checkCollision(BoundingBox *a, BoundingBox *b){
  // the sides of the rects
  int leftA, leftB;
  int rightA, rightB;
  int topA, topB;
  int bottomA, bottomB;
  
  // Calculate the sides of "A"
  leftA = a->position.x;
  rightA = a->position.x + a->dim->x;
  topA = a->position.y;
  bottomA = a->position.y + a->dim->y;

  // Calculate the sides of "B"
  leftB = b->position.x;
  rightB = b->position.x + b->dim->x;
  topB = b->position.y;
  bottomB = b->position.y + b->dim->y;

  // If any of the sides from A are outside of Bool
  if (bottomA <= topB) {
    return false;
  }
  if (topA >= bottomB) {
    return false;
  }
  if (rightA <= leftB) {
    return false;
  }
  if (leftA >= rightB) {
    return false;
  }
  // Collision!
  return true;
}

float speed(float accel, float dt, float velocity)
{
  return accel * (dt * velocity);
}

EMIT_BULLET(shoot_bullet) {
  if (gun->num_particles == MAX_BULLETS)
    return NULL;
  Bullet *next = &gun->clip->pool[gun->num_particles];
  memcpy(&next->e.position, &gun->position, sizeof(Position));
  next->e.draw = &state->bullet_draw_component;
  next->ammo = gun->bullet->ammo;
  gun->num_particles++;
  return next;
}

RECLAIM_BULLET(reclaim_bullet) {
  // TODO: detect a bullet outside of the memory range
  memmove(bullet, &gun->clip->pool[gun->num_particles - 1], sizeof(Bullet));
  gun->num_particles--;
}

UPDATE_BULLETS(update_bullets) {
  for (int i=0; i<gun->num_particles; i++) {
    Bullet *b = &gun->clip->pool[i];
    BoundingBox bb_next_x = {
      .position = {
	.x=b->e.position.x + speed(b->e.phys->accel.x, dt, b->e.phys->veloc.x),
	.y=b->e.position.y,
      },
      .dim =&b->e.draw->dim,
    };
    BoundingBox bb_next_y = {
      .position = {
	.x=b->e.position.x,
	.y=b->e.position.y + speed(b->e.phys->accel.y, dt, b->e.phys->veloc.y),
      },
      .dim=&b->e.draw->dim,
    };
    if ((bb_next_x.position.x < state->screen_w/-2.0f) ||
	(bb_next_x.position.x + bb_next_x.dim->x > state->screen_w/2.0f)) {
      reclaim_bullet(gun, b);
    } 
    if ((bb_next_y.position.y < state->screen_h/-2.0f) ||
	(bb_next_y.position.y + bb_next_y.dim->y > state->screen_h/2.0f)) {
      reclaim_bullet(gun, b);
    } 
    b->e.position.x += speed(b->e.phys->accel.x, dt, b->e.phys->veloc.x);
    b->e.position.y += speed(b->e.phys->accel.y, dt, b->e.phys->veloc.y);
    // calculate next position
  }
}

void init_turret(Turret *t, Position *pos)
{
  Entity *e = (Entity *)t;
  e->kind = TURRET;
  e->draw = &state->turret_draw_component;
  e->position.x = pos->x;
  e->position.y = pos->y;
  t->gun.bullet = &state->bullet0;
  t->gun.num_particles = 0;
  t->gun.rate = 0.25f;
  t->gun.clip = &state->clip[state->turret_count];
  t = &state->turret[state->turret_count];
  state->turret_count++;
}

// Scenes
SCENE_INIT(null_scene_init) {}
SCENE_UPDATE(null_scene_update) { state->scene = &state->scenes.StartingScene; }
SCENE_INPUT(null_scene_input) {}
SCENE_RENDER(null_scene_render) {}

SCENE_UPDATE(starting_scene) { state->scene = &state->scenes.GoingScene; }

SCENE_UPDATE(ready_scene) { return; }

SCENE_UPDATE(going_scene) {
  Position tower_half;
  Position pos;
  Entity *t = NULL;
  
  if (!state->init_done) {
    state->init_done = 1;
    state->turret_count = 0;
    
    // Place tower
    t = (Entity *)&state->tower;
    t->draw = &state->tower_draw_component;
    tower_half.x = t->draw->dim.x/2.0f;
    tower_half.y = t->draw->dim.y/2.0f;
    t->position.x = -tower_half.x;
    t->position.y = -tower_half.y;
    
    // Place turrets
    /*
    Position turret_half;
    turret_half.x = state->turret_draw_component.dim.x/2.0f;
    turret_half.y = state->turret_draw_component.dim.x/2.0f;
    */
    pos.x = -tower_half.x;
    pos.y = tower_half.y;
    init_turret(&state->turret[0], &pos);
    pos.x = tower_half.x;
    pos.y = tower_half.y;
    init_turret(&state->turret[1], &pos);
    pos.x = 0;
    pos.y = -tower_half.y;
    init_turret(&state->turret[2], &pos);
  }
}

SCENE_INPUT(going_scene_input) {
  // TODO
}

SCENE_RENDER(going_scene_render) {
  
  Entity *e = NULL;
  // Units
  /*
  for (int i=0; i<state->unit_count; i++){
    state->api.PlatformDrawBox(state->unit[i].position.x,
			       state->unit[i].position.y,
			       state->unit[i].draw->dim.x,
			       state->unit[i].draw->dim.y,
			       state->unit[i].draw->color.r,
			       state->unit[i].draw->color.g,
			       state->unit[i].draw->color.b,
			       state->unit[i].draw->color.a,
			       0);
  }
  */
  // Bullets
  for (int i=0; i < state->turret_count; i++){
    for (int b=0; b < state->turret[i].gun.num_particles; b++) {
      state->api.PlatformDrawBox(state->clip[i].pool[b].e.position.x,
				 state->clip[i].pool[b].e.position.y,
				 state->clip[i].pool[b].e.draw->dim.x,
				 state->clip[i].pool[b].e.draw->dim.y,
				 state->clip[i].pool[b].e.draw->color.r,
				 state->clip[i].pool[b].e.draw->color.g,
				 state->clip[i].pool[b].e.draw->color.b,
				 state->clip[i].pool[b].e.draw->color.a,
				 0);
    }
  }
  // Turrets
  for (int i=0; i<state->turret_count; i++){
    e = ((Entity *)&state->turret[i]);
    state->api.PlatformDrawBox(e->position.x,
			       e->position.y,
			       e->draw->dim.x,
			       e->draw->dim.y,
			       e->draw->color.r,
			       e->draw->color.g,
			       e->draw->color.b,
			       e->draw->color.a,
			       state->turret[i].rotation);
  }
  // Towers
  /*
  for (int i=0; i<state->tower_count; i++){
    state->api.PlatformDrawBox(state->tower[i].position.x,
			       state->tower[i].position.y,
			       state->tower[i].draw->dim.x,
			       state->tower[i].draw->dim.y,
			       state->tower[i].draw->color.r,
			       state->tower[i].draw->color.g,
			       state->tower[i].draw->color.b,
			       state->tower[i].draw->color.a,
			       0);
  }
  */
  state->api.PlatformDrawBox(state->tower.position.x,
			     state->tower.position.y,
			     state->tower.draw->dim.x,
			     state->tower.draw->dim.y,
			     state->tower.draw->color.r,
			     state->tower.draw->color.g,
			     state->tower.draw->color.b,
			     state->tower.draw->color.a,
			     0);
}

SCENE_UPDATE(win_scene) {
  state->scene = &state->scenes.GoingScene;
}

SCENE_UPDATE(lose_scene) {
  state->scene = &state->scenes.GoingScene;
}

SCENE_UPDATE(credits_scene) { return; }

SCENE_UPDATE(exit_game) { state->api.PlatformQuit(); }

static const Scene NULL_SCENE = {.init = null_scene_init,
                                 .update = null_scene_update,
                                 .input = null_scene_input,
                                 .render = null_scene_render};

static const Scene STARTING_SCENE = {.init = null_scene_init,
                                     .input = null_scene_input,
                                     .render = null_scene_render,
                                     .update = starting_scene};

static const Scene GOING_SCENE = {.init = null_scene_init,
                                  .input = going_scene_input,
                                  .update = going_scene,
                                  .render = going_scene_render};

extern GAME_INIT(GameInit)
{ 
  state = GameAllocateStruct(&memory, GameState);
  state->api = api;
  if(state->memory.ptr == 0) {
    state->memory = memory;
  }
  
  state->screen_w = screen_w;
  state->screen_h = screen_h;

  state->ammo0.damage = 1;
  state->ammo0.draw=&state->bullet_draw_component;
  state->bullet0.ammo=&state->ammo0;
  memcpy(&state->tower_draw_component, &tower_draw_component, sizeof(tower_draw_component));
  memcpy(&state->turret_draw_component, &turret_draw_component, sizeof(turret_draw_component));
  memcpy(&state->bullet_draw_component, &bullet_draw_component, sizeof(bullet_draw_component));
  memcpy(&state->bullet_phys_component, &bullet_phys_component, sizeof(bullet_phys_component));

  // state->api.PlatformSetBackgroundColor(0.545f, 0.525f, 0.298f, 1.0f);
  state->api.PlatformSetBackgroundColor(0.369f, 0.333f, 0.18f, 1.0f);

  state->init_done = 0;

  memcpy(&state->scenes.StartingScene, &STARTING_SCENE, sizeof(STARTING_SCENE));
  memcpy(&state->scenes.GoingScene, &GOING_SCENE, sizeof(GOING_SCENE));
  memcpy(&state->kbm_input_handler, &kbm_input_handler, sizeof(kbm_input_handler));
  memcpy(&state->kbm_input_handler.ih, &ih_kbm, sizeof(InputHandler));
  memcpy(&state->controller_input_handler, &controller_input_handler, sizeof(controller_input_handler));
  memcpy(&state->controller_input_handler.ih, &ih_cntlr, sizeof(InputHandler));

  memcpy(&state->scenes.StartingScene, &STARTING_SCENE, sizeof(Scene));
  memcpy(&state->scenes.ReadyScene, &NULL_SCENE, sizeof(Scene));
  memcpy(&state->scenes.GoingScene, &GOING_SCENE, sizeof(Scene));
  memcpy(&state->scenes.WinScene, &NULL_SCENE, sizeof(Scene));
  memcpy(&state->scenes.LoseScene, &NULL_SCENE, sizeof(Scene));
  memcpy(&state->scenes.CreditsScene, &NULL_SCENE, sizeof(Scene));
  state->scene = &state->scenes.StartingScene;
}

extern GAME_UPDATE(GameUpdate)
{
  state->scene->update(dt);
}

extern GAME_RENDER(GameRender)
{
  state->scene->render();
}

extern GAME_KEYBOARD_INPUT(GameKeyboardInput)
{
  Entity *e = (Entity *)&state->turret[state->current_turret];
  // TODO: actually delegate using an event object or something
  // TODO: The input commands could be reified and executed later
  state->kbm_input_handler.ih.handleInput();
  if (key_state == BUTTON_RELEASED)
    return;
  switch(symbol) {
  case SCANCODE_W:
    state->kbm_input_handler.key_w(e);
    break;
  case SCANCODE_S:
    state->kbm_input_handler.key_s(e);
    break;
  case SCANCODE_A:
    state->kbm_input_handler.key_a(e);
    break;
  case SCANCODE_D:
    state->kbm_input_handler.key_d(e);
    break;
  case SCANCODE_J:
    state->kbm_input_handler.key_j(e);
    break;
  case SCANCODE_I:
    state->kbm_input_handler.key_i(e);
    break;
  case SCANCODE_L:
    state->kbm_input_handler.key_l(e);
    break;
  case SCANCODE_UP:
    state->kbm_input_handler.key_up_arrow(e);
    break;
  case SCANCODE_DOWN:
    state->kbm_input_handler.key_down_arrow(e);
    break;
  case SCANCODE_LEFT:
    state->kbm_input_handler.key_left_arrow(e);
    break;
  case SCANCODE_RIGHT:
    state->kbm_input_handler.key_right_arrow(e);
    break;
  case SCANCODE_ESCAPE:
    state->kbm_input_handler.key_escape(e);
    break;
  default:
    break;
  }
}

extern GAME_CONTROLLER_BUTTON_EVENT(GameControllerButtonInput)
{
  Entity *e = (Entity *)&state->turret[state->current_turret];
  state->controller_input_handler.ih.handleInput();
  if (button_state == BUTTON_RELEASED)
    return;

  switch(button){
  case CONTROLLER_BUTTON_DPAD_UP:
    state->controller_input_handler.dpad_up(e);
    break;
  case CONTROLLER_BUTTON_DPAD_DOWN:
    state->controller_input_handler.dpad_down(e);
    break;
  case CONTROLLER_BUTTON_DPAD_LEFT:
    state->controller_input_handler.dpad_left(e);
    break;
  case CONTROLLER_BUTTON_DPAD_RIGHT:
    state->controller_input_handler.dpad_right(e);
    break;
  case CONTROLLER_BUTTON_B:
    state->controller_input_handler.button_b(e);
    break;
  case CONTROLLER_BUTTON_A:
    state->controller_input_handler.button_a(e);
    break;
  case CONTROLLER_BUTTON_START:
    state->controller_input_handler.button_start(e);
    break;
  default:
    break;
  }
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

