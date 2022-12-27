
#include <game/cube.h>

void
Cube_Jump(
    Cube *cube,
    int speed)
{
  if (Cube_InState(cube, STATE_GROUNDED)) {
    const Dynamics *dynamics = cube->body.dynamics;
    int sy = Math_signum(dynamics->gravity.y);
    cube->body.velocity.y = -1 * sy * speed;
  }
}

void
Cube_Accelerate(
    Cube *cube,
    Direction direction,
    int speed)
{
  const Vector *vector = Vector_FromDirection(direction);

  int ax = speed * vector->x;
  int ay = speed * vector->y;

  Body *body = &cube->body;
  Body_SetAcceleration(body, ax, ay);
}

void
Cube_ApplyMovement(Cube *cube) {
  Bounds *hitbox = &cube->hitbox;
  Body *body = &cube->body;

  Body_Update(body);
  Vector *position = Body_GetPosition(body);

  // revert 8w fixed-point integer
  hitbox->center.x = position->x >> 8;
  hitbox->center.y = position->y >> 8;

  cube->rotation.angle -= cube->rotation.velocity;
}
