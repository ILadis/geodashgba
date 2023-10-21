#ifndef CHUNK_H
#define CHUNK_H

#include <gba.h>
#include <hit.h>
#include <grid.h>

#include <game/object.h>
#include <game/camera.h>

typedef void (*HitCallback)(Unit *unit, Object *object, Hit *hit);

typedef struct Chunk {
  Object objects[20];
  Cell cells[20];
  Grid grid;
  Bounds bounds;
  unsigned int index, count;
} Chunk;

static inline void
HitCallback_Invoke(
    HitCallback callback,
    Unit *unit,
    Object *object,
    Hit *hit)
{
  if (callback != NULL) {
    callback(unit, object, hit);
  }
}

static inline const Bounds*
Chunk_GetBounds(Chunk *chunk) {
  /* Note: This previously returned the bounds of chunk-grid. But if the
   * bounds of a grid grows (for example if an object hitbox is too large
   * to fit), this would result in overlapping course chunks. That's why
   * a separate fixed chunk bounds is currently needed.
   */
  return &chunk->bounds;
}

static inline bool
Chunk_InViewport(
    Chunk *chunk,
    Camera *camera)
{
  const Bounds *bounds = Chunk_GetBounds(chunk);
  return Camera_InViewport(camera, bounds);
}

void
Chunk_AssignIndex(
    Chunk *chunk,
    int index);

Object*
Chunk_AllocateObject(Chunk *chunk);

void
Chunk_AddObject(
    Chunk *chunk,
    Object *object);

Object*
Chunk_FindObjectByType(
    Chunk *chunk,
    Type type);

bool
Chunk_CheckHits(
    Chunk *chunk,
    Unit *unit,
    HitCallback callback);

#endif
