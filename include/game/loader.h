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

#define Loader_ForLayout(...) ((Loader) { .layout = {__VA_ARGS__, NULL} })

Loader*
Loader_ForTestCourse(bool reset);

void
Loader_GetChunk(
    Loader *loader,
    Chunk *chunk);

#endif
