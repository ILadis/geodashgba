
#include <gba.h>
#include <log.h>
#include <everdrive.h>
#include <disk.h>
#include <scene.h>

#include <game/level.h>
#include <game/course.h>

extern Logger* Debug_GetLogger();

static void
Scene_DoPlay() {
  //Disk sdcard = {0};
  //Disk_Initialize(&sdcard, Everdrive_CardReadBlock);

  //Everdrive_UnlockSystem();
  //Everdrive_CardInitialize();

  unsigned char buffer[512];
  const unsigned int sizes[] = {
    1024*2,     //   1MB ->  1.231s
    1024*2*32,  //  32MB ->  8.173s
    1024*2*64,  //  64MB -> 15.345s
    1024*2*128, // 128MB -> 29.690s
  };

  for (unsigned int i = 0; i < length(sizes); i++) {
    const unsigned int size = sizes[i];
    unsigned int sector = 0;

    GBA_TimerData overflows[] = { {-16}, {-1024}, {0} };
    GBA_StartTimerCascade(GBA_TIMER_FREQUENCY_1024, overflows);

    while (sector < size) {
      if (!Everdrive_CardReadBlock(sector++, buffer)) {
        break;
      }
    }

    int seconds = GBA_GetTimerValue(2, overflows);
    int millis = GBA_GetTimerValue(1, overflows);

    Logger *logger = Debug_GetLogger();

    Logger_Print(logger, "Read ");
    Logger_PrintHex32(logger, sector);
    Logger_PrintLine(logger, " sectors:");

    Logger_Print(logger, "  0x");
    Logger_PrintHex16(logger, seconds);
    Logger_Print(logger, ".");
    Logger_PrintHex16(logger, millis);
    Logger_PrintLine(logger, " seconds");
  }

  while (true);
}

const Scene *disk = &(Scene) {
  .enter = Scene_Noop,
  .play = Scene_DoPlay,
  .exit = Scene_Noop,
};
