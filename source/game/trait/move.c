
#include <game/trait.h>
#include <game/cube.h>

void
MoveTrait_Jump(
    MoveTrait *trait,
    int speed)
{
  Cube *cube = trait->cube;
  if (Cube_InState(cube, STATE_GROUNDED)) {
    const Dynamics *dynamics = trait->body.dynamics;
    int sy = Math_signum(dynamics->gravity.y);
    trait->launch = -1 * sy * speed;
  }
}

void
MoveTrait_Launch(
    MoveTrait *trait,
    int speed)
{
  const Dynamics *dynamics = trait->body.dynamics;
  int sy = Math_signum(dynamics->gravity.y);
  trait->launch = -1 * sy * speed;
}

void
MoveTrait_Accelerate(
    MoveTrait *trait,
    Direction direction,
    int speed)
{
  static const Dynamics dynamics = {
    .friction = { 60, 0 },
    .gravity  = { 0, 90 },
    .limits   = { 600, 3000 },
  };

  const Vector *vector = Vector_FromDirection(direction);

  int ax = speed * vector->x;
  int ay = speed * vector->y;

  Body *body = &trait->body;
  Body_SetAcceleration(body, ax, ay);
  Body_SetDynamics(body, &dynamics);
}

void
MoveTrait_HaltMovement(MoveTrait *trait) {
  Body *body = &trait->body;

  const Dynamics *dynamics = Dynamics_OfZero();
  Body_SetDynamics(body, dynamics);
  Body_SetVelocity(body, 0, 0);
  Body_SetAcceleration(body, 0, 0);
}

static void
MoveTrait_Apply(
    void *self,
    unused Course *course)
{
  MoveTrait *trait = self;
  Body *body = &trait->body;

  int launch = trait->launch;
  if (launch != 0) {
    body->velocity.y = launch;
    trait->launch = 0;
  }

  Body_Update(body);

  Cube *cube = trait->cube;
  if (Cube_EnteredState(cube, STATE_VICTORY)) {
    static const Dynamics dynamics = {
      .friction = { 100, 100 },
      .gravity  = { 0, 0 },
      .limits   = { 600, 3000 },
    };

    Body_SetAcceleration(body, 0, 0);
    Body_SetDynamics(body, &dynamics);
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

static void
MoveTrait_Action(void *self) {
  MoveTrait *trait = self;
  MoveTrait_Jump(trait, 1400);
}

Trait*
MoveTrait_BindTo(
    MoveTrait *move,
    Cube *cube,
    bool enabled)
{
  Trait *trait = &move->base;
  trait->self = move;
  trait->enabled = enabled;
  trait->type = TRAIT_TYPE_MOVE;
  trait->Apply = MoveTrait_Apply;
  trait->Action = MoveTrait_Action;

  move->cube = cube;
  move->launch = 0;
  move->body.position = Vector_Of(0, 0);
  move->body.velocity = Vector_Of(0, 0);
  move->body.acceleration = Vector_Of(0, 0);
  move->body.dynamics = Dynamics_OfZero();

  Cube_AddTrait(cube, trait);

  return trait;
}
