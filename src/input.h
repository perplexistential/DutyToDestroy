#ifndef INPUT_H
#define INPUT_H

#include "entity.h"

typedef enum {
  INPUT_KIND_KEYBOARD,
  INPUT_KIND_CONTROLLER,
  INPUT_KIND_JOYSTICK, // Maybe later
} InputKind;

typedef struct {
  
} InputEvent;

#define INPUT_COMMAND(n) void n(Entity *e)
typedef INPUT_COMMAND(InputCommandFn);

#define HANDLE_INPUT(n) InputCommandFn* n()
typedef HANDLE_INPUT(HandleInputFn);

typedef struct {
  InputKind kind;
  HandleInputFn *handleInput;
} InputHandler;

typedef struct {
  InputHandler ih;
  int pressed_w;
  int pressed_s;
  int pressed_a;
  int pressed_d;
  int pressed_j;
  int pressed_i;
  int pressed_l;
  int pressed_up_arrow;
  int pressed_down_arrow;
  int pressed_left_arrow;
  int pressed_right_arrow;
  InputCommandFn *key_w;
  InputCommandFn *key_s;
  InputCommandFn *key_a;
  InputCommandFn *key_d;
  InputCommandFn *key_j;
  InputCommandFn *key_i;
  InputCommandFn *key_l;
  InputCommandFn *key_up_arrow;
  InputCommandFn *key_down_arrow;
  InputCommandFn *key_left_arrow;
  InputCommandFn *key_right_arrow;
  InputCommandFn *key_space;
  InputCommandFn *key_escape;
  // No need for mouse, yet
} KeyboardAndMouseInputHandler;

typedef struct {
  InputHandler ih;
  int pressed_up;
  int pressed_down;
  int pressed_left;
  int pressed_right;
  int pressed_b;
  int pressed_a;
  InputCommandFn *dpad_up;
  InputCommandFn *dpad_down;
  InputCommandFn *dpad_left;
  InputCommandFn *dpad_right;
  InputCommandFn *button_b;
  InputCommandFn *button_a;
  InputCommandFn *button_start;
  InputCommandFn *button_select;
} ControllerInputHandler;

#endif
