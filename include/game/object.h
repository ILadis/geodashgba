#ifndef OBJECT_H
#define OBJECT_H

#include <gba.h>
#include <vector.h>
#include <hit.h>

#include <game/camera.h>

typedef struct Object {
  Bounds hitbox, viewbox;
  const GBA_TileMapRef *tiles;
  bool solid, deadly;
  // TODO consider adding action function (when cube hits this object)?
} Object;

void
Object_CreateBox(Object *object);

void
Object_CreateBoxWithPole(
    Object *object,
    int height);

void
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

static inline void
Object_AssignFrom(Object *object, Object *other) {
  *object = *other;
}

#endif
