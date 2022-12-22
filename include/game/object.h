#ifndef OBJECT_H
#define OBJECT_H

#include <gba.h>
#include <vector.h>
#include <hit.h>

#include <game/camera.h>

typedef struct Object {
  Bounds hitbox, viewbox;
  const struct Prototype *proto;
  bool solid, deadly;
  char align4 properties[32];
} Object;

typedef struct Prototype {
  bool (*hit)(Object *object, Shape *shape);
  void (*move)(Object *object, Vector *position);
  void (*draw)(Object *object, GBA_TileMapRef *target);
  // TODO consider trigger and tick functions
} Prototype;

typedef struct Properties Properties;

bool
Object_CreateDisk(Object *object);

bool
Object_CreateBox(
    Object *object,
    int width, int height);

bool
Object_CreateBoxWithPole(
    Object *object,
    int height);

bool
Object_CreateSpike(
    Object *object,
    Direction direction);

static inline Properties*
Object_GetProperties(Object *object) {
  return (Properties *) object->properties;
}

static inline bool
Object_IsHit(
    Object *object,
    Shape *shape)
{
  if (object->proto->hit != NULL) {
    return object->proto->hit(object, shape);
  }

  return true;
}

static inline void
Object_Move(
    Object *object,
    Vector *position)
{
  object->hitbox.center.x  += position->x * 8;
  object->hitbox.center.y  += position->y * 8;
  object->viewbox.center.x += position->x * 8;
  object->viewbox.center.y += position->y * 8;

  if (object->proto->move != NULL) {
    object->proto->move(object, position);
  }
}

static inline void
Object_Draw(
    Object *object,
    GBA_TileMapRef *target)
{
  object->proto->draw(object, target);
}

static inline void
Object_AssignFrom(Object *object, Object *other) {
  *object = *other;
}

#endif
