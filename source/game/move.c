
#include <game/move.h>

void
Movement_Update(Movement *movement) {
  int vx = movement->velocity.current.x + movement->acceleration.x + movement->gravity.x;
  int vy = movement->velocity.current.y + movement->acceleration.y + movement->gravity.y;

  // apply friction
  if (vx < 0) {
    vx += Math_min(-vx, movement->friction.x);
  } else if (vx > 0) {
    vx -= Math_min(+vx, movement->friction.x);
  }

  int lx = movement->velocity.limit.x;
  int ly = movement->velocity.limit.y;

  // limit to max speed
  vx = Math_clamp(vx, -lx, +lx);
  vy = Math_clamp(vy, -ly, +ly);

  // set new position
  movement->position.x += vx;
  movement->position.y += vy;

  // set new velocity
  movement->velocity.current.x = vx;
  movement->velocity.current.y = vy;
}
