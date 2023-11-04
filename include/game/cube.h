#ifndef CUBE_H
#define CUBE_H

#include <gba.h>
#include <math.h>
#include <vector.h>

#include <game/camera.h>
#include <game/body.h>
#include <game/course.h>
#include <game/trait.h>

typedef enum State {
  STATE_UNKNOWN,
  STATE_AIRBORNE,
  STATE_GROUNDED,
  STATE_DESTROYED,
  STATE_VICTORY,
} State;

typedef struct Cube {
  bool success;
  struct {
    State current;
    State previous;
  } state;
  Trait *traits[TRAIT_COUNT];
  GBA_Sprite *sprite;
} Cube;

Cube*
Cube_GetInstance();

void
Cube_Reset(Cube *cube);

static inline void*
Cube_GetTrait(Cube *cube, int index) {
  return cube->traits[index]->self;
}

static inline void
Cube_AddTrait(Cube *cube, Trait *trait) {
  cube->traits[trait->type] = trait;
}

static inline void
Cube_SetTraitEnabled(Cube *cube, int index, bool enabled) {
  Trait_SetEnabled(cube->traits[index], enabled);
}

static inline int
Cube_GetRotation(Cube *cube) {
  RotateTrait *trait = Cube_GetTrait(cube, TRAIT_TYPE_ROTATE);
  return trait->angle;
}

static inline const Body*
Cube_GetBody(Cube *cube) {
  MoveTrait *trait = Cube_GetTrait(cube, TRAIT_TYPE_MOVE);
  return &trait->body;
}

static inline const Vector*
Cube_GetPosition(Cube *cube) {
  HitTrait *trait = Cube_GetTrait(cube, TRAIT_TYPE_HIT);
  return &trait->hitbox.center;
}

static inline void
Cube_SetPosition(Cube *cube, const Vector *position) {
  MoveTrait *trait = Cube_GetTrait(cube, TRAIT_TYPE_MOVE);
  Body_SetPosition(&trait->body, position->x << 8, position->y << 8);
}

static inline bool
Cube_EnteredState(Cube *cube, State state) {
  return cube->state.current == state && cube->state.previous != state;
}

static inline bool
Cube_InState(Cube *cube, State state) {
  return cube->state.current == state;
}

static inline void
Cube_SetState(Cube *cube, State state) {
  cube->state.current = state;
  cube->success |= state == STATE_VICTORY;
}

static inline void
Cube_ShiftState(Cube *cube) {
  cube->state.previous = cube->state.current;
}

static inline bool
Cube_IsMoving(Cube *cube) {
  const Body *body = Cube_GetBody(cube);
  return body->velocity.x != 0
      || body->velocity.y != 0;
}

static inline bool
Cube_IsShadowShape(Shape *shape) {
  Cube *cube = Cube_GetInstance();
  HitTrait *trait = Cube_GetTrait(cube, TRAIT_TYPE_HIT);
  return &trait->shape != shape;
}

static inline void
Cube_Jump(Cube *cube, int speed) {
  MoveTrait *trait = Cube_GetTrait(cube, TRAIT_TYPE_MOVE);
  MoveTrait_Jump(trait, speed);
}

static inline void
Cube_Launch(Cube *cube, int speed) {
  MoveTrait *trait = Cube_GetTrait(cube, TRAIT_TYPE_MOVE);
  MoveTrait_Launch(trait, speed);
}

static inline void
Cube_Accelerate(Cube *cube, Direction direction, int speed) {
  MoveTrait *trait = Cube_GetTrait(cube, TRAIT_TYPE_MOVE);
  MoveTrait_Accelerate(trait, direction, speed);
}

static inline void
Cube_HaltMovement(Cube *cube) {
  MoveTrait *trait = Cube_GetTrait(cube, TRAIT_TYPE_MOVE);
  MoveTrait_HaltMovement(trait);
}

static inline void
Cube_ChangeDynamics(Cube *cube, const Dynamics *dynamics) {
  MoveTrait *trait = Cube_GetTrait(cube, TRAIT_TYPE_MOVE);
  Body_SetDynamics(&trait->body, dynamics);
}

void
Cube_Update(
    Cube *cube,
    Course *course);

void
Cube_Action(Cube *cube);

void
Cube_Draw(
    Cube *cube,
    Camera *camera);

#endif
