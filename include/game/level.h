#ifndef LEVEL_H
#define LEVEL_H

#include <gba.h>
#include <vector.h>

#include <game/chunk.h>
#include <game/object.h>

typedef enum LevelId {
  LEVEL_SELECT_COURSE,
  LEVEL_TEST_COURSE,
  LEVEL_COUNT,
} LevelId;

typedef struct Level {
  const enum Format {
    FORMAT_ASCII,
    FORMAT_BINV1,
  } format;
  union {
    const void *read;
    void *write;
  } buffer;
  const Vector size;
  Chunk *chunk;
  Vector cursor;
  int limit;
} Level;

#define layout(line, args...) ((const char[][sizeof(line)]) { line, ##args })

#define Level_FromLayout(line, args...) ((Level) { \
  .format = FORMAT_ASCII, \
  .buffer = { (void *) layout(line, ##args) }, \
  .size = { \
    .x = sizeof(line), \
    .y = length(layout(line, ##args)), \
  } \
})

#define Level_FromBuffer(buf) ((Level) { \
  .format = FORMAT_BINV1, \
  .buffer = { (void *) buf }, \
  .size = { length(buf) } \
})

Level*
Level_GetById(LevelId id);

int
Level_GetChunkCount(Level *level);

bool
Level_GetChunk(
    Level *level,
    Chunk *chunk);

bool
Level_AddChunk(
    Level *level,
    Chunk *chunk);

#endif
