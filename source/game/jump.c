
#include <game/cube.h>
#include <game/course.h>

static inline int
Cube_CalculateRotationVelocity(
    Cube *cube,
    Course *course)
{
  Cube shadow = (Cube) {
    .movement = cube->movement,
    .hitbox = cube->hitbox,
    .state = cube->state,
  };

  // dont slow down vertical movement
  Movement_SetFriction(&shadow.movement, 0);
  int sign = Math_signum(shadow.movement.velocity.current.x);
  if (sign == 0) {
    return 0;
  }

  int frames = 0;
  do {
    frames++;

    Cube_Update(&shadow);
    Hit hit = Course_CheckHits(course, &shadow.hitbox);

    Cube_TakeHit(&shadow, &hit);
  } while (shadow.state.current != STATE_GROUNDED);

  // do a 1/4 rotation
  int alpha = 64;
  // do a 2/4 rotation
  if (frames >= 24) alpha = 128;
  // do a 3/4 rotation
  if (frames >= 40) alpha = 192;
  // do a 4/4 rotation
  if (frames >= 64) alpha = 256;

  int velocity = sign * Math_div(alpha, frames);

  return velocity;
}

void
Cube_HandleRotation(
    Cube *cube,
    Course *course)
{
  if (Cube_EnteredState(cube, STATE_GROUNDED)) {
    cube->rotation.velocity = 0;
    cube->rotation.angle = 64;
  }

  if (Cube_EnteredState(cube, STATE_AIRBORNE)) {
    int velocity = Cube_CalculateRotationVelocity(cube, course);
    cube->rotation.velocity = velocity;
    cube->rotation.angle = 64;
  }
}
