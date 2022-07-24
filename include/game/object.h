#ifndef OBJECT_H
#define OBJECT_H

#include <gba.h>
#include <vector.h>
#include <hit.h>

#include <game/camera.h>

typedef struct Object {
  Vector position;
  // TODO should be predefined and referenced here
  Bounds hitbox;
  const GBA_TileMapRef *tiles;
} Object;

void
Object_CreateBlock(Object *object);

void
Object_CreateBlockWithPole(Object *object);

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
  object->hitbox.center.x = (position->x * 8) + object->hitbox.center.x;
  object->hitbox.center.y = (position->y * 8) + object->hitbox.center.y;
}

void
Object_DrawColumn(
    Object *object,
    int x);

void
Object_DrawRow(
    Object *object,
    int y);

#endif
