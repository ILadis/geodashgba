
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

    GBA_EnableInterrupt(GBA_IRQ_VBLANK);
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

    const SoundSampler *sampler = SineSoundSampler_GetInstance();

    static const char notes1[] = "E` | E` B/2  C`/2 D`/2 E`/4 D`/4 C`/2 B/2  | A A/2 C`/2 E`  D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z            | Z/2 D`/2 F`/2 A` G`/2 F`/2 | E`. C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z | E` B/2  C`/2 D` C`/2 B/2 | A A/2 C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z            | Z/2 D`/2 F`/2 A` G`/2 F`/2 | E`. C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z | E2 C2  | D2  B,2  | C2  A,2 | G^,2 B,2  | E2 C2  | D2  B,2  | C  E A A | G^4 | E` B/2  C`/2 D` C`/2 B/2 | A A/2 C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A Z            | Z/2 D`/2 F`/2 A` G`/2 F`/2 | E`. C`/2 E` D`/2 C`/2 | B  B/2  C`/2 D` E` | C` A A2 |";
    static const char notes2[] = "B  | Z  G^/2 Z/2  Z              A/2  G^/2 | E Z        Z2            | G^ G^/2 A/2  B  Z  | Z2 Z/2 A/2 B/2 C`/2 | Z/2 F/2  A/2  C` B/2  A/2  | G.  E/2  G  F/2  E/2  | G^ G^/2 A/2  B  Z  | Z4       | Z  G^/2 Z/2  Z  A/2  G/2 | E Z        Z2           | G^ G^/2 A/2  B  Z  | Z2 Z/2 A/2 B/2 C`/2 | Z/2 F/2  A/2  C` B/2  A/2  | G.  E/2  G  F/2  E/2  | G^ G^/2 A/2  B  Z  | Z4       | C2 A,2 | B,2 G^,2 | A,2 D,2 | D,2  G^,2 | C2 A,2 | B,2 G^,2 | A, C E E | E4  | Z  G^/2 Z/2  Z  A/2  G/2 | E Z        Z2           | G^ G^/2 A/2  B  Z  | Z2 Z/2 A/2 B/2 C`/2 | Z/2 F/2  A/2  C` B/2  A/2  | G.  E/2  G  F/2  E/2  | G^ G^/2 A/2  B  Z  | Z4      |";

    static SoundTrack track1 = {0};
    SoundTrack_AssignNotes(&track1, notes1, 2800);

    static SoundTrack track2 = {0};
    SoundTrack_AssignNotes(&track2, notes2, 2800);

    static SoundChannel channel1 = {0};
    SoundChannel_SetTrackAndSampler(&channel1, &track1, sampler, 13);

    static SoundChannel channel2 = {0};
    SoundChannel_SetTrackAndSampler(&channel2, &track2, sampler, 13);

    SoundPlayer_AddChannel(player, &channel1);
    SoundPlayer_AddChannel(player, &channel2);

    once = false;
  }

  Selector *selector = Selector_GetInstance();
  Selector_SetVisible(selector, true);

  const Collection *collection = Collection_GetInstance();
  Binv1Level *level = Collection_GetLevelByIndex(collection, 0);

  Course *course = Course_GetInstance();
  Course_ResetAndLoad(course, &level->base);
  while (!Course_AwaitReadyness(course));

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
  SoundPlayer_VSync(player);
  GBA_VSyncWait();

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
  while (!Course_AwaitReadyness(course));
}

const Scene *entry = &(Scene) {
  .enter = Scene_DoEnter,
  .play = Scene_DoPlay,
  .exit = Scene_DoExit,
};
