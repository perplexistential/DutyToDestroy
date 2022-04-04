#ifndef GAME_H
#define GAME_H

#include "entity.h"
#include "state.h"
#include "input.h"
#include "components.h"

typedef struct Level {
  int tower_count;
  int turret_count;
  struct Level *next;
} Level;

typedef struct {
  SceneUpdateFn *StartingScene;
  SceneUpdateFn *ReadyScene;
  SceneUpdateFn *GoingScene;
  SceneUpdateFn *WinScene;
  SceneUpdateFn *LoseScene;
  SceneUpdateFn *CreditsScene;
  SceneUpdateFn *ExitGame;
} Scenes;

typedef struct {
} Spawner;

typedef struct {
} Shooter;


#endif
