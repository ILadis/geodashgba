#ifndef LOG_H
#define LOG_H

#include <types.h>

typedef struct Logger {
  bool (*enable)();
  void (*print)(const char *message);
} Logger;

#define Logger_CreateNew(print) ((Logger) { NULL, print })

Logger*
Logger_GetInstance();

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

void
Logger_PrintHex8(
    Logger *logger,
    unsigned char num);

void
Logger_PrintHex16(
    Logger *logger,
    unsigned short num);

void
Logger_PrintBuffer(
    Logger *logger,
    unsigned char *buffer,
    int length);

#endif
