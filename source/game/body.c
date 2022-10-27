
#include <game/body.h>

const Dynamics*
Dynamics_OfZero() {
  static const Dynamics dynamics = {0};
  return &dynamics;
}

void
Body_Update(Body *body) {
  const Dynamics *dynamics = body->dynamics;

  int vx = body->velocity.x + body->acceleration.x + dynamics->gravity.x;
  int vy = body->velocity.y + body->acceleration.y + dynamics->gravity.y;

  // apply friction
  if (vx < 0) {
    vx += Math_min(-vx, dynamics->friction);
  } else if (vx > 0) {
    vx -= Math_min(+vx, dynamics->friction);
  }

  int lx = dynamics->maxvel.x;
  int ly = dynamics->maxvel.y;

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
