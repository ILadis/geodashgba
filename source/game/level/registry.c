
#include <game/level.h>

#ifdef NOGBA
static Level testCourse = Level_FromLayout(
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
extern Level testCourse;
#endif

Level*
Level_GetById(LevelId id) {
  static Level *levels[] = {
    [LEVEL_TEST_COURSE] = &testCourse,
  };

  return levels[id];
}
