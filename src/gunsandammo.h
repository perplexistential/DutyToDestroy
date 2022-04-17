#ifndef GUNSANDAMMO_H
#define GUNSANDAMMO_H

#include "components.h"
#include "entity.h"
#include "coordinates.h"

typedef struct {
  float damage;
  DrawComponent *draw;
} AmmoKind;

typedef struct {
  Entity e;
  AmmoKind *ammo;
} Bullet;

#define MAX_PARTICLES 20

typedef struct {
  Bullet pool[MAX_PARTICLES];
  PhysicsComponent phys[MAX_PARTICLES];
} ClipComponent;


typedef struct {
  Polar *parent_rotation;
  Position *parent_position;
  Polar trajectory;
  float rate;
  float last_fire;
  int num_particles;
  Position position;
  ClipComponent *clip;
  Bullet *bullet;
} Gun;

#define EMIT_BULLET(n) Bullet* n(Gun* gun)
typedef EMIT_BULLET(ShootGun);

#define RECLAIM_BULLET(n) void n(Gun* gun, Bullet* bullet)
typedef RECLAIM_BULLET(ReclaimBullet);

#define UPDATE_BULLETS(n) void n(Gun *gun, float dt)
typedef UPDATE_BULLETS(UpdateBullets);

typedef struct Turret {
  Entity e;
  float health;
  float spread;
  Polar rotation;
  Gun gun;
  ShootGun *shoot;
  ReclaimBullet *reclaim;
  UpdateBullets *update;
} Turret;

#endif
