#ifndef OBJECT_H
#define OBJECT_H

#include <gba.h>
#include <vector.h>
#include <hit.h>

typedef enum Type Type;

typedef struct Object {
  enum Type {
    TYPE_FLOOR,
    TYPE_DISK,
    TYPE_BOX,
    TYPE_BOX_WITH_POLE,
    TYPE_BOX_WITH_CHAINS,
    TYPE_PIT,
    TYPE_SPIKE,
    TYPE_PAD,
    TYPE_RING,
    TYPE_COIN,
    TYPE_PORTAL,
    TYPE_GOAL,
    TYPE_GOAL_WALL,
    TYPE_COUNT,
  } type;
  Bounds hitbox, viewbox;
  bool solid, deadly;
  char align4 properties[36];
} Object;

typedef struct Properties Properties;

bool
Object_CreateFloor(Object *object);

bool
Object_CreateDisk(
    Object *object,
    int width);

bool
Object_CreateOffsetDisk(
    Object *object,
    Direction direction,
    int width);

bool
Object_CreatePad(Object *object);

bool
Object_CreateRing(Object *object);

bool
Object_CreatePortal(Object *object);

bool
Object_CreateCoin(Object *object);

bool
Object_CreateRegularBox(
    Object *object,
    int width, int height);

bool
Object_CreateGridBox(
    Object *object,
    int width, int height);

bool
Object_CreateBoxWithPole(
    Object *object,
    int height);

bool
Object_CreateBoxWithChains(
    Object *object,
    int height);

bool
Object_CreatePit(
    Object *object,
    int width,
    bool hanging);

bool
Object_CreateSpike(
    Object *object,
    Direction direction);

bool
Object_CreateTinySpike(Object *object);

bool
Object_CreateGoal(Object *object);

bool
Object_CreateGoalWall(
    Object *object,
    int height);

static inline Properties*
Object_GetProperties(Object *object) {
  return (Properties *) object->properties;
}

bool
Object_IsHit(
    Object *object,
    Shape *shape);

void
Object_Move(
    Object *object,
    Vector *position);

void
Object_Draw(
    Object *object,
    GBA_TileMapRef *target);

bool
Object_Animate(
    Object *object,
    GBA_TileMapRef *target,
    int frame);

static inline void
Object_AssignFrom(Object *object, const Object *other) {
  *object = *other;
}

#endif
