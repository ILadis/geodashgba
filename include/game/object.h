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
  char align4 properties[16];
} Object;

typedef struct Prototype {
  void (*draw)(Object *object, GBA_TileMapRef *target);
  // TODO consider trigger and tick functions
} Prototype;

typedef struct Properties Properties;

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

static inline void
Object_SetPosition(
    Object *object,
    int x, int y)
{
  object->hitbox.center.x  += x * 8;
  object->hitbox.center.y  += y * 8;
  object->viewbox.center.x += x * 8;
  object->viewbox.center.y += y * 8;
}

static inline Properties*
Object_GetProperties(Object *object) {
  return (Properties *) object->properties;
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
