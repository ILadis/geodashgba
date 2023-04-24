
#include <everdrive.h>

extern void Debug_PrintLine(char *message);
extern void Debug_Print(char *message);
extern void Debug_PrintNewline();
extern void Debug_PrintHex8(unsigned char num);
extern void Debug_PrintHex16(unsigned short num);

static bool log = false;
static bool fast = false;

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
  };

  return &system;
}

void
Everdrive_UnlockSystem() {
  Everdrive_System *system = Everdrive_GetSystem();
  *(system->unlockKey) = 0x00A5;

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
    .speed = fast ? 1 : 0,
    .mode = mode,
    .awaitF0 = awaitF0 ? 1 : 0,
    .startF0 = startF0 ? 1 : 0,
  };

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

static int
Everdrive_CardReadData() {
  Everdrive_System *system = Everdrive_GetSystem();

  volatile Everdrive_CardData data = *(system->cardData);
  volatile Everdrive_DeviceStatus status = {0};

  do {
    status = *(system->deviceStatus);
  } while (status.cardBusy);

  return data.lowByte & 0x00FF;
}

static unsigned int
Everdrive_CardWriteData(unsigned int data) {
  Everdrive_System *system = Everdrive_GetSystem();
  system->cardData->value = data = (data | 0xFF00);

  volatile Everdrive_DeviceStatus status = {0};

  do {
    status = *(system->deviceStatus);
  } while (status.cardBusy);

  return data;
}

static unsigned int
Everdrive_CardReadCommand() {
  Everdrive_System *system = Everdrive_GetSystem();

  volatile Everdrive_CardCommand command = *(system->cardCommand);
  volatile Everdrive_DeviceStatus status = {0};

  do {
    status = *(system->deviceStatus);
  } while (status.cardBusy);

  if (log) Debug_PrintHex8(command.value);

  return command.value;
}

static unsigned int
Everdrive_CardWriteCommand(unsigned int command) {
  Everdrive_System *system = Everdrive_GetSystem();
  system->cardCommand->value = command = (command & 0x00FF);

  if (log) Debug_PrintHex8(command);

  volatile Everdrive_DeviceStatus status = {0};

  do {
    status = *(system->deviceStatus);
  } while (status.cardBusy);

  return command;
}

static void
Everdrive_CardSendCommand(
    Everdrive_CardCommands command,
    u32 argument)
{
  if (log) { Debug_PrintNewline(); Debug_Print(">"); }

  unsigned int checksum = 0;
  unsigned int commands[] = {
    command,
    argument >> 24,
    argument >> 16,
    argument >>  8,
    argument >>  0,
  };

  Everdrive_CardSetMode(EVERDRIVE_CARD_MODE8, false, false);

  bool tmp = log; log = false;
  Everdrive_CardWriteCommand(0xFF); log = tmp;

  for (int i = 0; i < length(commands); i++) {
    checksum ^= Everdrive_CardWriteCommand(commands[i]);

    for (int j = 0; j < 8; j++) {
      checksum <<= 1;
      if (checksum & (1 << 8)) {
        checksum ^= 0x12;
      }
    }
  }

  Everdrive_CardWriteCommand(checksum | 1);
}

static bool
Everdrive_CardReceiveResponse(
    Everdrive_CardResponse response,
    unsigned int *buffer)
{
  if (log) { Debug_Print("<"); }

  bool tmp = log; log = false;
  Everdrive_CardReadCommand(); log = tmp;
  Everdrive_CardSetMode(EVERDRIVE_CARD_MODE1, false, false);

  Everdrive_System *system = Everdrive_GetSystem();

  int attempts = 2048;
  while (true) {
    volatile Everdrive_CardControl control = *(system->cardControl);

    if ((control.value & 0b11000000) == 0) {
      Everdrive_CardSetMode(EVERDRIVE_CARD_MODE8, false, false);
      break;
    }
    else if (attempts-- < 0) {
      return false;
    }

    bool tmp = log; log = false;
    Everdrive_CardReadCommand(); log = tmp;
  }

  if (buffer == NULL) {
    static unsigned int fallback[32];
    buffer = fallback;
  }

  int index = 0;
  int length = response == EVERDRIVE_CARD_R2 ? 17 : 6;

  while (index < length-1) {
    buffer[index++] = Everdrive_CardReadCommand();
  }

  volatile Everdrive_CardControl control = *(system->cardControl);
  buffer[index++] = control.value & 0xFF;

  return true;
}

bool
Everdrive_CardInitialize() {
  // see: https://electronics.stackexchange.com/questions/602105/how-can-i-initialize-use-sd-cards-with-spi
  // and: https://luckyresistor.me/cat-protector/software/sdcard-2/
  // and: https://github.com/florianhumblot/Phat/blob/master/phat/SDDevice.cpp

  // slow mode
  fast = false;
  Everdrive_CardSetMode(EVERDRIVE_CARD_MODE8, false, false);

  unsigned int response[16] = {0};

  // wait for card
  for (int i = 0; i < 40; i++) {
    Everdrive_CardWriteData(0xFF);
  }

  log = true;
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD0, 0x00);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);

  // wait for card
  for (int i = 0; i < 40; i++) {
    Everdrive_CardWriteData(0xFF);
  }

  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD8, 0x1AA);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R7, response);

  if (response[0] != 0x01 || (response[3] & 1) != 1) {
    // unsupported card
    // return false;
  }

  int attempts = 2048;
  while (attempts-- > 0) {
    Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD55, 0x00);
    Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);
    if ((response[3] & 1) != 1) { log = false; continue; }

    Everdrive_CardSendCommand(EVERDRIVE_CARD_ACMD41, 0x40300000);
    Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);
    if ((response[1] & 128) == 0) { log = false; continue; }

    break;
  }

  if (attempts <= 0) {
    // return false;
  }

  log = true;
  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD2, 0x00);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R2, response);

  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD3, 0x00);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);

  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD7, 0x00);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, NULL);

  unsigned int rca = 0
    | ((response[1] & 0xFF) << 24)
    | ((response[2] & 0xFF) << 16)
    | ((response[3] & 0xFF) <<  8)
    | ((response[4] & 0xFF) <<  0);

  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD9, rca);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R2, response);

  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD7, rca);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);

  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD55, rca);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);

  Everdrive_CardSendCommand(EVERDRIVE_CARD_CMD6, 2);
  Everdrive_CardReceiveResponse(EVERDRIVE_CARD_R1, response);

  fast = true;
  Everdrive_CardSetMode(EVERDRIVE_CARD_MODE8, false, false);

  return true;
}

bool
Everdrive_CardReadBlock(unsigned int address, void *buffer) {
  extern void GBA_Memcpy(void *dst, const void *src, int size);

  Everdrive_System *system = Everdrive_GetSystem();

  fast = true;
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