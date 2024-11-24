
#include <io.h>
#include "test.h"

test(WriteInt32_ShouldWriteExpectedBytes) {
  // arrange
  unsigned char data[32] = {0};

  DataSource *source = Buffer_From(&(Buffer){0}, data, length(data));
  Writer *writer = DataSource_AsWriter(source);

  // act
  Writer_WriteInt32(writer, 0x12AB34CD);

  // assert
  assert(data[0] == 0xCD);
  assert(data[1] == 0x34);
  assert(data[2] == 0xAB);
  assert(data[3] == 0x12);
}

test(ReadInt32_ShouldReadExpectedBytes) {
  // arrange
  unsigned char data[32] = {0xCD, 0x34, 0xAB, 0x12};

  DataSource *source = Buffer_From(&(Buffer){0}, data, length(data));
  Reader *reader = DataSource_AsReader(source);

  // act
  int value;
  Reader_ReadInt32(reader, &value);

  // assert
  assert(value == 0x12AB34CD);
}

test(WriteInt32_ShouldWriteExpectedBytesAsHex) {
  // arrange
  unsigned char data[32] = {0};

  DataSource *raw = Buffer_From(&(Buffer){0}, data, length(data));
  DataSource *source = Base16_Of(&(Base16){0}, raw);

  Writer *writer = DataSource_AsWriter(source);

  // act
  Writer_WriteInt32(writer, 0x12AB34CD);

  // assert
  assert(data[0] == 'C');
  assert(data[1] == 'D');
  assert(data[2] == '3');
  assert(data[3] == '4');
  assert(data[4] == 'A');
  assert(data[5] == 'B');
  assert(data[6] == '1');
  assert(data[7] == '2');
}

test(ReadInt32_ShouldReadExpectedBytesFromHex) {
  // arrange
  char *data = "CD34AB12";

  DataSource *raw = Buffer_FromString(&(Buffer){0}, data);
  DataSource *source = Base16_Of(&(Base16){0}, raw);

  Reader *reader = DataSource_AsReader(source);

  // act
  int value;
  Reader_ReadInt32(reader, &value);

  // assert
  assert(value == 0x12AB34CD);
}

suite(
  WriteInt32_ShouldWriteExpectedBytes,
  ReadInt32_ShouldReadExpectedBytes,
  WriteInt32_ShouldWriteExpectedBytesAsHex,
  ReadInt32_ShouldReadExpectedBytesFromHex);
