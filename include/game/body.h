#ifndef BODY_H
#define BODY_H

#include <gba.h>
#include <math.h>
#include <vector.h>

typedef struct Dynamics {
  Vector friction;
  Vector gravity;
  Vector limits;
} Dynamics;

typedef struct Body {
  const Dynamics *dynamics;
  Vector position;
  Vector acceleration, velocity;
} Body;

const Dynamics*
Dynamics_OfZero();

static inline Dynamics
Dynamics_OfInverseGravity(const Dynamics *dynamics) {
  return (Dynamics) {
    .friction = dynamics->friction,
    .limits = dynamics->limits,
    .gravity = Vector_Of(-dynamics->gravity.x, -dynamics->gravity.y)
  };
}

static inline void
Body_SetDynamics(
    Body *body,
    const Dynamics *dynamics)
{
  body->dynamics = dynamics;
}

static inline void
Body_SetVelocity(
    Body *body,
    int x, int y)
{
  body->velocity.x = x;
  body->velocity.y = y;
}

static inline void
Body_SetAcceleration(
    Body *body,
    int x, int y)
{
  body->acceleration.x = x;
  body->acceleration.y = y;
}

static inline void
Body_SetPosition(
    Body *body,
    int x, int y)
{
  body->position.x = x;
  body->position.y = y;
}

static inline Vector*
Body_GetPosition(Body *body) {
  return &body->position;
}

void
Body_Update(Body *body);

#endif
