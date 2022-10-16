#ifndef MOVE_H
#define MOVE_H

#include <gba.h>
#include <math.h>
#include <vector.h>

typedef struct Movement {
  Vector position;
  Vector acceleration, gravity, friction;
  struct { Vector current, limit; } velocity;
} Movement;

static inline void
Movement_SetVelocityLimit(
    Movement *movement,
    int x, int y)
{
  movement->velocity.limit.x = Math_abs(x);
  movement->velocity.limit.y = Math_abs(y);
}

static inline void
Movement_SetVelocity(
    Movement *movement,
    int x, int y)
{
  movement->velocity.current.x = x;
  movement->velocity.current.y = y;
}

static inline void
Movement_SetAcceleration(
    Movement *movement,
    int x, int y)
{
  movement->acceleration.x = x;
  movement->acceleration.y = y;
}

static inline void
Movement_SetGravity(
    Movement *movement,
    int gravity)
{
  movement->gravity.x = 0;
  movement->gravity.y = Math_abs(gravity);
}

static inline void
Movement_SetFriction(
    Movement *movement,
    int friction)
{
  movement->friction.y = 0;
  movement->friction.x = Math_abs(friction);
}

static inline void
Movement_SetPosition(
    Movement *movement,
    int x, int y)
{
  movement->position.x = x;
  movement->position.y = y;
}

static inline Vector*
Movement_GetPosition(Movement *movement) {
  return &movement->position;
}

void
Movement_Update(Movement *movement);

/*
void
Movement_ForceStop(Movement *move);

void
Movement_DoJump(Movement *move);

void
Movement_EnableThrusters(
    Movement *move,
    bool enable);

typedef struct Free {
  int moveAcceleration;
  int jumpVelocity;
};

typedef struct Glide {
  Movement *move;
  int velocity, acceleration;
} Glide;

Glide_EnableThrusters(Glide *glide, Movement *move);
Glide_DisableThrusters(Glide *glide, Movement *target);

typedef struct Sprint {
  Movement *move;
  int velocity, jump;
} Sprint;

void
Sprint_DoJump(Sprint *sprint, Movement *target);
*/

#endif
