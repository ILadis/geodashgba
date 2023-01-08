
#include <game/level.h>

#ifdef NOGBA
static Level course01 = Level_FromLayout(
  "       ",
  "       ",
  "_______",
);

static Level course02 = Level_FromLayout(
  "                                                                                                                                                                                             ",
  "                                                                                                                            --      --       ^^^^                                            ",
  "                                                                                                               --          ^^^^    ^^^^      ~~~~         ^                                  ",
  "                                                                                                           -      xxxx  xxxxxxxxxxxxxxx               xxxxx                                  ",
  "                                                        :                                              -                xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx ^              ^                 ",
  "                                                       <x                                          -                    xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx xxxxxxxxx  ~~~~~        ^        ",
  "                                      i                <x                                      -                                        xxxxxxxxxxxxxxxxxxx                   ~~~~~~~        ",
  "                                  i   x                                          ^         -                                                                                           ----  ",
  "                              i   x   x                           ^        xxxxxxxxxxxxx                                xxxxxxxxxxxxxxx                                                      ",
  "_____________^______^^_______^x___x___x_______^^___xxxxxxxx__xxxxxxxxxxx___xxxxxxxxxxxxx________________________________xxxxxxxxxxxxxxx_xxxxxxxxxxxxxxxxxxx__________________________T__^^^^_",
);

static Level course03 = Level_FromLayout(
  "                                                                                                                                                                                                                                                        xxx                                                                                                                                          ----                                                                                                                                                             x                                                                                                                                ",
  "                                                                                                         xxxxxxxxxxxx                                                                                                                                   vvv                                                             ~~                                                                                ------                                                                                                                                                      v                                                                                                                                ",
  "                                                                                                         vvvvvvvvvvvv                                                                -                    ^                                                                                         i    ^          --      ~                               -        T                            T                   ~                                                                                           x       x       x                                      ^                                                               x       x       x                                i ^          ",
  "                                                                         i                                                                                                       -                        ~              T                T                                                     i   x~~~~x                    ~                         -        ~   x                    xx      x              ~~     ~ T                                                            ^     ^                    v       v       v                                  ~~~~~~~~~~     ^                                                    v       v       v          i      i   xxx    i   x x          ",
  "                                       i                             i   x                 ^                                 ^           ^                      ^            -                         -             i   x            i   x             x                                   i   x   x    x --- T                ~~            ^     -                x                    vv      x                       ~    ~~~x                       -      -                   T ~     ~ ~                                                                 ~              ~~~~~       ^              xx             xxxx                                  i   x  i   x          x   x            ",
  "                                   i   x             ~           i   x   x                 ~                                 ~           ~                 x~~~~x     ^  T               ~~~~      ~     -    ^  i   x   x     x  i   x   x         x   x                                   x   x   x    x     ~                      x~~~~~~~~~~x                   x                         x  x                               x                    -    --      T            i   ~ x   T x   ~~    T          x       x       x        ^        ^        ~                        ~~~~~~~              vv             vvvv           x       x       x      x   x  x   x          x   x      T     ",
  "_______________T^^^^______^^_____^^x___x_________T^^^^^^^______^^x___x___x^^^____^^____^_______^__________T___T___T______^_______^^x^^^______T^^^^x~~~~x___x____x__x~~~~~x____________________~~________ __x~~x__x___x___x_____x__x___x___x_____x___x___x__x~~x^^^____^____^^____^^x____x___x___x___x____x____________________________x__________x___________________x^^^^^^_____^^____^^______x__x_______________________________x^______^^____^x~x________________x^^^^______^^x___ _x___x_x________xx^^^^_____^x______^x______^x~~~~~~~~~~~x__~~~~_~~~~_________________________________________^___^_________^____^__________^^_____^x______^x______^x~~x___x___x__x___x__________x___x____~xx^^___",
);
#else
extern Level course01;
extern Level course02;
extern Level course03;
#endif

Level*
Level_GetById(LevelId id) {
  static Level *levels[LEVEL_COUNT] = {
    [LEVEL_EMPTY] = &course01,
    [LEVEL_STEREO_MADNESS] = &course02,
    [LEVEL_BACK_ON_TRACK] = &course03,
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
