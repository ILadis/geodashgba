
#include <game/level.h>

#ifdef NOGBA
static Level testCourse = Level_FromLayout(
  "                                                                                                                                                                                            ",
  "                                                                                                                                                                                            ",
  "                                                                                                                                                                                            ",
  "                                                                                                                            --      --       ^^^^                                           ",
  "                                                                                                               --          ^^^^    ^^^^      xxxx         ^                                 ",
  "                                                                                                           -      xxxx  xxxxxxxxxxxxxxx               xxxxx                                 ",
  "                                                        :                                              -                xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx ^              ^                ",
  "                                                       <x                                          -                    xxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxx xxxxxxxxx  ----x        ^       ",
  "                                    i                  <x                                      -                                        xxxxxxxxxxxxxxxxxxx                   ------x       ",
  "                               i    x                                            ^         -                                                                                          ----- ",
  "                          i    x    x                             ^        xxxxxxxxxxxxx                                xxxxxxxxxxxxxxx                                                     ",
  "_________^______^^_______^x____x____x_________^^___xxxxxxxx__xxxxxxxxxxx___xxxxxxxxxxxxx________________________________xxxxxxxxxxxxxxx_xxxxxxxxxxxxxxxxxxx_________________________________",
);
#else
extern Level testCourse;
#endif

Level*
Level_GetById(LevelId id) {
  static Level *levels[] = {
    [LEVEL_TEST_COURSE] = &testCourse,
  };

  return levels[id];
}
