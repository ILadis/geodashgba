
#include <game/level.h>

#ifdef NOGBA
static Level course01 = Level_FromLayout(
  "{n:Playground}               |",
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
  "{n:Stereo Madness}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |",
  "                                                                                                                                                                                                                                                : :                                                                                                                                                                                                                                                                                                          |",
  "                                                                                                                            --      --       ^^^^                                                            ^                                  xox                                                                                                                                                                                                                                                                                                          |",
  "                                                                                                               --          ^^^^    ^^^^      ~~~~         ^                                                  ~                                  vvv                       i                                                                                                                                                                           ^                                                                                                      |",
  "                                          *                                                                -      ____  xxxxxxxxxxxxxxx              ^xxxxx                                              ~                                                            i   x~~~                                                                 :      : :      :      :                                                                               x ^                                   : :                                                              |",
  "                                                        :                                              -                xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx^               ^                         ~     --------                                               i   x   x    --                                                              xo     xox      x      x                                                                                 x ^                                 xox                                                         G    |",
  "                                                       <x                                          -                    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  ~~~~~        ^            ~                   -                     i   i              i   x   x   x        --                                                          vv     vvv      v      v                   i                                                       ___     x ^                               vvv                                                              |",
  "                                      i                <x                                      -                        xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx         ~~~~~~~        ~                          -              i   x   x          i   x   x   x   x               ___                                                                                         i   x~                   :  :                           _       x     ~                                                                                      T         |",
  "                                  i   x                                          ^         -                                           v             v     v                           ----                                -           x   x   x          x   x   x   x   x               xxx    ___                            --                                                i   x   x   ~                xoox                       _             x          ^     ~~~                                          ~           ^      ^      ~~~x         |",
  "                              i   x   x                           ^        xxxxxxxxxxxxx                                                                                                                                      x~~~~x   x   x   x xxxxxx   x   x   x   x   x               xxx    xxx ~              --      --                                                    x   x   x      ~             vvvv                   _                   x      ~ ~ ~       -   ___                           x~~~      ~      ~ ~ ~~~~ ~ ~~      x         |",
  "             ^      ,^      ^^x...x...x       ^^   xxxxxxxx..xxxxxxxxxxx...xxxxxxxxxxxxx.......................o~~~~~~~~xxxxxxxxxxxxxxx~xxxxxxxxxxxxx~xxxxx~xxxx~~--__  ^^           T  ^^^^..................................x....x...x...x...x.xxxxxx...x...x...x...x...x...............xxx....xxx...~       ^^x.....xxx.........xxx^^       ^^     ^^^      ^      ^     ^^x...x...x...x.........~    ^^           ^xx....-----..........................x.......................^^      ^^^    x~...~...~~~x............~......................x.........|",
);

static Level course03 = Level_FromLayout(
  "{n:Back on Track}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |",
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
  "{n:Polargeist}                                                                                                                                                                                                                                                                                                                                                                                                           :                                                                                                                                                                                                                                                                                                                                                                                          |",
  "                                                                                                                                                                                                                                                                                                                                                                                                                    :   x                                                                                                                                                                                                                                                                                                                                                                                           |",
  "                                                                                                                                                                                                                                                                                                                                                                                                                :   x          :                                                ^                                                                           :                                                                                                                                         :        :                                                                                                    |",
  "                                                                                                                                                                                                                                                                                                                                                              :                                                 x              x :                                              x                                                                           x :                                                                                                                                       xoooooooox                                                                                                    |",
  "                                                                                                                                                                                                                                             :         :                                                                                                      x :                                                              v x :                                                                                                                        v x :                                                                                                                                     vvvvvvvvvv                                                                                                    |",
  "                                                                            ::   ::                                                                                                                                                         <x         x :                                                                                                    v x :                                                     x        v x :                                    ^ -                                                                                 v x :                                                                                                                :                                                                                                     ::                         |",
  "                                                                            xx   xx                                           :                                                         :  : :  :                                           <o         v x                                                                                                      v x :                                               x   x    x     v x :                                ~ ~              x                                                                o     v x :      :                     :                                                                           :     x                                ^                                                              :     xx                         |",
  "                                                                            vv   vv                                          <x                                                         xooxoxoox                                           <o           v                                                                    i                               _   v x :                           i             x   x   x    x x     v x                          ^ -          x~    @   x xx                                                               o     v x      x                    <x                                                                           x     v                  xxxxxxxxxxxxxxx>                                                             x     vv                         |",
  "                                                      @                                                                      <o                    i                                    vvvvvvvvv    ^     @                   @     T    @            _                                                                  @   x        i                        _   v x                       @   x       xxx   x   x   x    x x x     v                        ~ ~            x         x xx    x      x                                            T        o     v      v                    <o               T                                                           v                    x   xxxxxxxxxxxxxxx>                             :                               v                           G    |",
  "                                                  i        ____                               ^             ^    i   @    i  <o         _      _   x _                      ^                        x @        @   xxxx   @       xxx             @     _           -        _                 -      -       -      @       x xxxx   x        @          @      _   v                   i ,,,,, x  x~   xxx   x   x   x    x x x x                      ^ -                  x         x xx    x xx   x xx   @                                x    x          o              ^  @        xx        ^           x                                                                 x          x   x   xx           xx>          ^                 <x                                     x                   ^      |",
  "                 @           @           @    i   x                                       xxxxxxxxxxx       o    x        x         i      _       x   __                   o                        x              xxxx           xxx    ______           ___   @     ---      --  xxx _____     __     ___     ___          x xxxx   x    @        xxx            _                 @   x ~~~~~ x  x    xxx   x   x   x    x x x x x             i   ~  ~                    x         x xx    x xx   x xx        x  i   x  i             x   x    x            o        xxxxx      x~   xx  xxxxxxx       x   x             @         @                     x      x      x     x      x   x   x   xx           xx>     ^    x     @      ^    <o                 x  T          x     x  @   @          x x      |",
  "               ,,,,,       ,,,,,       ,,,,,,,x...x................o^^      ^^   ^^   T.................o       ^x........x       ^^x..............x.....^^^      ^^   ^^        ^     ^           T x..............xxxx...........xxx..............................................xxx.......................................x.xxxx...x.............xxx.............      ^   ^    ,,,,,,x.......x..x....xxx...x...x...x....x.x.x.x.x.x    ^    ^^x...........................x.........x.xx....x.xx...x.xx........x.~x...x.~x       ^^x...x...x....x..............o      xxxxx......x....xx..xxxxxxx...x...x...x,,,,,      ,,,,,     ,,,,,,   ^    ^   ^x~...x..x...x     ^x..x..x..x...x...x...x...xx...........xx>.xxxxx^       ,,,,,,xxxxxx       ^^    ^^xxx...x..x^^^^     ^x..x..x,,,,,,,,,,     ^^x.x......|",
);

static Level course05 = Level_FromLayout(
  "{n:Dry Out}                                                                                                                                                                                                                                  :                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |",
  "                                                                                                                                                                                                                                             x :                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |",
  "                                                                                                                                                                                                                                             v x :                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |",
  "                                                                                                                                                                                                                                               v x                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |",
  "                                                                                                                                                                                                                                      ~    -     v                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |",
  "                                                                                                                                                                                                                                             -                                 :                 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                                                                                                                                                                                                 :                                                                      |",
  "                                                                                                                                                                                                                            ~      ~           -      -      -     -           x :               x           x      x     v     vvx```xvvv     vvxvvv        v    vvx```x```x````x`x```x```x`````x      vv   x```x`````x   vvx```x````xvvvvv                                                                                                                                        :                       :                                x  :                                                                   |",
  "                                                                                    :                                                                   :                                                                                        --     --     --    -         v x :             x           x      x                 x                 ^               x   x    x     x   x        <o       x   x           x        x                                                                                                                                             x  :              :     x :                              v  x            :                        :                             |",
  "                                                                         :         <x                                                                  <x                            : :                                 ~     ~~                                      --        v x :           x                                                      o                   x              x        <o           x                                                 i                                                                        :        :                              v  x  :           x     v x                                 v            x :                      x                             |",
  "                                                                        <x         <o                                                                  <o     :         :            xox                                                                                     -     v x :         x           ^      ^                                                                                                                                              x                                                                       <x        x                                 v  x           v       v                                              v x                      v        x                    |",
  "                                                 :                      <o       i                           ^                                      :         x         x            vvv           :                  ~                                                        -     v x         x           o      o                                                                                                                                           i  x                :                                                      <o        v                                    v                                                  x                 v                               x x   ^        G     |",
  "                                  i             <x                   i           x                       --- x  x        i                      :   x         v         v                         <x               i                                                             -     v                                                                                                                                                C                       x  x               <x                              i                    x                     ^            x ^ x    x                       x                                x  x T                                         x  x x x x              |",
  "                     ^            x             <o        ^    ^ i   x           x                  ^~       x  x x      x                      x         ^                        ~xxxxx~     i  <o               x                                                               -                                                                                                                                                    xx                      x  x               <o                     @        x            @   x   x            x      x o x      x   x o x    x  x              x     x x           x      x        x  x  x x          x                    i   x     x  x x x x   T^^^^      |",
  "               ^^    x      ^^x...x....x     ^       ^    x    x.x...x      ^^x..x..x^^^       ^^   x........x..x.x.x....x^^^xx      ^^x....o.........xxxxxxxxxxxxx...xxxxx...xx.............xxx       ^    ^^    ^x.................................................................o           C                                                                                                                                      xx.x    ^^   ^     ^^x..x..x^^^^      ^^x      ^^    ^^x^^^  T,,,,,,,,    ^x  T^^^^   ,,,,,,x,,,x      ^    ^x.xx...x...x..x...x...x...x....x..x..x    ^^     x..x..x.x     ^     x..x...x^^^     x..x..x x^^^^^    ^x.x    ^^   ^^    ^^^x...x.xx..x..x.x.x.x..xxxxxx      |",
);
#else
extern Level course01;
extern Level course02;
extern Level course03;
extern Level course04;
extern Level course05;
#endif

Level*
Level_GetById(LevelId id) {
  static Level *levels[LEVEL_COUNT] = {
    [LEVEL_EMPTY] = &course01,
    [LEVEL_STEREO_MADNESS] = &course02,
    [LEVEL_BACK_ON_TRACK] = &course03,
    [LEVEL_POLARGEIST] = &course04,
    [LEVEL_DRY_OUT] = &course05,
  };

  Level *level = NULL;
  if (id < length(levels)) {
    level = levels[id];
  }

  return level;
}

void
Level_GetName(
    Level *level,
    char *name)
{
  extern void AsciiLevel_GetName(Level *level, char *name);
  extern void Binv1Level_GetName(Level *level, char *name);

  const void (*get[])(Level *level, char *name) = {
    [FORMAT_ASCII] = AsciiLevel_GetName,
    [FORMAT_BINV1] = Binv1Level_GetName,
  };

  return get[level->format](level, name);
}


void
Level_SetName(
    Level *level,
    char *name)
{
  extern void AsciiLevel_SetName(Level *level, char *name);
  extern void Binv1Level_SetName(Level *level, char *name);

  const void (*set[])(Level *level, char *name) = {
    [FORMAT_ASCII] = AsciiLevel_SetName,
    [FORMAT_BINV1] = Binv1Level_SetName,
  };

  return set[level->format](level, name);
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
