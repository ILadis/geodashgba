
#include <gba.h>
#include <everdrive.h>
#include <disk.h>
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
    extern void Debug_Print(char *message);
    extern void Debug_PrintLine(char *message);
    extern void Debug_PrintNewline();
    extern void Debug_PrintBuffer(unsigned char *buffer, int length);

    Everdrive_UnlockSystem();

    if (!Everdrive_CardInitialize()) {
      Debug_PrintNewline();
      Debug_PrintLine("diskInit failed");
      while(true);
    }

    Disk disk = {0};
    DiskEntry entry = {0};

    if (!Disk_Initialize(&disk, Everdrive_CardReadBlock)) {
      Debug_PrintNewline();
      Debug_PrintLine("Error initializing disk");
      while(true);
    }

    char *rootDir[] = { NULL };
    if (!Disk_OpenDirectory(&disk, rootDir)) {
      Debug_PrintNewline();
      Debug_PrintLine("Error opening root directory");
      while(true);
    }

    Debug_PrintNewline();
    Debug_PrintLine("Listing root directory:");
    while (Disk_ReadDirectory(&disk, &entry)) {
      Debug_Print(entry.name);
      Debug_PrintLine(entry.type == DISK_ENTRY_FILE ? " (f)" : " (d)");
    }

    char *gbasysDir[] = { "GBASYS     ", NULL };
    if (!Disk_OpenDirectory(&disk, gbasysDir)) {
      Debug_PrintNewline();
      Debug_PrintLine("Error opening GBASYS directory");
      while(true);
    }

    Debug_PrintNewline();
    Debug_PrintLine("Listing GBASYS directory:");
    while (Disk_ReadDirectory(&disk, &entry)) {
      Debug_Print(entry.name);
      Debug_PrintLine(entry.type == DISK_ENTRY_FILE ? " (f)" : " (d)");
    }

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
