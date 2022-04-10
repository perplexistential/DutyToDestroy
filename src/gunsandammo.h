#ifndef GUNSANDAMMO_H
#define GUNSANDAMMO_H

#include "components.h"
#include "entity.h"

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
} ClipComponent;

typedef struct {
  float rate;
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

typedef struct {
  Entity e;
  float health;
  float spread;
  float rotation;
  Gun gun;
  ShootGun *shoot;
  ReclaimBullet *reclaim;
  UpdateBullets *update;
} Turret;

#endif
