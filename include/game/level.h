#ifndef LEVEL_H
#define LEVEL_H

#include <gba.h>
#include <vector.h>

#include <game/chunk.h>
#include <game/object.h>

typedef enum LevelId {
  LEVEL_EMPTY,
  LEVEL_STEREO_MADNESS,
  LEVEL_BACK_ON_TRACK,
  LEVEL_POLARGEIST,
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

#define Level_ToLayout(line, lines...) ((const char[][sizeof(line)]) { line, ##lines })
#define Level_FromLayout(line, lines...) ((Level) { \
  .format = FORMAT_ASCII, \
  .buffer = { (void *) Level_ToLayout(line, ##lines) }, \
  .size = { \
    .x = sizeof(line), \
    .y = length(Level_ToLayout(line, ##lines)), \
  } \
})

#define Level_ToBinv1(byte, bytes...) ((const unsigned char[]) { byte, ##bytes })
#define Level_FromData(byte, bytes...) ((Level) { \
  .format = FORMAT_BINV1, \
  .buffer = { (void *) Level_ToBinv1(byte, ##bytes) }, \
  .size = { length(Level_ToBinv1(byte, ##bytes)) } \
})

#define Level_AllocateNew(length) ((Level) { \
  .format = FORMAT_BINV1, \
  .buffer = { (void *) ((unsigned char[length]) {0}) }, \
  .size = { length } \
})

Level*
Level_GetById(LevelId id);

void
Level_GetName(
    Level *level,
    char *name);

void
Level_SetName(
    Level *level,
    char *name);

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
