
#include <game/body.h>

const Dynamics*
Dynamics_OfZero() {
  static const Dynamics dynamics = {0};
  return &dynamics;
}

void
Body_Update(Body *body) {
  const Dynamics *dynamics = body->dynamics;

  Vector gravity = dynamics->gravity;
  if (dynamics->center != NULL) {
    // both body position and gravity center must use a 24.8 fixed-point integer
    int dx = Math_floor(dynamics->center->x - body->position.x, 8);
    int dy = Math_floor(dynamics->center->y - body->position.y, 8);

    int angle = Math_floor(Math_atan2(dx, dy) * 256, 16);

    gravity.x = Math_floor(Math_cos(angle) * gravity.x, 8);
    gravity.y = Math_floor(Math_sin(angle) * gravity.y, 8);
  }

  int vx = body->velocity.x + body->acceleration.x + gravity.x;
  int vy = body->velocity.y + body->acceleration.y + gravity.y;

  // apply friction
  if (vx < 0) {
    vx += Math_min(-vx, dynamics->friction.x);
  } else if (vx > 0) {
    vx -= Math_min(+vx, dynamics->friction.x);
  }

  if (vy < 0) {
    vy += Math_min(-vy, dynamics->friction.y);
  } else if (vy > 0) {
    vy -= Math_min(+vy, dynamics->friction.y);
  }

  int lx = dynamics->limits.x;
  int ly = dynamics->limits.y;

  // limit to max speed
  vx = Math_clamp(vx, -lx, +lx);
  vy = Math_clamp(vy, -ly, +ly);

  // set new position
  body->position.x += vx;
  body->position.y += vy;

  // set new velocity
  body->velocity.x = vx;
  body->velocity.y = vy;
}
