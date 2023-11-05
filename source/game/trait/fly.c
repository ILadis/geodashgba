
#include <game/trait.h>
#include <game/cube.h>

static void
FlyTrait_Enabled(
    void *self,
    bool enabled)
{
  FlyTrait *trait = self;
  Body *body = trait->body;

  static const Dynamics dynamics = {
    .friction = { 60, 0 },
    .gravity  = { 0, 40 },
    .limits   = { 600, 500 },
  };

  Cube *cube = trait->cube;
  Camera *camera = Camera_GetInstance();

  if (enabled) {
    trait->acceleration = body->acceleration.x;
    trait->restore.dynamics = body->dynamics;
    trait->restore.limits = camera->limit.upper;

    Body_SetDynamics(body, &dynamics);
    GBA_Sprite_SetTileId(cube->sprite, 18);
  } else {
    camera->limit.lower = Vector_Of(0, 0);
    camera->limit.upper = trait->restore.limits;

    Body_SetDynamics(body, trait->restore.dynamics);
    GBA_Sprite_SetTileId(cube->sprite, 0);
  }
}

static inline void
FlyTrait_MoveCameraIntoFrame(
    unused FlyTrait *trait,
    Course *course)
{
  const Bounds *bounds = Course_GetBounds(course);
  Vector upper = Bounds_Upper(bounds);

  Camera *camera = Camera_GetInstance();

  int current = camera->position.y;
  int target = upper.y - camera->viewport.size.y*2 - 15;

  int dy = Math_clamp(target - current, -3, +3);

  camera->limit.lower.y = current + dy;
  camera->limit.upper.y = current + dy;
}

static void
FlyTrait_Apply(
    void *self,
    Course *course)
{
  FlyTrait *trait = self;
  FlyTrait_MoveCameraIntoFrame(trait, course);

  Body *body = trait->body;
  Cube *cube = trait->cube;
  if (!Cube_InState(cube, STATE_DESTROYED)) {
    Body_Update(body);
    Body_SetAcceleration(body, trait->acceleration, 0);
  }

  RotateTrait *rotate = Cube_GetTrait(cube, TRAIT_TYPE_ROTATE);
  if (Cube_InState(cube, STATE_GROUNDED)) {
    rotate->angle = 64;
  } else {
    int dx = (body->velocity.x << 1);
    int dy = (-1 * body->velocity.y);

    int angle = Math_floor(Math_atan2(dx, dy) * 256, 16);
    rotate->angle = 64 + angle;
  }
}

static void
FlyTrait_Action(void *self) {
  FlyTrait *trait = self;
  Body *body = trait->body;

  const Dynamics *dynamics = body->dynamics;

  int gravity = dynamics->gravity.y;
  int sy = -1 * Math_signum(dynamics->gravity.y);
  int ay = sy * gravity;

  Body_SetAcceleration(body, trait->acceleration, ay * 2);

  Cube *cube = trait->cube;
  if (Cube_InState(cube, STATE_GROUNDED)) {
    body->velocity.y = ay;
  }
}

Trait*
FlyTrait_BindTo(
    FlyTrait *fly,
    Cube *cube,
    bool enabled)
{
  Trait *trait = &fly->base;
  trait->self = fly;
  trait->enabled = enabled;
  trait->type = TRAIT_TYPE_FLY;
  trait->Enabled = FlyTrait_Enabled;
  trait->Apply = FlyTrait_Apply;
  trait->Action = FlyTrait_Action;

  MoveTrait *move = Cube_GetTrait(cube, TRAIT_TYPE_MOVE);

  fly->cube = cube;
  fly->body = &move->body;
  fly->acceleration = 0;
  fly->restore.dynamics = NULL;
  fly->restore.limits = Vector_Of(0, 0);

  Cube_AddTrait(cube, trait);

  return trait;
}
