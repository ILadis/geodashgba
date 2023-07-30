
#include <game/collection.h>
#include "../test.h"

test(DefineNew_ShouldDefineNewCollectionWithGivenSize) {
  // act
  Collection *collection = Collection_DefineNew(1024);

  // assert
  assert(collection->length == 1024);
}

test(DefineNew_ShouldDefineNewCollectionWithUsableSpace) {
  // act
  Collection *collection = Collection_DefineWithUsableSpace(7);

  // assert
  assert(collection->length > 7);
}

test(AddLevel_ShouldReturnFalseWhenThereIsNoSpaceLeftForNewLevel) {
  // arrange
  Collection *collection = Collection_DefineWithUsableSpace(6);

  const Level level1 = Level_FromData(0x00, 0x00, 0x00);
  const Level level2 = Level_FromData(0x00, 0x00, 0x00);
  const Level level3 = Level_FromData(0x00, 0x00, 0x00);

  // act
  bool result1 = Collection_AddLevel(collection, &level1);
  bool result2 = Collection_AddLevel(collection, &level2);
  bool result3 = Collection_AddLevel(collection, &level3);

  // assert
  assert(result1 == true && result2 == true);
  assert(result3 == false);
  assert(collection->count == 2);
  assert(collection->allocations[0] == 3);
  assert(collection->allocations[1] == 3);
}

test(AddLevel_ShouldAllocateSpaceForLevelAndAppendLevelData) {
  // arrange
  Collection *collection = Collection_DefineNew(1024);
  const Level level = Level_FromData(0x00, 0x00, 0x00);

  // act
  bool result = Collection_AddLevel(collection, &level);

  // assert
  assert(result == true);
  assert(collection->count == 1);
  assert(collection->allocations[0] == 3);
}

test(GetLevelByIndex_ShouldReturnExpectedLevel) {
  // arrange
  Collection *collection = Collection_DefineWithUsableSpace(9);

  const Level level1 = Level_FromData(0x01, 0x02, 0x03);
  Collection_AddLevel(collection, &level1);

  const Level level2 = Level_FromData(0x04, 0x05, 0x06);
  Collection_AddLevel(collection, &level2);

  const Level level3 = Level_FromData(0x07, 0x08, 0x09);
  Collection_AddLevel(collection, &level3);

  // act
  const Level *level = Collection_GetLevelByIndex(collection, 2);

  // assert
  assert(level != NULL);
  assert(level->size.x == 3);
  assert(level->buffer.read != level3.buffer.read);

  const unsigned char *buffer = level->buffer.read;
  assert(buffer[0] == 0x07 && buffer[1] == 0x08 && buffer[2] == 0x09);
}

test(GetLevelByIndex_ShouldReturnNullIfLevelWithIndexDoesNotExist) {
  // arrange
  Collection *collection = Collection_DefineWithUsableSpace(6);

  const Level level1 = Level_FromData(0x01, 0x02, 0x03);
  Collection_AddLevel(collection, &level1);

  const Level level2 = Level_FromData(0x04, 0x05, 0x06);
  Collection_AddLevel(collection, &level2);

  // act
  const Level *levels[] = {
    Collection_GetLevelByIndex(collection, -1),
    Collection_GetLevelByIndex(collection, +2),
  };

  // assert
  assert(levels[0] == NULL);
  assert(levels[1] == NULL);
}

test(ReadFrom_ShouldReturnTrueIfCollectionIsPresentInReader) {
  // arrange
  unsigned char buffer[] = {
    [12]  = 'L', 'e', 'v', 'e', 'l', 'C', 'o', 'l', 'l', 'e', 'c', 't', 'i', 'o', 'n', '!',
            0xFF, 0x00, 0x00, 0x00,
    [512] = 0x00
  };

  Collection *collection = Collection_DefineWithUsableSpace(200);

  DataReader data;
  DataReader_From(&data, buffer, sizeof(buffer));

  Reader *reader = DataReader_AsReader(&data);

  // act
  bool result = Collection_ReadFrom(collection, reader);

  // assert
  assert(result == true);
  assert(collection->length == 0xFF);
}

test(ReadFrom_ShouldReturnFalseIfCollectionInReaderWouldNotFitIntoProvidedCollection) {
  // arrange
  unsigned char buffer[] = {
    [12]  = 'L', 'e', 'v', 'e', 'l', 'C', 'o', 'l', 'l', 'e', 'c', 't', 'i', 'o', 'n', '!',
            0xFF, 0x00, 0x00, 0x00,
    [512] = 0x00
  };

  Collection *collection = Collection_DefineNew(254);

  DataReader data;
  DataReader_From(&data, buffer, sizeof(buffer));

  Reader *reader = DataReader_AsReader(&data);

  // act
  bool result = Collection_ReadFrom(collection, reader);

  // assert
  assert(result == false);
  assert(collection->length == 0xFF);
}

suite(
  DefineNew_ShouldDefineNewCollectionWithGivenSize,
  DefineNew_ShouldDefineNewCollectionWithUsableSpace,
  AddLevel_ShouldAllocateSpaceForLevelAndAppendLevelData,
  AddLevel_ShouldAllocateSpaceForLevelAndAppendLevelData,
  GetLevelByIndex_ShouldReturnExpectedLevel,
  GetLevelByIndex_ShouldReturnNullIfLevelWithIndexDoesNotExist,
  ReadFrom_ShouldReturnTrueIfCollectionIsPresentInReader,
  ReadFrom_ShouldReturnFalseIfCollectionInReaderWouldNotFitIntoProvidedCollection);
