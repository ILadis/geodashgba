#ifndef LEVEL_H
#define LEVEL_H

#include <gba.h>
#include <io.h>
#include <vector.h>

#include <game/chunk.h>
#include <game/object.h>

typedef int LevelId;

typedef struct Level {
  void *self;
  void (*GetName)(void *self, char *name);
  void (*SetName)(void *self, char *name);
  int  (*GetChunkCount)(void *self);
  bool (*GetChunk)(void *self, Chunk *chunk);
  bool (*AddChunk)(void *self, Chunk *chunk);
} Level;

static inline void
Level_GetName(Level *level, char *name) {
  level->GetName(level->self, name);
}

static inline void
Level_SetName(Level *level, char *name) {
  level->SetName(level->self, name);
}

static inline int
Level_GetChunkCount(Level *level) {
  return level->GetChunkCount(level->self);
}

static inline bool
Level_GetChunk(Level *level, Chunk *chunk) {
  return level->GetChunk(level->self, chunk);
}

static inline bool
Level_AddChunk(Level *level, Chunk *chunk) {
  return level->AddChunk(level->self, chunk);
}

int
Level_Convert(
    Level *from,
    Level *to);

typedef struct AsciiLevel {
  DataSource *source;
  Chunk *chunk;
  Vector size, cursor;
  int limit;
  struct Level base;
} AsciiLevel;

Level*
AsciiLevel_From(
    AsciiLevel *level,
    DataSource *source);

typedef struct Binv1Level {
  DataSource *source;
  Chunk *chunk;
  int size;
  struct Level base;
} Binv1Level;

Level*
Binv1Level_From(
    Binv1Level *level,
    DataSource *source);

#endif
