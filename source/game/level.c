
#include <game/level.h>

static Level testCourse = Level_FromLayout(
  "                                                                                                                                                                                            ",
  "                                                                                                                                                                                            ",
  "                                                                                                                                                                                            ",
  "                                                                                                                            --      --       ^^^^                                           ",
  "                                                                                                               --          ^^^^    ^^^^      xxxx         ^                                 ",
  "                                                                                                           -      xxxx  xxxxxxxxxxxxxxx               xxxxx                                 ",
  "                                                        :                                              -                xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx ^              ^                ",
  "                                                       <x                                          -                    xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx xxxxxxxxx  ----x        ^       ",
  "                                    i                  <x                                      -                        xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx                   ------x       ",
  "                               i    x                                            ^         -                            xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx                           ----- ",
  "                          i    x    x                             ^        xxxxxxxxxxxxx                                xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx                                 ",
  "_________^______^^_______^x____x____x_________^^___xxxxxxxx__xxxxxxxxxxx___xxxxxxxxxxxxx________________________________xxxxxxxxxxxxxxx_xxxxxxxxxxxxxxxxxxx_________________________________",
);

extern const unsigned char levelData[362];
static Level binv1Course = Level_FromBuffer(levelData);

Level*
Level_GetById(LevelId id) {
  static Level *levels[] = {
    [LEVEL_TEST_COURSE] = &testCourse,
    [LEVEL_BINV1_COURSE] = &binv1Course,
  };

  return levels[id];
}

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
