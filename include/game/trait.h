#ifndef TRAIT_H
#define TRAIT_H

#include <types.h>
#include <grid.h>
#include <hit.h>

#include <game/body.h>
#include <game/course.h>

typedef struct Cube Cube;

typedef struct Trait {
  bool enabled;
  void *self;
  enum {
    TRAIT_TYPE_SPAWN,
    TRAIT_TYPE_MOVE,
    TRAIT_TYPE_FLY,
    TRAIT_TYPE_ROTATE,
    TRAIT_TYPE_HIT,
    TRAIT_COUNT,
  } type;
  void (*Enabled)(void *self, bool enabled);
  void (*Apply)(void *self, Course *course);
  void (*Action)(void *self);
} Trait;

static inline void
Trait_SetEnabled(Trait *trait, bool enabled) {
  bool changed = trait->enabled != enabled;
  if (changed) {
    trait->enabled = enabled;
    if (trait->Enabled != NULL) {
      trait->Enabled(trait->self, enabled);
    }
  }
}

static inline void
Trait_Apply(Trait *trait, Course *course) {
  if (trait->enabled) {
    trait->Apply(trait->self, course);
  }
}

static inline void
Trait_Action(Trait *trait) {
  if (trait->enabled && trait->Action != NULL) {
    trait->Action(trait->self);
  }
}

typedef struct HitTrait {
  Trait base;
  Cube *cube;
  Bounds hitbox;
  Vector vertices[4];
  Shape shape;
} HitTrait;

Trait*
HitTrait_BindTo(
    HitTrait *trait,
    Cube *cube,
    bool enabled);

typedef struct MoveTrait {
  Trait base;
  Cube *cube;
  Body body;
  int launch;
} MoveTrait;

Trait*
MoveTrait_BindTo(
    MoveTrait *trait,
    Cube *cube,
    bool enabled);

void
MoveTrait_Jump(
    MoveTrait *trait,
    int speed);

void
MoveTrait_Launch(
    MoveTrait *trait,
    int speed);

void
MoveTrait_Accelerate(
    MoveTrait *trait,
    Direction direction,
    int speed);

void
MoveTrait_HaltMovement(MoveTrait *trait);

typedef struct RotateTrait {
  Trait base;
  Cube *cube;
  int angle;
  int velocity;
} RotateTrait;

Trait*
RotateTrait_BindTo(
    RotateTrait *trait,
    Cube *cube,
    bool enabled);

typedef struct SpawnTrait {
  Trait base;
  Cube *cube;
  int timer;
} SpawnTrait;

Trait*
SpawnTrait_BindTo(
    SpawnTrait *trait,
    Cube *cube,
    bool enabled);

typedef struct FlyTrait {
  Trait base;
  Cube *cube;
  Body *body;
  int acceleration;
  struct {
    const Dynamics *dynamics;
    Vector limits;
  } restore;
} FlyTrait;

Trait*
FlyTrait_BindTo(
    FlyTrait *trait,
    Cube *cube,
    bool enabled);

#endif
