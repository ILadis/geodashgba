#ifndef CUBE_H
#define CUBE_H

#include <gba.h>
#include <math.h>
#include <vector.h>
#include <hit.h>

#include <game/camera.h>
#include <game/body.h>
#include <game/course.h>

typedef enum State {
  STATE_UNKNOWN,
  STATE_AIRBORNE,
  STATE_GROUNDED,
  STATE_DESTROYED,
} State;

typedef struct Cube {
  Body body;
  Bounds hitbox;
  struct { State current, previous; } state;
  struct { int angle, velocity; } rotation;
  int attempts, timer;
  Vector vertices[4];
  Shape shape;
  GBA_Sprite *sprite;
} Cube;

Cube*
Cube_GetInstance();

static inline const Vector*
Cube_GetPosition(Cube *cube) {
  return &cube->hitbox.center;
}

void
Cube_SetPosition(
    Cube *cube,
    const Vector *position);

static inline bool
Cube_EnteredState(Cube *cube, State state) {
  return cube->state.current == state && cube->state.previous != state;
}

static inline bool
Cube_InState(Cube *cube, State state) {
  return cube->state.current == state;
}

void
Cube_Jump(
    Cube *cube,
    int speed);

void
Cube_Launch(
    Cube *cube,
    int speed);

void
Cube_Accelerate(
    Cube *cube,
    Direction direction,
    int speed);

void
Cube_Update(
    Cube *cube,
    Course *course);

void
Cube_Draw(
    Cube *cube,
    Camera *camera);

#endif
