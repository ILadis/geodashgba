#ifndef EVERDRIVE_H
#define EVERDRIVE_H

#include "types.h"

#define EVERDRIVE_MEM_IO 0x09FC0000

#define EVERDRIVE_REG_DEVCNT  *((volatile u16 *) (EVERDRIVE_MEM_IO+0x0000))
#define EVERDRIVE_REG_DEVSTAT *((volatile u16 *) (EVERDRIVE_MEM_IO+0x0002))
#define EVERDRIVE_REG_CARDCNT *((volatile u16 *) (EVERDRIVE_MEM_IO+0x0010))
#define EVERDRIVE_REG_UNLOCK  *((volatile u16 *) (EVERDRIVE_MEM_IO+0x00B4))

typedef u16 Everdrive_UnlockKey;

typedef union Everdrive_DeviceControl {
  u16 value;
  struct {
    u16 mapRegisters: 1;
    u16 mapPSRAM: 1;
    u16 enablePSRAMWrite: 1;
    // Used for DMA'ing from ROM (0=Off, 1=On)
    u16 enableAutoWrite: 1;
    // Select which save type to use (0=None, 1=EEPROM, 2=SRAM, 4=Flash 64K, 5=Flash 128K)
    u16 saveType: 3;
    // Select which bank of 64K SRAM is mapped to E000000h
    u16 sramBank: 3;
    u16 enableRTC: 1;
    u16 romBank: 1;
    // Enable EEPROM mapping for 32M games (0=Off, 1=On)
    u16 bigROM: 1;
  };
} Everdrive_DeviceControl;

typedef union Everdrive_DeviceStatus {
  u16 value;
  struct {
    u16 cardBusy: 1;
    u16 cardTimeout: 1;
  };
} Everdrive_DeviceStatus;

typedef u16 Everdrive_CardCommand;
typedef u32 Everdrive_CardArgument;

enum {
  EVERDRIVE_CARD_CMD0   = 0x40, // Resets all cards to idle state
  EVERDRIVE_CARD_CMD2   = 0x42, // Asks any card to send their "card identification" (CID)
  EVERDRIVE_CARD_CMD3   = 0x43, // Ask the card to publish a new "relative card address" (RCA)
  EVERDRIVE_CARD_CMD7   = 0x47, // Toggles card between stand-by and transfer states
  EVERDRIVE_CARD_CMD8   = 0x48, // Tell the SD card which voltages it must accept
  EVERDRIVE_CARD_CMD12  = 0x4C, // Stop transmission on multiple block read
  EVERDRIVE_CARD_CMD17  = 0x51, // Reads single block
  EVERDRIVE_CARD_CMD18  = 0x52, // Reads multiple blocks
  EVERDRIVE_CARD_CMD55  = 0x77, // Next command set to be an application command
  EVERDRIVE_CARD_ACMD6  = 0x46, // Defines the data bus width (use a 1 or 4 bit bus)
  EVERDRIVE_CARD_ACMD41 = 0x69, // Sends host capacity support information (HCS)
};

typedef union Everdrive_CardResponse {
  u8 data[17];
  struct {
    u8 commandIndex: 6;
    u8 transmission: 1;
    u8 start: 1;
    u8 unmapped1: 8;
    u8 unmapped2: 8;
    u8 currentState: 4;
    u8 readyForData: 1;
    u8 cardStatus3: 3;
    u8 unmapped3: 5;
    u8 acmdAccepted: 1;
    u8 unmapped4: 2;
    u8 end: 1;
    u8 crc7: 7;
  } R1;
  struct {
    u8 commandIndex: 6;
    u8 transmission: 1;
    u8 start: 1;
    u8 unmapped: 5;
    u8 uhsIICard: 1;
    u8 capacityStatus: 1;
    u8 poweredUp: 1;
    u8 voltageBits: 8;
    u8 reserved1: 8;
    u8 reserved2: 8;
    u8 end: 1;
    u8 crc7: 7;
  } R3;
  struct {
    u8 commandIndex: 6;
    u8 transmission: 1;
    u8 start: 1;
    u8 reserved1: 8;
    u8 reserved2: 8;
    u8 voltageAccepted: 4;
    u8 pcieResponse: 1;
    u8 pcieSupport: 1;
    u8 reserved3: 2;
    u8 echoBack: 8;
    u8 end: 1;
    u8 crc7: 7;
  } R7;
  struct {
    u8 commandIndex: 6;
    u8 transmission: 1;
    u8 start: 1;
    u8 rca1: 8;
    u8 rca2: 8;
    u8 unmapped1: 8;
    u8 unmapped2: 8;
    u8 end: 1;
    u8 crc7: 7;
  } R6;
} Everdrive_CardResponse;

typedef enum Everdrive_CardMode {
  // wait for card response
  EVERDRIVE_CARD_MODE1 = 0,
  EVERDRIVE_CARD_MODE2 = 1,
  // required for start/wait F0
  EVERDRIVE_CARD_MODE4 = 2,
  // send or receive commands
  EVERDRIVE_CARD_MODE8 = 3,
} Everdrive_CardMode;

typedef union Everdrive_CardData {
  u16 value;
  struct {
    u16 highByte: 8;
    u16 lowByte: 8;
  };
} Everdrive_CardData;

typedef union Everdrive_CardControl {
  u16 value;
  struct {
    u16 speed: 1;
    u16 mode: 2;
    u16 awaitF0: 1;
    u16 startF0: 1;
  };
  struct {
    u16 commandIndex: 6;
    u16 transmission: 1;
    u16 start: 1;
  };
} Everdrive_CardControl;

typedef enum Everdrive_CardSpeed {
  EVERDRIVE_CARD_SPEED_SLOW = 0,
  EVERDRIVE_CARD_SPEED_FAST = 1,
} Everdrive_CardSpeed;

#define EVERDRIVE_UNLOCK_KEY(ADDR)     ((Everdrive_UnlockKey *)     (ADDR))
#define EVERDRIVE_DEVICE_CONTROL(ADDR) ((Everdrive_DeviceControl *) (ADDR))
#define EVERDRIVE_DEVICE_STATUS(ADDR)  ((Everdrive_DeviceStatus *)  (ADDR))

#define EVERDRIVE_CARD_COMMAND(ADDR)   ((Everdrive_CardCommand *) (ADDR))
#define EVERDRIVE_CARD_DATA(ADDR)      ((Everdrive_CardData *)    (ADDR))
#define EVERDRIVE_CARD_CONTROL(ADDR)   ((Everdrive_CardControl *) (ADDR))

typedef struct Everdrive_System {
  Everdrive_UnlockKey *const volatile unlockKey;

  Everdrive_DeviceControl *const volatile deviceControl;
  const Everdrive_DeviceStatus *const volatile deviceStatus;

  Everdrive_CardCommand *const volatile cardCommand;
  Everdrive_CardData *const volatile cardData;
  Everdrive_CardControl *const volatile cardControl;

  Everdrive_CardArgument cardAddress; // Holds "relative card address" (RCA)
  Everdrive_CardSpeed cardSpeed;

  void (*cardCallback)(Everdrive_CardCommand command);
} Everdrive_System;

Everdrive_System*
Everdrive_GetSystem();

void
Everdrive_UnlockSystem();

bool
Everdrive_CardInit();

bool
Everdrive_CardInitialize();

bool
Everdrive_CardReadBlock(unsigned int address, void *buffer);

#endif
