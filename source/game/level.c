
#include <game/level.h>

void
Level_GetChunk(
    Level *level,
    Chunk *chunk)
{
  extern void AsciiLevel_GetChunk(Level *level, Chunk *chunk);
  extern void Binv1Level_GetChunk(Level *level, Chunk *chunk);

  const void (*get[])(Level *level, Chunk *chunk) = {
    [FORMAT_ASCII] = AsciiLevel_GetChunk,
    [FORMAT_BINV1] = Binv1Level_GetChunk,
  };

  get[level->format](level, chunk);
}

void
Level_AddChunk(
    Level *level,
    Chunk *chunk)
{
  extern void AsciiLevel_AddChunk(Level *level, Chunk *chunk);
  extern void Binv1Level_AddChunk(Level *level, Chunk *chunk);

  const void (*add[])(Level *level, Chunk *chunk) = {
    [FORMAT_ASCII] = AsciiLevel_AddChunk,
    [FORMAT_BINV1] = Binv1Level_AddChunk,
  };

  add[level->format](level, chunk);
}
