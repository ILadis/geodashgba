
#include <game/trait.h>
#include <game/cube.h>

static inline int
RotateTrait_CalculateRotationVelocity(
    RotateTrait *trait,
    Course *course)
{
  Cube *cube = trait->cube;
  Cube shadow = (Cube) {
    .state  = cube->state,
  };

  MoveTrait move;
  MoveTrait_BindTo(&move, &shadow);

  HitTrait hit = {0};
  HitTrait_BindTo(&hit, &shadow);

  const Body *body = Cube_GetBody(cube);
  move.body = *body;

  int sign = -1
    * Math_signum(body->velocity.x)
    * Math_signum(body->dynamics->gravity.y);

  if (sign == 0) {
    return 0;
  }

  int frames = 0;
  do {
    frames += 4;

    Trait_Apply(&move.base, course);
    Trait_Apply(&move.base, course);
    Trait_Apply(&move.base, course);
    Trait_Apply(&move.base, course);

    Trait_Apply(&hit.base, course);
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

static void
RotateTrait_Apply(
    void *self,
    Course *course)
{
  RotateTrait *trait = self;
  Cube *cube = trait->cube;

  if (Cube_EnteredState(cube, STATE_GROUNDED)) {
    trait->velocity = 0;
    trait->angle = 64;
  }

  else if (Cube_EnteredState(cube, STATE_AIRBORNE)) {
    int velocity = RotateTrait_CalculateRotationVelocity(trait, course);
    trait->velocity = velocity;
    trait->angle = 64;
  }

  else {
    trait->angle += trait->velocity;
  }
}

Trait*
RotateTrait_BindTo(
    RotateTrait *rotation,
    Cube *cube)
{
  Trait *trait = &rotation->base;
  trait->self = rotation;
  trait->enabled = false;
  trait->type = TRAIT_TYPE_ROTATE;
  trait->Apply = RotateTrait_Apply;

  rotation->cube = cube;
  rotation->velocity = 0;
  rotation->angle = 64;

  Cube_AddTrait(cube, trait);

  return trait;
}
