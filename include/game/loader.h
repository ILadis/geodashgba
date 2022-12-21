#ifndef LOADER_H
#define LOADER_H

#include <gba.h>
#include <vector.h>

#include <game/chunk.h>
#include <game/object.h>

typedef struct Loader {
  const char **layout;
  Vector size, cursor;
} Loader;

#define Loader_ForLayout(layout) ((Loader) { layout, { 0, length(layout) }, { 0, 0 } })

Loader*
Loader_ForTestCourse();

void
Loader_GetChunk(
    Loader *loader,
    Chunk *chunk);

#endif
