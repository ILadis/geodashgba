
#include <gba.h>
#include <everdrive.h>
#include <disk.h>
#include <scene.h>

static void
Scene_DoPlay() {
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

const Scene *disk = &(Scene) {
  .enter = Scene_Noop,
  .play = Scene_DoPlay,
  .exit = Scene_Noop,
};
