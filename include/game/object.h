#ifndef OBJECT_H
#define OBJECT_H

#include <gba.h>
#include <vector.h>
#include <hit.h>

#include <game/camera.h>

typedef struct Object {
  Vector position;
  Bounds hitbox, viewbox;
  const GBA_TileMapRef *tiles;
  // TODO consider adding action function (when cube hits this object)?
} Object;

void
Object_CreateBlock(Object *object);

void
Object_CreateBlockWithPole(Object *object);

void
Object_CreateLowPlatform(Object *object);

void
Object_CreateBox(Object *object);

void
Object_CreatePole(Object *object);

static inline void
Object_SetPosition(
    Object *object,
    Vector *position)
{
  object->position.x = position->x;
  object->position.y = position->y;
  object->hitbox.center.x  += position->x * 8;
  object->hitbox.center.y  += position->y * 8;
  object->viewbox.center.x += position->x * 8;
  object->viewbox.center.y += position->y * 8;
}

void
Object_DrawColumn(
    Object *object,
    Camera *camera,
    int x);

void
Object_DrawRow(
    Object *object,
    Camera *camera,
    int y);

#endif
