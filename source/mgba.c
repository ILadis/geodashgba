
#include <log.h>

typedef enum mGBA_LogLevel {
  // mGBA cli log level mask:
  mGBA_LOG_FATAL = 0, // 1
  mGBA_LOG_ERROR = 1, // 2
  mGBA_LOG_WARN =  2, // 4
  mGBA_LOG_INFO =  3, // 8
  mGBA_LOG_DEBUG = 4, // 16
} mGBA_LogLevel;

static struct {
  volatile unsigned short *enable;
  volatile unsigned short *flags;
  char *message;
} mGBA_Debug = {
  .enable  = (volatile unsigned short*) 0x4FFF780,
  .flags   = (volatile unsigned short*) 0x4FFF700,
  .message = (char*) 0x4FFF600,
};

bool
mGBA_DebugEnable() {
  mGBA_Debug.enable[0] = 0xC0DE;
  return mGBA_Debug.enable[0] == 0x1DEA;
}

void
mGBA_DebugLog(const char* message) {
  mGBA_LogLevel level = mGBA_LOG_INFO;

  // details see: https://github.com/mgba-emu/mgba/tree/master/opt/libgba
  const int limit = 0x100;
  static int index = 0;

  for (int i = 0; i < limit; i++) {
    char symbol = message[i];

    switch (symbol) {
    case '\0': return;
    case '\n':
      mGBA_Debug.message[index] = '\0';
      mGBA_Debug.flags[0] = level | 0x100;
      index = 0;
      break;
    default:
      mGBA_Debug.message[index++] = symbol;
      break;
    }
  }
}
