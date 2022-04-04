#ifndef GUNSANDAMMO_H
#define GUNSANDAMMO_H

#include "components.h"

typedef struct {
} BulletAlive;

typedef struct {
} BulletHit;

typedef struct {
} BulletState;

typedef struct {
} Ammo;

typedef struct { 
 BulletState *state;
} Bullet;

typedef struct {
  Bullet *ammo;
} TurretState;


typedef struct {
  TurretState *state;
  float health;
  float spread;
  Bullet *bullets;

  DrawComponent draw;
  PhysicsComponent physics;
} Turret;

#endif
