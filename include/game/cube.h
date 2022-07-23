#ifndef CUBE_H
#define CUBE_H

#include <gba.h>
#include <math.h>
#include <vector.h>
#include <hit.h>

#include <game/camera.h>

typedef enum State {
  STATE_UNKNOWN,
  STATE_JUMPING,
  STATE_GROUNDED,
} State;

typedef struct Cube {
  Vector position;
  Vector velocity, acceleration;
  Vector gravity, friction;
  Bounds hitbox;
  State state;
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

void
Cube_Reset(Cube *cube);

void
Cube_Jump(Cube *cube);

void
Cube_Accelerate(
    Cube *cube,
    Direction direction);

void
Cube_Update(Cube *cube);

void
Cube_TakeHit(
    Cube *cube,
    Hit *hit);

void
Cube_Draw(
    Cube *cube,
    Camera *camera);

#endif
