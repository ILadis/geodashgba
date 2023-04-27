
#include <everdrive.h>

extern void Debug_PrintLine(char *message);
extern void Debug_Print(char *message);
extern void Debug_PrintNewline();
extern void Debug_PrintHex8(unsigned char num);
extern void Debug_PrintHex16(unsigned short num);

static bool log = false;

#define withDisabledLog(block) \
do { \
  bool state = log; \
  log = false; \
  block; \
  log = state; \
} while(0);

Everdrive_System*
Everdrive_GetSystem() {
#ifdef NOGBA
  static char MEM_IO[0x00B5] = {0};
#else
  const int MEM_IO = EVERDRIVE_MEM_IO;
#endif

  static Everdrive_System system = {
    .unlockKey = EVERDRIVE_UNLOCK_KEY(MEM_IO + 0x00B4),

    .deviceControl = EVERDRIVE_DEVICE_CONTROL(MEM_IO + 0x0000),
    .deviceStatus  = EVERDRIVE_DEVICE_STATUS(MEM_IO + 0x0002),

    .cardCommand = EVERDRIVE_CARD_COMMAND(MEM_IO + 0x0010),
    .cardData    = EVERDRIVE_CARD_DATA(MEM_IO + 0x0012),
    .cardControl = EVERDRIVE_CARD_CONTROL(MEM_IO + 0x0014),

    .cardSpeed = EVERDRIVE_CARD_SPEED_SLOW,
  };

  return &system;
}

void
Everdrive_UnlockSystem() {
  Everdrive_System *system = Everdrive_GetSystem();
  *(system->unlockKey) = 0x00A5; // "magic" unlock key

  Everdrive_DeviceControl control = {
    .mapRegisters = 1,
    .mapPSRAM = 1,
  };

  system->deviceControl->value = control.value;
}

static void
Everdrive_CardSetMode(
    Everdrive_CardMode mode,
    bool awaitF0, bool startF0)
{
  Everdrive_System *system = Everdrive_GetSystem();
  Everdrive_CardControl control = {
    .speed = system->cardSpeed,
    .mode = mode,
    .awaitF0 = awaitF0 ? 1 : 0,
    .startF0 = startF0 ? 1 : 0,
  };

  system->cardControl->value = control.value;
}

static void
Everdrive_CardSetSpeed(
    Everdrive_CardMode mode,
    Everdrive_CardSpeed speed)
{
  Everdrive_System *system = Everdrive_GetSystem();
  Everdrive_CardControl control = {
    .speed = speed,
    .mode = mode,
  };

  system->cardSpeed = speed;
  system->cardControl->value = control.value;
}

static int
Everdrive_CardAwaitF0() {
  Everdrive_System *system = Everdrive_GetSystem();
  Everdrive_CardSetMode(EVERDRIVE_CARD_MODE4, true, true);

  int attempts = 65000;
  while (attempts-- > 0) {
    volatile Everdrive_CardData data = *(system->cardData);
    volatile Everdrive_DeviceStatus status = {0};

    do {
      status = *(system->deviceStatus);
    } while (status.cardBusy);

    if (!status.cardTimeout) {
      return data.lowByte & 0x00FF;
    }

    Everdrive_CardSetMode(EVERDRIVE_CARD_MODE4, true, false);
  }

  return -1;
}

static Everdrive_CardCommand
Everdrive_CardReadCommand() {
  Everdrive_System *system = Everdrive_GetSystem();

  volatile Everdrive_CardCommand command = *(system->cardCommand) & 0x00FF;
  volatile Everdrive_DeviceStatus status = {0};

  do {
    status = *(system->deviceStatus);
  } while (status.cardBusy);

  if (log) Debug_PrintHex8(command);

  return command;
}

static Everdrive_CardCommand
Everdrive_CardWriteCommand(Everdrive_CardCommand command) {
  Everdrive_System *system = Everdrive_GetSystem();
  *(system->cardCommand) = command = (command & 0x00FF);

  if (log) Debug_PrintHex8(command);

  volatile Everdrive_DeviceStatus status = {0};

  do {
    status = *(system->deviceStatus);
  } while (status.cardBusy);

  return command;
}

static void
Everdrive_CardSendCommand(
    Everdrive_CardCommand command,
    Everdrive_CardArgument argument)
{
  if (log) {
    Debug_PrintNewline();
    Debug_Print(">");
  }

  unsigned int checksum = 0;
  unsigned int commands[] = {
    command,
    argument >> 24,
    argument >> 16,
    argument >>  8,
    argument >>  0,
  };

  Everdrive_CardSetMode(EVERDRIVE_CARD_MODE8, false, false);
  withDisabledLog(Everdrive_CardWriteCommand(0xFF));

  for (int i = 0; i < length(commands); i++) {
    checksum ^= Everdrive_CardWriteCommand(commands[i]);

    for (int j = 0; j < 8; j++) {
      checksum <<= 1;
      if (checksum & (1 << 8)) {
        checksum ^= 0x12;
      }
    }
  }

  // TODO why no << 1 required?
  Everdrive_CardWriteCommand(checksum | 1);
}

static bool
Everdrive_CardReceiveResponse(
    Everdrive_CardResponse response,
    unsigned int *buffer)
{
  if (log) Debug_Print("<");

  withDisabledLog(Everdrive_CardReadCommand());
  Everdrive_CardSetMode(EVERDRIVE_CARD_MODE1, false, false);

  Everdrive_System *system = Everdrive_GetSystem();

  int attempts = 2048;
  while (true) {
    volatile Everdrive_CardControl control = *(system->cardControl);

    if (control.start == 0 && control.transmission == 0) break;
    if (attempts-- < 0) return false;

    withDisabledLog(Everdrive_CardReadCommand());
  }

  Everdrive_CardSetMode(EVERDRIVE_CARD_MODE8, false, false);

  if (buffer == NULL) {
    static unsigned int fallback[32];
    buffer = fallback;
  }

  int length = response == EVERDRIVE_CARD_R2 ? 17 : 6;
  int index = 0;

  while (index < length) {
    buffer[index++] = Everdrive_CardReadCommand();
  }

  return true;
}

bool
Everdrive_CardInitialize() {
  unsigned int response[32] = {0};
  Everdrive_CardSetSpeed(EVERDRIVE_CARD_MODE8, EVERDRIVE_CARD_SPEED_SLOW);

  log = true;
  // CMD0 has no response
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD0, 0x00);

  // CMD8 has no response on v1.x cards
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD8, 0x1AA);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R7, response);

  //           v-- command index      v-- voltage bits
  if (response[0] != 0x08 || response[3]  != 1) {
    // voltage not accepted or v1.x card
    return false;
  }

  int attempts = 2048;
  while (attempts-- > 0) {
    Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD55, 0x00);
    Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);
    //            v-- card status (ACMD accepted)
    if ((response[3] & 1) != 1) { log = false; continue; }

    //                                         HCS=1 --v v-- voltage 3.4-3.5V
    Everdrive_CardSendCommand(EVERDRIVE_CARD_ACMD41, 0x40300000); // is OCR register
    Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R3, response);
    //            v----v-- card busy (must be 1) then check CCS (response[1] & 64 == 1)
    if ((response[1] & 128) == 0) { log = false; continue; }

    break;
  }

  if (attempts <= 0) {
    return false;
  }

  log = true;
  // receive CID (card identification)
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD2, 0x00);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R2, response);

  // receive RCA (relative address register)
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD3, 0x00);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R6, response);

  unsigned int rca = 0
    // rca bits
    | ((response[1] & 0xFF) << 24)
    | ((response[2] & 0xFF) << 16);

  // reached stand-by state, switch to transfer state
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD7, rca);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);

  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD55, rca);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);

  // sets bus width (use 4 bits for data line)
  Everdrive_CardSendCommand(EVERDRIVE_CARD_ACMD6, 0x2);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);

  Everdrive_CardSetSpeed(EVERDRIVE_CARD_MODE8, EVERDRIVE_CARD_SPEED_FAST);

  return true;
}

bool
Everdrive_CardReadBlock(unsigned int address, void *buffer) {
  extern void GBA_Memcpy(void *dst, const void *src, int size);

  Everdrive_System *system = Everdrive_GetSystem();
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD18, address);

// while (slen > 0)...
  if (Everdrive_CardAwaitF0() == -1) {
    return false;
  }

  GBA_Memcpy(buffer, system->cardData, 512);

  // TODO need to cancel OP (CMD18) when new read is received

// buffer += 512
// end while

  return true;
}
