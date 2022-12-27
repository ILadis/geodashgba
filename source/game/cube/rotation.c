
#include <game/cube.h>

static inline int
Cube_CalculateRotationVelocity(
    Cube *cube,
    Course *course)
{
  extern void Cube_ApplyMovement(Cube *cube);
  extern void Cube_ApplyHit(Cube *cube, Course *course);

  Cube shadow = (Cube) {
    .body   = cube->body,
    .hitbox = cube->hitbox,
    .state  = cube->state,
  };

  int sign = Math_signum(shadow.body.velocity.x);
  if (sign == 0) {
    return 0;
  }

  int frames = 0;
  do {
    frames++;

    Cube_ApplyMovement(&shadow);
    Cube_ApplyHit(&shadow, course);
  } while (Cube_InState(&shadow, STATE_AIRBORNE));

  // do a 1/4 rotation
  int alpha = 64;
  // do a 4/4 rotation
  if (frames >= 64) alpha = 256;
  // do a 3/4 rotation
  else if (frames >= 40) alpha = 192;
  // do a 2/4 rotation
  else if (frames >= 24) alpha = 128;

  int velocity = sign * Math_div(alpha, frames);

  return velocity;
}

void
Cube_ApplyRotation(
    Cube *cube,
    Course *course)
{
  if (Cube_EnteredState(cube, STATE_GROUNDED)) {
    cube->rotation.velocity = 0;
    cube->rotation.angle = 64;
  }

  else if (Cube_EnteredState(cube, STATE_AIRBORNE)) {
    int velocity = Cube_CalculateRotationVelocity(cube, course);
    cube->rotation.velocity = velocity;
    cube->rotation.angle = 64;
  }
}
