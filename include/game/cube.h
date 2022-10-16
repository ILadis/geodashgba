#ifndef CUBE_H
#define CUBE_H

#include <gba.h>
#include <math.h>
#include <vector.h>
#include <hit.h>

#include <game/camera.h>
#include <game/move.h>

typedef struct Course Course;

typedef enum State {
  STATE_UNKNOWN,
  STATE_AIRBORNE,
  STATE_GROUNDED,
} State;

typedef struct Cube {
  Movement movement;
  Bounds hitbox;
  struct { State current, previous; } state;
  struct { int angle, velocity; } rotation;
  GBA_Sprite *sprite;
} Cube;

Cube*
Cube_GetInstance();

static inline Bounds*
Cube_GetHitbox(Cube *cube) {
  return &cube->hitbox;
}

static inline Vector*
Cube_GetPosition(Cube *cube) {
  return &cube->hitbox.center;
}

static inline bool
Cube_EnteredState(Cube *cube, State state) {
  return cube->state.current == state && cube->state.previous != state;
}

static inline bool
Cube_InState(Cube *cube, State state) {
  return cube->state.current == state;
}

static inline void
Cube_SetState(
    Cube *cube,
    State state)
{
  cube->state.previous = cube->state.current;
  cube->state.current = state;
}

void
Cube_Reset(
    Cube *cube,
    Vector *spawn);

void
Cube_Jump(Cube *cube);

void
Cube_Accelerate(
    Cube *cube,
    Direction direction,
    int speed);

void
Cube_Update(Cube *cube);

void
Cube_TakeHit(
    Cube *cube,
    Hit *hit);

void
Cube_HandleRotation(
    Cube *cube,
    Course *course);

void
Cube_Draw(
    Cube *cube,
    Camera *camera);

#endif
