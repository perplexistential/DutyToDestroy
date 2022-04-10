#ifndef STATE_H
#define STATE_H

#include "input.h"

#define SCENE_INIT(n) void n()
typedef SCENE_INIT(SceneInitFn);

#define SCENE_UPDATE(n) void n(float dt)
typedef SCENE_UPDATE(SceneUpdateFn);

#define SCENE_RENDER(n) void n()
typedef SCENE_RENDER(SceneRenderFn);

#define SCENE_INPUT(n) void n(InputEvent *event)
typedef SCENE_INPUT(SceneInputFn);

typedef struct Scene {
  SceneInitFn *init;
  SceneUpdateFn *update;
  SceneRenderFn *render;
  SceneInputFn *input;
} Scene;

#endif
