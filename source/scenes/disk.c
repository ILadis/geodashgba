
#include <gba.h>
#include <log.h>
#include <everdrive.h>
#include <disk.h>
#include <scene.h>

extern Logger* Debug_GetLogger();

static void
Scene_DoPlay() {
  Logger *logger = Debug_GetLogger();

  Everdrive_UnlockSystem();

  if (!Everdrive_CardInitialize()) {
    Logger_PrintNewline(logger);
    Logger_PrintLine(logger, "diskInit failed");
    while(true);
  }

  Disk disk = {0};
  DiskEntry entry = {0};

  if (!Disk_Initialize(&disk, Everdrive_CardReadBlock)) {
    Logger_PrintNewline(logger);
    Logger_PrintLine(logger, "Error initializing disk");
    while(true);
  }

  char *rootDir[] = { NULL };
  if (!Disk_OpenDirectory(&disk, rootDir)) {
    Logger_PrintNewline(logger);
    Logger_PrintLine(logger, "Error opening root directory");
    while(true);
  }

  Logger_PrintNewline(logger);
  Logger_PrintLine(logger, "Listing root directory:");
  while (Disk_ReadDirectory(&disk, &entry)) {
    Logger_Print(logger, entry.name);
    Logger_PrintLine(logger, entry.type == DISK_ENTRY_FILE ? " (f)" : " (d)");
  }

  char *gbasysDir[] = { "GBASYS     ", NULL };
  if (!Disk_OpenDirectory(&disk, gbasysDir)) {
    Logger_PrintNewline(logger);
    Logger_PrintLine(logger, "Error opening GBASYS directory");
    while(true);
  }

  Logger_PrintNewline(logger);
  Logger_PrintLine(logger, "Listing GBASYS directory:");
  while (Disk_ReadDirectory(&disk, &entry)) {
    Logger_Print(logger, entry.name);
    Logger_PrintLine(logger, entry.type == DISK_ENTRY_FILE ? " (f)" : " (d)");
  }

  while(true);
}

const Scene *disk = &(Scene) {
  .enter = Scene_Noop,
  .play = Scene_DoPlay,
  .exit = Scene_Noop,
};
