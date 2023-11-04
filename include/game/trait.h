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
    TRAIT_TYPE_ROTATE,
    TRAIT_TYPE_HIT,
    TRAIT_COUNT,
  } type;
  void (*Apply)(void *self, Course *course);
} Trait;

static inline void
Trait_Apply(Trait *trait, Course *course) {
  return trait->Apply(trait->self, course);
}

static inline void
Trait_ApplyAll(Trait **traits, Course *course) {
  for (int index = 0; index < TRAIT_COUNT; index++) {
    if (traits[index] != NULL) {
      Trait_Apply(traits[index], course);
    }
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
    Cube *cube);

typedef struct MoveTrait {
  Trait base;
  Cube *cube;
  Body body;
  int launch;
} MoveTrait;

Trait*
MoveTrait_BindTo(
    MoveTrait *trait,
    Cube *cube);

void
MoveTrait_Jump(
    MoveTrait *trait,
    int speed);

void
MoveTrait_HaltMovement(MoveTrait *trait);

void
MoveTrait_Launch(
    MoveTrait *trait,
    int speed);

void
MoveTrait_Accelerate(
    MoveTrait *trait,
    Direction direction,
    int speed);

typedef struct RotateTrait {
  Trait base;
  Cube *cube;
  int angle;
  int velocity;
} RotateTrait;

Trait*
RotateTrait_BindTo(
    RotateTrait *trait,
    Cube *cube);

typedef struct SpawnTrait {
  Trait base;
  Cube *cube;
  int timer;
} SpawnTrait;

Trait*
SpawnTrait_BindTo(
    SpawnTrait *trait,
    Cube *cube);

#endif
