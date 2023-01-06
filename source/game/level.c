
#include <game/level.h>

#ifdef NOGBA
static Level course01 = Level_FromLayout(
  "              ",
  "              ",
  "_________^____",
);

static Level course02 = Level_FromLayout(
  "                                                                                                                                                                                             ",
  "                                                                                                                                                                                             ",
  "                                                                                                                                                                                             ",
  "                                                                                                                            --      --       ^^^^                                            ",
  "                                                                                                               --          ^^^^    ^^^^      xxxx         ^                                  ",
  "                                                                                                           -      xxxx  xxxxxxxxxxxxxxx               xxxxx                                  ",
  "                                                        :                                              -                xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx ^                                ",
  "                                                       <x                                          -                    xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx xxxxxxxxx  ----^                 ",
  "                                    i                  <x                                      -                                        xxxxxxxxxxxxxxxxxxx                x  ------^        ",
  "                               i    x                                            ^         -                                                                                        x  ----  ",
  "                          i    x    x                     T       ^    T   xxxxxxxxxxxxx                                xxxxxxxxxxxxxxx                                                      ",
  "_________^______^^_______^x____x____x_________^^___xxxxxxxx__xxxxxxxxxxx___xxxxxxxxxxxxx________________________________xxxxxxxxxxxxxxx_xxxxxxxxxxxxxxxxxxx__________________________TT_^^^^_",
);
#else
extern Level course01;
extern Level course02;
#endif

Level*
Level_GetById(LevelId id) {
  static Level *levels[] = {
    [LEVEL_SELECT_COURSE] = &course01,
    [LEVEL_TEST_COURSE] = &course02,
  };

  Level *level = NULL;
  if (id < length(levels)) {
    level = levels[id];
  }

  return level;
}

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
