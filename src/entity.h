// Copyright 2022 of thatsamorais(thatsamorais@gmail.com)

#ifndef ENTITY_H
#define ENTITY_H

#include "components.h"

#define masker(n) (1u << n)

typedef enum {
  TOWER = masker(0),
  TURRET = masker(1),
  BULLET = masker(2),
  UNIT = masker(3),
  PICKUP = masker(4),
} EntityKind;

typedef struct {
  float x, y;
} Position;

typedef struct {
  EntityKind kind;

  // TODO: object pool etc with pointers?
  Position position;
  DrawComponent *draw;
} Entity;

#endif
