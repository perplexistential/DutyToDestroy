#ifndef GAME_H
#define GAME_H

#include "entity.h"
#include "input.h"
#include "state.h"
#include "components.h"
#include "gunsandammo.h"

typedef struct Level {
  int tower_count;
  int turret_count;
  struct Level *next;
} Level;

typedef struct {
  Scene StartingScene;
  Scene ReadyScene;
  Scene GoingScene;
  Scene WinScene;
  Scene LoseScene;
  Scene CreditsScene;
  Scene ExitGame;
} Scenes;

typedef struct {
} Spawner;

typedef struct {
} Shooter;


#endif
