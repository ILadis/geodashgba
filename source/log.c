
#include <log.h>

static bool
Logger_NoopEnable() { return true; }

static void
Logger_NoopLog(const char* message) { return; }

extern bool mGBA_DebugEnable();
extern void mGBA_DebugLog(const char* message);

static Logger interfaces[] = {
  { mGBA_DebugEnable, mGBA_DebugLog },
  { Logger_NoopEnable, Logger_NoopLog },
};

Logger*
Logger_GetInstance() {
  static Logger *logger = NULL;

  if (logger == NULL) {
    for (int i = 0; i < sizeof(interfaces); i++) {
      logger = &interfaces[i];
      if (logger->enable()) break;
    }
  }

  return logger;
}

void
Logger_PrintHex8(Logger *logger, unsigned char num) {
  static char digits[] = "0123456789ABCDEF";
  char message[3] = {0};

  int upper = num >> 4;
  message[0] = digits[upper];

  int lower = num & 0x0f;
  message[1] = digits[lower];

  Logger_Print(logger, message);
}

void
Logger_PrintHex16(Logger *logger, unsigned short num) {
  Logger_PrintHex8(logger, num >> 8);
  Logger_PrintHex8(logger, num & 0xff);
}

void
Logger_PrintBuffer(
    Logger *logger,
    unsigned char *buffer,
    int length)
{
  int index = 0, cols = 0;
  while (index < length) {
    Logger_PrintHex8(logger, buffer[index++]);

    if (index % 2 == 0) {
      Logger_Print(logger, " ");
      cols++;

      if (cols == 5) {
        Logger_PrintNewline(logger);
        cols = 0;
      }
    }
  }
}