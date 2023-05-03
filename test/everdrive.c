
#include <everdrive.h>
#include "test.h"

test(CardResponse_ShouldMapCorrectFieldsForR1) {
  // arrange
  Everdrive_CardResponse response = {{ 0x37, 0x00, 0x00, 0x01, 0x20, 0x83 }};

  // assert
  assert(sizeof(response.R1) == 6);
  assert(response.R1.commandIndex == 55);
  assert(response.R1.acmdAccepted == 1);
  assert(response.R1.readyForData == 0);
  assert(response.R1.currentState == 1);
  assert(response.R1.crc7 == 0x41);
  assert(response.R1.end == 1);
}

test(CardResponse_ShouldMapCorrectFieldsForR3) {
  // arrange
  Everdrive_CardResponse response = {{ 0x3F, 0x40, 0xFF, 0x80, 0x00, 0xFF }};

  // assert
  assert(sizeof(response.R3) == 6);
  assert(response.R3.commandIndex == 63);
  assert(response.R3.poweredUp == 0);
  assert(response.R3.capacityStatus == 1);
  assert(response.R3.uhsIICard == 0);
  assert(response.R3.crc7 == 0x7F);
  assert(response.R3.end == 1);
}

test(CardResponse_ShouldMapCorrectFieldsForR6) {
  // arrange
  Everdrive_CardResponse response = {{ 0x03, 0xAA, 0xAA, 0x05, 0x20, 0xD1 }};

  // assert
  assert(sizeof(response.R6) == 6);
  assert(response.R6.rca1 == 0xAA);
  assert(response.R6.rca2 == 0xAA);
  assert(response.R6.crc7 == 0x68);
  assert(response.R6.end == 1);
}

test(CardResponse_ShouldMapCorrectFieldsForR7) {
  // arrange
  Everdrive_CardResponse response = {{ 0x08, 0x00, 0x00, 0x01, 0xAA, 0x13 }};

  // assert
  assert(sizeof(response.R7) == 6);
  assert(response.R7.commandIndex == 8);
  assert(response.R7.voltageAccepted == 1);
  assert(response.R7.echoBack == 0xAA);
  assert(response.R7.crc7 == 0x09);
  assert(response.R7.end == 1);
}

static Everdrive_CardCommand *buffer = NULL;

static void
Everdrive_CommandsCallback(Everdrive_CardCommand command) {
  Everdrive_System *system = Everdrive_GetSystem();

  // write to buffer
  if (*buffer == 0) {
    *(buffer++) = command;
    *(system->cardCommand) = *buffer;
  }
  // read from buffer
  else if (*buffer == command) {
    *(system->cardCommand) = *(++buffer);
  }
}

test(CardInitialize_ShouldSendExpectedCardCommands) {
  // arrange
  Everdrive_CardCommand commands[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // CMD0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // CMD8
    0xFF, 0x08, 0x00, 0x00, 0x01, 0xAA, 0x13,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // CMD55
    0xFF, 0x37, 0x00, 0x00, 0x01, 0x20, 0x83,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ACMD41
    0xFF, 0x3F, 0x40, 0xFF, 0x80, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // CMD55
    0xFF, 0x37, 0x00, 0x00, 0x01, 0x20, 0x83,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ACMD41
    0xFF, 0x3F, 0xC0, 0xFF, 0x80, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // CMD2
    0xFF, 0x3F, 0x03, 0x53, 0x44, 0x53, 0x44, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // CMD3
    0xFF, 0x03, 0xAA, 0xAA, 0x05, 0x20, 0xD1,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // CMD7
    0xFF, 0x07, 0x00, 0x00, 0x07, 0x00, 0x75,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // CMD55
    0xFF, 0x37, 0x00, 0x00, 0x09, 0x20, 0x33,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ACMD6
    0xFF, 0x06, 0x00, 0x00, 0x09, 0x20, 0xB9,
  };

  Everdrive_System *system = Everdrive_GetSystem();
  system->cardCallback = Everdrive_CommandsCallback;
  buffer = commands;

  // act
  bool result = Everdrive_CardInitialize();

  // assert
  assert(result == true);
  assert(commands[ 1] == EVERDRIVE_CARD_CMD0   && commands[ 6] == 0x95);
  assert(commands[ 8] == EVERDRIVE_CARD_CMD8   && commands[13] == 0x87);
  assert(commands[22] == EVERDRIVE_CARD_CMD55  && commands[27] == 0x65);
  assert(commands[36] == EVERDRIVE_CARD_ACMD41 && commands[41] == 0xAB);
  assert(commands[50] == EVERDRIVE_CARD_CMD55  && commands[55] == 0x65);
  assert(commands[64] == EVERDRIVE_CARD_ACMD41 && commands[69] == 0xAB);
}

suite(
  CardResponse_ShouldMapCorrectFieldsForR1,
  CardResponse_ShouldMapCorrectFieldsForR3,
  CardResponse_ShouldMapCorrectFieldsForR6,
  CardResponse_ShouldMapCorrectFieldsForR7,
  CardInitialize_ShouldSendExpectedCardCommands);
