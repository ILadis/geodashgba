
#include <gba.h>
#include <log.h>
#include <sound.h>
#include <scene.h>

#include <game/assets.h>
#include <game/camera.h>
#include <game/course.h>
#include <game/level.h>
#include <game/collection.h>
#include <game/progress.h>
#include <game/selector.h>
#include <game/records.h>

static void
Scene_DoEnter() {
  static bool once = true;

  if (once) {
    GBA_System *system = GBA_GetSystem();

    GBA_Sprite_ResetAll();
    GBA_EnableSprites();

    // used for tilemaps
    GBA_Memcpy32(&system->tileSets8[0][0], tilesTiles, sizeof(tilesTiles));
    GBA_Memcpy32(&system->backgroundPalette[0], tilesPal, sizeof(tilesPal));

    // used for sprites
    GBA_Memcpy32(&system->tileSets4[4][0], spritesTiles, sizeof(spritesTiles));
    GBA_Memcpy32(&system->spritePalette[0], spritesPal, sizeof(spritesPal));

    SoundPlayer *player = SoundPlayer_GetInstance();
    SoundPlayer_Enable(player);

    static char notes1[] = "C`. B/2 A G | A F E D | G Z B2 | C Z2 G/2 G/2 | G G Z G^/2 G^/2 | A A Z G/2 G/2 | G C` B A | B2. G/2 G/2 | G G Z G^/2 G^/2 | A A Z G/2 G/2 | G F` E` D^` | E`2. G/2 G/2 | G G Z G^/2 G^/2 | A A Z G/2 G/2 | G C` B A | B2 Z D`/2 D`/2 | D` C` Z C`/2 C`/2 | C` B Z B/2 B/2 | A C` B  A  | G G/2 G/2 G G | E`2. B  | C` C` G E | F. G/2 G G | G4 | F`2. B | C` B A F | E. G/2 G G | G4 | E`2. G | B_ B_ B_ B_ | A. D`/2 D` D` | D^`2. D^` | E` C`/2 B/2 A G | F E D C | A Z B Z | C` C`/2 C`/2 C` Z | C2 F C^ | D2 F2 | E4 | B_4 | C2 E C^ | D2 E2 | C4 | A4 | A` Z A` A` | A`  Z A` A` | B_`2 B_`. B_`/2 | B_`4 | B_`2 B_`. B_`/2 | B_`4 | A`2 A`. A`/2 | A`4 | C`2 F` C^` | D`2 F`2 | E`4 | B_`4 | C`2 E` C^` | D`2  E`2 | C`4 | A`4 | Z A`2 A` | A`2 A` A`  | C``2. F` | B_`2 F`2  | F`2. C` | G`2. C | F` Z E` D`/2 E`/2 | F` F`/2 F`/2 F` Z |";
    static char notes2[] = "G.  Z2 Z/2  | F Z2 Z  | E Z Z2 | Z Z2 E/2 E/2 | E E Z E/2  E/2  | E E Z E/2 E/2 | E E  E E | F2. F/2 F/2 | F F Z F/2  F/2  | F F Z2        | Z A  G  F^  | G2.  E/2 E/2 | E E Z E/2  E/2  | E E Z E/2 E/2 | E E  E E | G2 Z F^/2 F^/2 | F^ F^ Z F/2  F/2  | F  E Z Z       | Z Z  F^ F^ | Z4            | C`2. G^ | A  G  E C | B. F/2 F F | F4 | B2.  G | A  G F D | C. E/2 E E | E4 | C`2. Z | G  G  G  G  | F. A/2  A  A  | A2.   A   | C` Z2.          | Z4      | F Z G Z | G  G/2  G/2  G  Z | Z4      | Z4    | Z4 | Z4  | Z4      | Z4    | Z4 | Z4 | F` Z E` D` | C^` Z D` F` | E`2  C^`. D`/2  | D`4  | E`2  C^`. D`/2  | D`4  | D`2 B.  C`/2 | C`4 | A2  A  A   | A2  A2  | B_4 | E`4  | B_2 B_ B_  | B_2  B_2 | A4  | C`4 | Z F`2 F` | F`2 E` E_` | F`2.  D` | G`2  D_`2 | D`2. Z  | E`2. Z | C` Z2.            | Z  C`/2 C`/2 C` Z |";
    static char notes3[] = "E`. Z2 Z/2  | Z4      | Z4     | Z4           | Z4              | Z4            | Z4       | Z4          | Z4              | Z4            | Z4          | Z4           | Z4              | Z4            | Z4       | Z4             | Z4                | Z4             | Z4         | Z4            | Z4      | Z4        | Z4         | Z4 | Z4     | Z4       | Z4         | Z4 | Z4     | Z4          | Z4            | Z4        | G  Z2.          | Z4      | Z2  F Z | E  E/2  E/2  E  Z | Z4      | Z4    | Z4 | Z4  | Z4      | Z4    | Z4 | Z4 | Z4         | Z4          | Z4              | Z4   | Z4              | Z4   | Z4           | Z4  | Z4         | Z4      | Z4  | Z4   | Z4         | Z4       | Z4  | Z4  | Z4       | Z4         | D`2.  Z  | D`2  Z2   | C`2. Z  | B_2. Z | A  Z2.            | Z  A/2  A/2  A  Z |";

    static NoteSoundChannel channel1 = {0};
    static NoteSoundChannel channel2 = {0};
    static NoteSoundChannel channel3 = {0};

    SoundPlayer_AddChannel(player, NoteSoundChannel_Create(&channel1, notes1, 13));
    SoundPlayer_AddChannel(player, NoteSoundChannel_Create(&channel2, notes2, 13));
    SoundPlayer_AddChannel(player, NoteSoundChannel_Create(&channel3, notes3, 13));

    once = false;
  }

  Selector *selector = Selector_GetInstance();
  Selector_SetVisible(selector, true);

  const Collection *collection = Collection_GetInstance();
  Binv1Level *level = Collection_GetLevelByIndex(collection, 0);

  Course *course = Course_GetInstance();
  Course_ResetAndLoad(course, &level->base);

  Progress *progress = Progress_GetInstance();
  Progress_SetMode(progress, MODE_SELECT);
  Progress_SetCourse(progress, course);
  Progress_SetProgress(progress, 0);

  Camera *camera = Camera_GetInstance();
  Camera_Reset(camera);

  const Vector *spawn = Course_GetSpawn(course);
  Camera_FollowTarget(camera, spawn);

  const Bounds *bounds = Course_GetBounds(course);
  Vector limit = Bounds_Upper(bounds);
  Camera_SetUpperLimit(camera, &limit);

  Camera_Update(camera);

  Logger *logger = Logger_GetInstance();
  Logger_PrintLine(logger, "Scene: select");
}

static void
Scene_DoPlay() {
  GBA_Input_PollStates();

  SoundPlayer *player = SoundPlayer_GetInstance();
  Camera *camera = Camera_GetInstance();
  Course *course = Course_GetInstance();
  Selector *selector = Selector_GetInstance();
  Progress *progress = Progress_GetInstance();
  Records *records = Records_GetInstance();

  Camera_Update(camera);
  Selector_Update(selector);

  if (GBA_Input_IsHit(GBA_KEY_LEFT)) {
    Selector_GoBackward(selector);
  }
  else if (GBA_Input_IsHit(GBA_KEY_RIGHT)) {
    Selector_GoForward(selector);
  }

  // update draw progress when selector box is out of screen bounds
  if (selector->redraw) {
    LevelId id = Selector_GetLevelId(selector);

    int best = Records_GetBestForLevel(records, id);
    Progress_SetProgress(progress, best);

    const bool *coins = Records_GetCollectedCoinsForLevel(records, id);
    Progress_SetCollectedCoins(progress, coins);
  }

  SoundPlayer_MixChannels(player);
  GBA_VSync();
//SoundPlayer_VSync(player);

  Course_Draw(course, camera);
  Selector_Draw(selector);
  Progress_Draw(progress);

  if (GBA_Input_IsHit(GBA_KEY_A)) {
    extern const Scene *play;
    Scene *current = Scene_GetCurrent();
    Scene_FadeReplaceWith(current, play);
  }
}

static void
Scene_DoExit() {
  Camera *camera = Camera_GetInstance();
  Camera_Reset(camera);

  Selector *selector = Selector_GetInstance();
  Selector_SetVisible(selector, false);

  Level *level = Selector_GetLevel(selector);

  Course *course = Course_GetInstance();
  Course_ResetAndLoad(course, level);
}

const Scene *entry = &(Scene) {
  .enter = Scene_DoEnter,
  .play = Scene_DoPlay,
  .exit = Scene_DoExit,
};
