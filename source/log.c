
#include <log.h>

static bool
Logger_NoopEnable() { return true; }

static void
Logger_NoopLog(unused const char* message) { return; }

extern bool mGBA_DebugEnable();
extern void mGBA_DebugLog(const char* message);

#ifdef NOGBA
#include <stdio.h>

static void
LibC_DebugLog(const char* message) {
  fprintf(stderr, "%s", message);
}

static Logger interfaces[] = {
  { Logger_NoopEnable, LibC_DebugLog },
  { Logger_NoopEnable, Logger_NoopLog },
};
#else
static Logger interfaces[] = {
  { mGBA_DebugEnable, mGBA_DebugLog },
  { Logger_NoopEnable, Logger_NoopLog },
};
#endif

Logger*
Logger_GetInstance() {
  static Logger *logger = NULL;

  if (logger == NULL) {
    for (unsigned int i = 0; i < sizeof(interfaces); i++) {
      logger = &interfaces[i];
      if (logger->enable()) break;
    }
  }

  return logger;
}

void
Logger_PrintHex8(Logger *logger, unsigned char num) {
  char message[3] = {0};

  message[0] = hexof(num, 1);
  message[1] = hexof(num, 0);

  Logger_Print(logger, message);
}

void
Logger_PrintHex16(Logger *logger, unsigned short num) {
  Logger_PrintHex8(logger, num >> 8);
  Logger_PrintHex8(logger, num >> 0);
}

void
Logger_PrintHex32(
    Logger *logger,
    unsigned int num)
{
  Logger_PrintHex8(logger, num >> 24);
  Logger_PrintHex8(logger, num >> 16);
  Logger_PrintHex8(logger, num >> 8);
  Logger_PrintHex8(logger, num >> 0);
}

void
Logger_PrintBuffer(
    Logger *logger,
    unsigned char *buffer,
    unsigned int length)
{
  unsigned int index = 0, cols = 0;
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
