
#include <game/trait.h>
#include <game/cube.h>

static inline void
HitTrait_ResolveHit(HitTrait *trait, int dy) {
  Vector *center = &trait->hitbox.center;
  center->y += dy;

  Cube *cube = trait->cube;
  Cube_Launch(cube, -1);
  Cube_SetPosition(cube, center);
}

static inline Raycast
HitTrait_RaycasFromMovement(HitTrait *trait) {
  Cube *cube = trait->cube;
  const Body *body = Cube_GetBody(cube);

  int vx = body->velocity.x >> 8;
  int vy = body->velocity.y >> 8;

  int px = body->position.x >> 8;
  int py = body->position.y >> 8;

  int gravity = Math_signum(body->dynamics->gravity.y);
  if (Cube_InState(cube, STATE_GROUNDED)) {
    py -= gravity;
  }

  return Raycast_Of(px - vx, py - vy, vx, vy, 8);
}

static inline Hit
HitTrait_IsHitByRaycast(
    HitTrait *trait,
    Object *object)
{
  Cube *cube = trait->cube;
  Raycast ray = HitTrait_RaycasFromMovement(trait);

  Bounds hitbox = Bounds_Enlarge(&object->hitbox, &trait->hitbox.size);
  Hit hit = Raycast_Intersects(&ray, &hitbox);

  if (Hit_IsHit(&hit)) {
    // only delta y must be corrected for hit resolution
    const Body *body = Cube_GetBody(cube);
    hit.delta.y = hit.position.y - (body->position.y >> 8);
  }

  return hit;
}

static void
HitTrait_Callback(
    Unit *unit,
    Object *object,
    Hit *hit)
{
  HitTrait *trait = unit->object;

  Cube *cube = trait->cube;
  if (Cube_InState(cube, STATE_DESTROYED)) return;

  // detailed hit check using cube shape
  Shape *shape = &trait->shape;
  if (!Object_IsHit(object, shape)) return;

  if (object->type == TYPE_GOAL) {
    Cube_SetState(cube, STATE_VICTORY);
  }

  else if (object->deadly) {
    Cube_HaltMovement(cube);
    Cube_SetState(cube, STATE_DESTROYED);
  }

  else if (object->solid) {
    Hit rayhit = HitTrait_IsHitByRaycast(trait, object);
    if (Hit_IsHit(&rayhit)) {
      hit = &rayhit;
    }

    bool dead = hit->delta.x != 0;
    if (dead) {
      Cube_HaltMovement(cube);
      Cube_SetState(cube, STATE_DESTROYED);
    }

    else {
      const Body *body = Cube_GetBody(cube);
      int gravity = Math_signum(body->dynamics->gravity.y);
      int delta = Math_signum(hit->delta.y);

      if (gravity != delta) {
        HitTrait_ResolveHit(trait, hit->delta.y + gravity); // +1 to stay in contact with ground
        Cube_SetState(cube, STATE_GROUNDED);
      } else {
        HitTrait_ResolveHit(trait, hit->delta.y);
      }
    }
  }
}

static void
HitTrait_Apply(
    void *self,
    Course *course)
{
  HitTrait *trait = self;
  Cube *cube = trait->cube;

  Cube_ShiftState(cube);

  if (Cube_IsMoving(cube)) {
    // assume airbone state before checking hits
    cube->state.current = Cube_InState(cube, STATE_VICTORY) ? STATE_VICTORY : STATE_AIRBORNE;

    const Body *body = Cube_GetBody(cube);
    // apply current body position to hitbox
    trait->hitbox.center.x = body->position.x >> 8;
    trait->hitbox.center.y = body->position.y >> 8;

    Bounds hitbox = trait->hitbox;
    Shape *shape = &trait->shape;

    // shrink hitbox by one when rotating
    hitbox.size.x -= 1;
    hitbox.size.y -= 1;

    // rotate vertices stored in shape
    int angle = Cube_GetRotation(cube);
    Bounds_Rotate(&hitbox, shape, angle);

    Unit unit = Unit_Of(&trait->hitbox, trait);
    Course_CheckHits(course, &unit, HitTrait_Callback);
  }
}

Trait*
HitTrait_BindTo(
    HitTrait *hit,
    Cube *cube)
{
  Trait *trait = &hit->base;
  trait->self = hit;
  trait->enabled = false;
  trait->type = TRAIT_TYPE_HIT;
  trait->Apply = HitTrait_Apply;

  hit->cube = cube;
  hit->shape = Shape_Of(hit->vertices);
  hit->hitbox = Bounds_Of(0, 0, 8, 8);

  Cube_AddTrait(cube, trait);

  return trait;
}
