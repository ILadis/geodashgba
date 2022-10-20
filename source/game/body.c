
#include <game/body.h>

void
Body_Update(Body *body) {
  int vx = body->velocity.current.x + body->acceleration.x + body->gravity.x;
  int vy = body->velocity.current.y + body->acceleration.y + body->gravity.y;

  // apply friction
  if (vx < 0) {
    vx += Math_min(-vx, body->friction.x);
  } else if (vx > 0) {
    vx -= Math_min(+vx, body->friction.x);
  }

  int lx = body->velocity.limit.x;
  int ly = body->velocity.limit.y;

  // limit to max speed
  vx = Math_clamp(vx, -lx, +lx);
  vy = Math_clamp(vy, -ly, +ly);

  // set new position
  body->position.x += vx;
  body->position.y += vy;

  // set new velocity
  body->velocity.current.x = vx;
  body->velocity.current.y = vy;
}
