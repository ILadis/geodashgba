#ifndef OBJECT_H
#define OBJECT_H

#include <gba.h>
#include <math.h>
#include <vector.h>
#include <hit.h>

typedef enum Type Type;

typedef struct Object {
  enum Type {
    OBJECT_TYPE_FLOOR,
    OBJECT_TYPE_DISK,
    OBJECT_TYPE_BOX,
    OBJECT_TYPE_BOX_WITH_POLE,
    OBJECT_TYPE_BOX_WITH_CHAINS,
    OBJECT_TYPE_PIT,
    OBJECT_TYPE_SPIKE,
    OBJECT_TYPE_PAD,
    OBJECT_TYPE_RING,
    OBJECT_TYPE_COIN,
    OBJECT_TYPE_PORTAL,
    OBJECT_TYPE_GOAL,
    OBJECT_TYPE_GOAL_WALL,
    OBJECT_TYPE_COUNT,
  } type;
  Bounds hitbox, viewbox;
  bool solid, deadly;
  char align4 properties[36];
} Object;

typedef struct Properties Properties;

typedef struct Faces {
  unsigned char repeat: 4;
  unsigned char flags:  4;
} Faces;

#define FACES_MAX_REPEAT 15

static inline void
Faces_SetFaceBorder(Faces *faces, Direction direction) {
  faces->flags |= 1 << direction;
}

static inline bool
Faces_IncrementRepeat(Faces *faces, Faces other) {
  if (faces->repeat >= FACES_MAX_REPEAT) {
    return false;
  }

  if (faces->flags == other.flags) {
    faces->repeat++;
    return true;
  }

  return false;
}

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
Object_CreateGravityPortal(Object *object);

bool
Object_CreateFlyPortal(Object *object);

bool
Object_CreateCoin(
    Object *object,
    int index);

bool
Object_CreateRegularBox(
    Object *object,
    int width, int height);

bool
Object_CreateGridBox(
    Object *object,
    Faces *faces,
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
