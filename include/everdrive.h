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
  // TODO use abbreviations
  EVERDRIVE_CARD_CMD0   = 0x40, // Software reset
  EVERDRIVE_CARD_CMD2   = 0x42, // Reads the "card identification register" (CID)
  EVERDRIVE_CARD_CMD3   = 0x43, // Reads the "relative card address register" (RCA)
  EVERDRIVE_CARD_CMD7   = 0x47, // Toggles card between stand-by and transfer states
  EVERDRIVE_CARD_CMD8   = 0x48, // Tell the SD card which voltages it must accept
  EVERDRIVE_CARD_CMD12  = 0x4C, // Stop transmission on multiple block read
  EVERDRIVE_CARD_CMD17  = 0x51, // Reads single block
  EVERDRIVE_CARD_CMD18  = 0x52, // Reads multiple blocks
  EVERDRIVE_CARD_CMD55  = 0x77, // Next command set to be an application command
  EVERDRIVE_CARD_ACMD6  = 0x46, // Specify bus width (1 or 4 bits)
  EVERDRIVE_CARD_ACMD41 = 0x69, // Send operation condition
};

typedef enum Everdrive_CardResponse {
  // TODO map bit fields using unions
  EVERDRIVE_CARD_R1,
  EVERDRIVE_CARD_R2,
  EVERDRIVE_CARD_R3,
  EVERDRIVE_CARD_R6,
  EVERDRIVE_CARD_R7,
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

  // TODO add returned card RCA
  Everdrive_CardSpeed cardSpeed;
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
