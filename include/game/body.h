#ifndef BODY_H
#define BODY_H

#include <gba.h>
#include <math.h>
#include <vector.h>

typedef struct Body {
  Vector position;
  Vector acceleration, gravity, friction;
  struct { Vector current, limit; } velocity;
} Body;

static inline void
Body_SetVelocityLimit(
    Body *body,
    int x, int y)
{
  body->velocity.limit.x = Math_abs(x);
  body->velocity.limit.y = Math_abs(y);
}

static inline void
Body_SetVelocity(
    Body *body,
    int x, int y)
{
  body->velocity.current.x = x;
  body->velocity.current.y = y;
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
Body_SetGravity(
    Body *body,
    int gravity)
{
  body->gravity.x = 0;
  body->gravity.y = Math_abs(gravity);
}

static inline void
Body_SetFriction(
    Body *body,
    int friction)
{
  body->friction.y = 0;
  body->friction.x = Math_abs(friction);
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
