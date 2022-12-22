#ifndef LOADER_H
#define LOADER_H

#include <gba.h>
#include <vector.h>

#include <game/chunk.h>
#include <game/object.h>

typedef struct Loader {
  const char *layout;
  const Vector size;
  Chunk *chunk;
  Vector cursor;
  int limit;
} Loader;

#define layout(line, args...) ((const char[][sizeof(line)]) { line, ##args })

#define Loader_ForLayout(line, args...) ((Loader) { \
  .layout = (char *) layout(line, ##args), \
  .size = { \
    .x = sizeof(line), \
    .y = length(layout(line, ##args)), \
  } \
})

Loader*
Loader_ForTestCourse();

void
Loader_GetChunk(
    Loader *loader,
    Chunk *chunk);

#endif
