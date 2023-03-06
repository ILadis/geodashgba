
#include <game/cube.h>

void
Cube_Jump(
    Cube *cube,
    int speed)
{
  if (Cube_InState(cube, STATE_GROUNDED)) {
    const Dynamics *dynamics = cube->body.dynamics;
    int sy = Math_signum(dynamics->gravity.y);
    cube->launch = -1 * sy * speed;
  }
}

void
Cube_Launch(
    Cube *cube,
    int speed)
{
  const Dynamics *dynamics = cube->body.dynamics;
  int sy = Math_signum(dynamics->gravity.y);
  cube->launch = -1 * sy * speed;
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

  int launch = cube->launch;
  if (launch != 0) {
    body->velocity.y = launch;
    cube->launch = 0;
  }

  Body_Update(body);
  Vector *position = Body_GetPosition(body);

  // revert 8w fixed-point integer
  hitbox->center.x = position->x >> 8;
  hitbox->center.y = position->y >> 8;

  cube->rotation.angle -= cube->rotation.velocity;

  if (Cube_EnteredState(cube, STATE_VICTORY)) {
    static const Dynamics dynamics = {
      .friction = { 100, 100 },
      .gravity  = { 0, 0 },
      .limits   = { 600, 3000 },
    };

    Body_SetDynamics(body, &dynamics);
    Cube_Accelerate(cube, DIRECTION_RIGHT, 0);
  }

  else if (Cube_InState(cube, STATE_VICTORY) && !Cube_IsMoving(cube)) {
    static const Dynamics dynamics = {
      .friction = { 0, 0 },
      .gravity  = { 200, 45 },
      .limits   = { 800, 800 },
    };

    Body_SetDynamics(body, &dynamics);
    Body_SetVelocity(body, -150, -800);
  }
}
