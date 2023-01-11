
#include <game/chunk.h>

void
Chunk_AssignIndex(
    Chunk *chunk,
    int index)
{
  Grid *grid = &chunk->grid;
  Grid_Reset(grid);
  Grid_SetCells(grid, chunk->cells);

  const int width  = 240; // 30 tiles
  const int height = 480; // 60 tiles

  int x = width * index + width/2;
  int y = height/2;

  Bounds bounds = Bounds_Of(x, y, width/2, height/2);
  Grid_SetBounds(grid, &bounds);

  chunk->index = index;
  chunk->count = 0;
}

Object*
Chunk_AllocateObject(Chunk *chunk) {
  int index = chunk->count;
  Object *object = NULL;

  if (index < length(chunk->objects)) {
    object = &chunk->objects[index];
    chunk->count++;
  }

  return object;
}

void
Chunk_AddObject(
    Chunk *chunk,
    Object *object)
{
  Grid *grid = &chunk->grid;

  Bounds *hitbox = &object->hitbox;
  Unit unit = Unit_Of(hitbox, object);

  if (!Grid_AddUnit(grid, &unit)) {
    // free the last allocated object if we can not add it to grid
    chunk->count--;
  }
}

Object*
Chunk_FindObjectByTyoe(
    Chunk *chunk,
    Type type)
{
  int count = chunk->count;
  for (int i = 0; i < count; i++) {
    Object *object = &chunk->objects[i];

    if (object->type == type) {
      return object;
    }
  }

  return NULL;
}

bool
Chunk_CheckHits(
    Chunk *chunk,
    Unit *unit,
    HitCallback callback)
{
  Grid *grid = &chunk->grid;

  Bounds *hitbox = unit->bounds;
  bool result = false;

  Iterator iterator;
  Grid_GetUnits(grid, hitbox, &iterator);

  while (Iterator_HasNext(&iterator)) {
    Unit *other = Iterator_GetNext(&iterator);
    Object *object = other->object;

    Bounds *bounds = &object->hitbox;

    Hit hit = Bounds_Intersects(bounds, hitbox);
    if (Hit_IsHit(&hit)) {
      HitCallback_Invoke(callback, unit, object, &hit);
      result = true;
    }
  }

  return result;
}
