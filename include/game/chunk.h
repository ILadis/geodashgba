#ifndef CHUNK_H
#define CHUNK_H

#include <gba.h>
#include <hit.h>
#include <grid.h>

#include <game/object.h>

typedef void (*HitCallback)(Unit *unit, Object *object, Hit *hit);

typedef struct Chunk {
  Object objects[20];
  Cell cells[20];
  Grid grid;
  int index, count;
} Chunk;

static inline const Bounds*
Chunk_GetBounds(Chunk *chunk) {
  return &chunk->grid.root.bounds;
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

Hit
Chunk_CheckHits(
    Chunk *chunk,
    Unit *unit,
    HitCallback callback);

#endif
