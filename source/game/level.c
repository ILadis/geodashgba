
#include <game/level.h>

#ifdef NOGBA
static Level course01 = Level_FromLayout(
  "                             |",
  "                             |",
  "                             |",
  "                             |",
  "                             |",
  "                             |",
  "                       G     |",
  "                             |",
  "             i    T          |",
  "             x   xx^^        |",
);

static Level course02 = Level_FromLayout(
  "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |",
  "                                                                                                                                                                                                                                                : :                                                                                                                                                                                                                                                                                                          |",
  "                                                                                                                            --      --       ^^^^                                                            ^                                  xox                                                                                                                                                                                                                                                                                                          |",
  "                                                                                                               --          ^^^^    ^^^^      ~~~~         ^                                                  ~                                  vvv                       i                                                                                                                                                                           ^                                                                                                      |",
  "                                                                                                           -      ____  xxxxxxxxxxxxxxx              ^xxxxx                                              ~                                                            i   x~~~                                                                 :      : :      :      :                                                                               x ^                                   : :                                                              |",
  "                                                        :                                              -                xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx^               ^                         ~     --------                                               i   x   x    --                                                              xo     xox      x      x                                                                                 x ^                                 xox                                                         G    |",
  "                                                       <x                                          -                    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  ~~~~~        ^            ~                   -                     i   i              i   x   x   x        --                                                          vv     vvv      v      v                   i                                                       ___     x ^                               vvv                                                              |",
  "                                      i                <x                                      -                        xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx         ~~~~~~~        ~                          -              i   x   x          i   x   x   x   x               ___                                                                                         i   x~                   :  :                           _       x     ~                                                                                      T         |",
  "                                  i   x                                          ^         -                                           v             v     v                           ----                                -           x   x   x          x   x   x   x   x               xxx    ___                            --                                                i   x   x   ~                xoox                       _             x          ^     ~~~                                          ~           ^      ^      ~~~x         |",
  "                              i   x   x                           ^        xxxxxxxxxxxxx                                                                                                                                      x~~~~x   x   x   x xxxxxx   x   x   x   x   x               xxx    xxx ~              --      --                                                    x   x   x      ~             vvvv                   _                   x      ~ ~ ~       -   ___                           x~~~      ~      ~ ~ ~~~~ ~ ~~      x         |",
  "             ^      ,^      ^^x...x...x       ^^   xxxxxxxx..xxxxxxxxxxx...xxxxxxxxxxxxx.......................o~~~~~~~~xxxxxxxxxxxxxxx~xxxxxxxxxxxxx~xxxxx~xxxx~~--__  ^^           T  ^^^^..................................x....x...x...x...x.xxxxxx...x...x...x...x...x...............xxx....xxx...~       ^^x.....xxx.........xxx^^       ^^     ^^^      ^      ^     ^^x...x...x...x.........~    ^^           ^xx....-----..........................x.......................^^      ^^^    x~...~...~~~x............~......................x         |",
);

static Level course03 = Level_FromLayout(
  "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |",
  "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |",
  "                                                                                                                                                                                                                                                        : :                                                                                                                                                                                                                                                                                                             :                                                                                                                                    |",
  "                                                                                                          :    :    :                                                                                                                                   xox                                                                                                                                          ----                                                                                                                                                               x                                                                                                                                    |",
  "                                                                                                          xooooxoooox                                                                                                                                   vvv                                                             ~~                                                                                ------                                                                                                    :       :       :                                   v                                                                  :       :       :                                                 |",
  "                                                                                                          vvvvvvvvvvv                                                                -                    ^                                                                                         i    ^          --      ~                               -        T                    ::      T                   ~                                                                                             x       x       x                                      ^                                                               x       x       x                                i ^              |",
  "                                                                         i                                                                                                       -                        ~              T                T                                                     i   x~~~~x                    ~                         -        ~   x                    xx      x              ~~     ~ T       i                                                      ^     ^                    v       v       v                                  ~~~~~~~~~~     ^                      ::             :  :           v       v       v          i      i   ---    i   x x         G    |",
  "                                       i                             i   x                 ^                                 ^           ^                      ^            -                         -             i   x            i   x             x                                   i   x   x    x --- T                ~~            ^     -                x                    vv      x                       ~    ~~~x                         -      -                   T ~     ~ ~                                                                 ~              ~~~~~       ^              xx             xoox                                  i   x  i   x          x   x                |",
  "                                   i   x             ~           i   x   x                 ~                                 ~           ~                 x~~~~x     ^  T               ~~~~      ~     -    ^  i   x   x     x  i   x   x         x   x                                   x   x   x    x     ~                      x~~~~~~~~~~x                   x                         x  x                               x                     -     --      T            i   ~ x   T x   ~~    T          x       x       x        ^        ^        ~                        ~~~~~~~              vv             vvvv           x       x       x      x   x  x   x          x   x      T         |",
  "               T^^^^      ^^     ^^x...x         T^^^^^^^      ^^x...x...x^^^    ^^    ^       ^          T   T   T      ^       ^^o^^^      T^^^^x~~~~x...x....x..x~~~~~x....................~~...........x~~x..x...x...x.....x..x...x...x.....x...x...x..x~~x^^^    ^    ^^    ^^x....x...x...x...x....x............................x..........x...................x^^^^^^     ^^    ^^      x  x...............................x^      ^^    ^x~~x.................x^^^^      ^^x.....x...x.x........xx^^^^     ^x      ^x      ^x~~~~~~~~~~~x..~~~~.~~~~...................................      ^   ^         ^    ^          ^^     ^x      ^x      ^x~~x...x...x..x...x..........x...x....~xx^^       |",
);

static Level course04 = Level_FromLayout(
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "               *    ",
    "             ,,,,,  ",
  );
#else
extern Level course01;
extern Level course02;
extern Level course03;
extern Level course04;
#endif

Level*
Level_GetById(LevelId id) {
  static Level *levels[LEVEL_COUNT] = {
    [LEVEL_EMPTY] = &course01,
    [LEVEL_STEREO_MADNESS] = &course02,
    [LEVEL_BACK_ON_TRACK] = &course03,
    [LEVEL_POLARGEIST] = &course04,
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
