#ifndef LOG_H
#define LOG_H

typedef struct Logger {
  void (*print)(const char *message);
} Logger;

static inline void
Logger_Print(Logger *logger, const char *message) {
  logger->print(message);
}

static inline void
Logger_PrintNewline(Logger *logger) {
  logger->print("\n");
}

static inline void
Logger_PrintLine(Logger *logger, const char *message) {
  Logger_Print(logger, message);
  Logger_PrintNewline(logger);
}

static inline void
Logger_PrintHex8(Logger *logger, unsigned char num) {
  static char digits[] = "0123456789ABCDEF";
  char message[3] = {0};

  int upper = num >> 4;
  message[0] = digits[upper];

  int lower = num & 0x0f;
  message[1] = digits[lower];

  Logger_Print(logger, message);
}

static inline void
Logger_PrintHex16(Logger *logger, unsigned short num) {
  Logger_PrintHex8(logger, num >> 8);
  Logger_PrintHex8(logger, num & 0xff);
}

static inline void
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

#endif
