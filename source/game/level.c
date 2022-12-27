
#include <game/level.h>

int
Level_GetChunkCount(Level *level) {
  extern int AsciiLevel_GetChunkCount(Level *level);
  extern int Binv1Level_GetChunkCount(Level *level);

  const int (*count[])(Level *level) = {
    [FORMAT_ASCII] = AsciiLevel_GetChunkCount,
    [FORMAT_BINV1] = Binv1Level_GetChunkCount,
  };

  return count[level->format](level);
}

bool
Level_GetChunk(
    Level *level,
    Chunk *chunk)
{
  extern bool AsciiLevel_GetChunk(Level *level, Chunk *chunk);
  extern bool Binv1Level_GetChunk(Level *level, Chunk *chunk);

  const bool (*get[])(Level *level, Chunk *chunk) = {
    [FORMAT_ASCII] = AsciiLevel_GetChunk,
    [FORMAT_BINV1] = Binv1Level_GetChunk,
  };

  return get[level->format](level, chunk);
}

bool
Level_AddChunk(
    Level *level,
    Chunk *chunk)
{
  extern bool AsciiLevel_AddChunk(Level *level, Chunk *chunk);
  extern bool Binv1Level_AddChunk(Level *level, Chunk *chunk);

  const bool (*add[])(Level *level, Chunk *chunk) = {
    [FORMAT_ASCII] = AsciiLevel_AddChunk,
    [FORMAT_BINV1] = Binv1Level_AddChunk,
  };

  return add[level->format](level, chunk);
}
