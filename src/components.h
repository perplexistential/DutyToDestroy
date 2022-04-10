// Copyright 2022 of thatsamorais(thatsamorais@gmail.com)

#ifndef COMPONENTS_H
#define COMPONENTS_H

// Draw

typedef struct {
  float x, y;
} Vec2D;

typedef struct {
  float r, g, b, a;
} Color;

typedef struct {
  Vec2D dim;
  Color color;
  Vec2D rotation;
} DrawComponent;

typedef struct {
  Vec2D veloc;
  Vec2D accel;
} PhysicsComponent;

#endif
