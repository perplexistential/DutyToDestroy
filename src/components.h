// Copyright 2022 of thatsamorais(thatsamorais@gmail.com)

#ifndef COMPONENTS_H
#define COMPONENTS_H

// Draw
typedef struct {
  float r, g, b, a;
} Color;

typedef struct DrawComponent {
  float w,h;
  Color color;
  float rotation;
} DrawComponent;

#endif
