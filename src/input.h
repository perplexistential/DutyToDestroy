#ifndef INPUT_H
#define INPUT_H

#include "entity.h"

#define INPUT_COMMAND(n) void n(Entity *e)
typedef INPUT_COMMAND(InputCommandFn);

#define HANDLE_INPUT(n) InputCommandFn* n()
typedef HANDLE_INPUT(HandleInputFn);

typedef enum {
  INPUT_KIND_KEYBOARD,
  INPUT_KIND_CONTROLLER,
  INPUT_KIND_JOYSTICK, // Maybe later
} InputKind;

typedef struct {
  InputKind kind;
  HandleInputFn *handleInput;
} InputHandler;

typedef struct {
  InputHandler ih;
  InputCommandFn *key_w;
  InputCommandFn *key_s;
  InputCommandFn *key_a;
  InputCommandFn *key_d;
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
