
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

static void
Everdrive_LogCallback(Everdrive_CardCommand command) {
  if (log) {
    Debug_PrintHex8(command);
  }
}

Everdrive_System*
Everdrive_GetSystem() {
#ifdef NOGBA
  static char MEM_IO[0x00FF] = {0};
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
    .cardCallback = Everdrive_LogCallback,
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
  system->cardCallback(command);

  volatile Everdrive_DeviceStatus status = {0};

  do {
    status = *(system->deviceStatus);
  } while (status.cardBusy);

  return command;
}

static Everdrive_CardCommand
Everdrive_CardWriteCommand(Everdrive_CardCommand command) {
  Everdrive_System *system = Everdrive_GetSystem();
  *(system->cardCommand) = command = (command & 0x00FF);

  system->cardCallback(command);

  volatile Everdrive_DeviceStatus status = {0};

  do {
    status = *(system->deviceStatus);
  } while (status.cardBusy);

  return command;
}

static bool
Everdrive_CardReceiveResponse(
    Everdrive_CardCommand command,
    Everdrive_CardResponse *response)
{
  if (log) Debug_Print("<");

  Everdrive_System *system = Everdrive_GetSystem();

  withDisabledLog(Everdrive_CardReadCommand());
  Everdrive_CardSetMode(EVERDRIVE_CARD_MODE1, false, false);

  int attempts = 2048;
  while (true) {
    volatile Everdrive_CardControl control = *(system->cardControl);

    if (control.start == 0 && control.transmission == 0) break;
    if (attempts-- < 0) return false;

    withDisabledLog(Everdrive_CardReadCommand());
  }

  Everdrive_CardSetMode(EVERDRIVE_CARD_MODE8, false, false);

  int length = command == EVERDRIVE_CARD_CMD2 ? 17 : 6;
  int index = 0;

  while (index < length) {
    response->data[index++] = Everdrive_CardReadCommand();
  }

  return true;
}

static bool
Everdrive_CardSendCommand(
    Everdrive_CardCommand command,
    Everdrive_CardArgument argument,
    Everdrive_CardResponse *response)
{
  if (log) {
    Debug_PrintNewline();
    Debug_Print(">");
  }

  Everdrive_CardCommand checksum = 0;
  Everdrive_CardCommand commands[] = {
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

  if (response != NULL) {
    return Everdrive_CardReceiveResponse(command, response);
  }

  return true;
}

bool
Everdrive_CardInitialize() {
  Everdrive_CardResponse response;
  Everdrive_CardSetSpeed(EVERDRIVE_CARD_MODE8, EVERDRIVE_CARD_SPEED_SLOW);

  log = true;
  // CMD0 has no response
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD0, 0x00, NULL);

  // CMD8 has no response on v1.x cards
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD8, 0x1AA, &response);

  if (!response.R7.voltageAccepted) {
    // voltage not accepted or v1.x card
    return false;
  }

  int attempts = 2048;
  while (attempts-- > 0) {
    Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD55, 0x00, &response);
    if (!response.R1.acmdAccepted) continue;

    // indicate "high capacity support" (HCS) at 3.4-3.5V
    Everdrive_CardSendCommand(EVERDRIVE_CARD_ACMD41, 0x40300000, &response);
    if (!response.R3.poweredUp) continue;
    // TODO check card capacity bit

    break;
  }

  if (attempts <= 0) {
    return false;
  }

  // receive CID (card identification)
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD2, 0x00, &response);

  // receive RCA (relative card address)
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD3, 0x00, &response);

  Everdrive_CardArgument rca = (response.R6.rca1 << 24) | (response.R6.rca2 << 16);

  Everdrive_System *system = Everdrive_GetSystem();
  system->cardAddress = rca;

  // reached stand-by state, switch to transfer state
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD7, rca, &response);

  // sets bus width (use 4 bits for data line)
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD55, rca, &response);
  Everdrive_CardSendCommand(EVERDRIVE_CARD_ACMD6, 0x02, &response);

  Everdrive_CardSetSpeed(EVERDRIVE_CARD_MODE8, EVERDRIVE_CARD_SPEED_FAST);

  return true;
}

bool
Everdrive_CardReadBlock(unsigned int address, void *buffer) {
  extern void GBA_Memcpy(void *dst, const void *src, int size);

  Everdrive_System *system = Everdrive_GetSystem();
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD18, address, NULL);

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
