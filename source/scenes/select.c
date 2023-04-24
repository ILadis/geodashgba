
#include <gba.h>
#include <everdrive.h>
#include <scene.h>

#include <assets/graphics/tiles.h>
#include <assets/graphics/sprites.h>

#include <game/camera.h>
#include <game/course.h>
#include <game/level.h>
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
    GBA_Memcpy(&system->tileSets8[0][0], tilesTiles, tilesTilesLen);
    GBA_Memcpy(&system->backgroundPalette[0], tilesPal, tilesPalLen);

    // used for sprites
    GBA_Memcpy(&system->tileSets4[4][0], spritesTiles, spritesTilesLen);
    GBA_Memcpy(&system->spritePalette[0], spritesPal, spritesPalLen);

    mGBA_DebugEnable(true);
    once = false;
  }

  Selector *selector = Selector_GetInstance();
  Selector_SetVisible(selector, true);

  Level *level = Level_GetById(LEVEL_EMPTY);
  Course *course = Course_GetInstance();
  Course_ResetAndLoad(course, level);

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

  mGBA_DebugLog(mGBA_LOG_INFO, "Scene: select");
}

static void
Scene_DoPlay() {
  GBA_Input_PollStates();

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

  // TODO workaround to draw progress when selector box is out of screen bounds
  if (selector->redraw) {
    LevelId id = Selector_GetLevelId(selector);
    int best = Records_GetBestForLevel(records, id);
    Progress_SetProgress(progress, best);
  }

  if (GBA_Input_IsHit(GBA_KEY_SELECT)) {
    extern void Debug_PrintLine(char *message);
    extern void Debug_PrintNewline();

    Everdrive_UnlockSystem();

    //u8 diskInit();
    if (!Everdrive_CardInitialize() /*diskInit() != 0*/) {
      Debug_PrintNewline();
      Debug_PrintLine("diskInit failed");
      while(true);
    }

    unsigned char buffer[512];
    //u8 diskRead(u32 sd_addr, u8 *dst, u16 slen);
    if (!Everdrive_CardReadBlock(0x2000, buffer) /*diskRead(0x2000, buffer, 1) != 0*/) {
      Debug_PrintNewline();
      Debug_PrintLine("Error reading from card");
      while(true);
    }

    Debug_PrintLine((char *) &buffer[3]); //MSDOS...
    Debug_PrintLine((char *) &buffer[0x47]); //print partition name
    Debug_PrintLine((char *) &buffer[0x1AE]); //disk error message
    Debug_PrintLine((char *) &buffer[0x1AE + 30]);
    Debug_PrintLine((char *) &buffer[0x1AE + 60]);

    while(true);
  }

  GBA_VSync();

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
