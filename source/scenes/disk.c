
#include <gba.h>
#include <log.h>
#include <everdrive.h>
#include <disk.h>
#include <scene.h>

#include <game/level.h>
#include <game/course.h>

extern Logger* Debug_GetLogger();
static Disk sdcard = {0};

static void
Scene_DoEnter() {
  static bool once = true;

  if (once) {
    Everdrive_UnlockSystem();

    if (Everdrive_CardInitialize()) {
      Disk_Initialize(&sdcard, Everdrive_CardReadBlock);
    }

    once = false;
  }
}

static void
Scene_DoPlay() {
  extern const Scene *play;
  Scene *current = Scene_GetCurrent();

  if (current->next != play) {
    Scene_ReplaceWith(current, play);
  }
}

static void
Scene_DoExit() {
  Logger *logger = Debug_GetLogger();

  char *directory[] = { NULL };
  char *filename = "LEVEL   BIN";

  if (!Disk_OpenDirectory(&sdcard, directory)) {
    Logger_PrintLine(logger, "Error opening level directory");
    while (true);
  }

  DiskEntry entry = {0};
  do {
    if (!Disk_ReadDirectory(&sdcard, &entry)) {
      Logger_PrintLine(logger, "Error: level file not found");
      while (true);
    }
  } while (!DiskEntry_NameEquals(&entry, filename));

  DataSource *source = Disk_OpenFile(&sdcard, &entry);
  if (source == NULL) {
    Logger_PrintLine(logger, "Error opening level file");
    while (true);
  }

  static Binv1Level level = {0};
  Binv1Level_From(&level, source);

  Course *course = Course_GetInstance();
  Course_ResetAndLoad(course, &level.base);
}

const Scene *disk = &(Scene) {
  .enter = Scene_DoEnter,
  .play = Scene_DoPlay,
  .exit = Scene_DoExit,
};
