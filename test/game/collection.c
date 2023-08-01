
#include <game/collection.h>
#include "../test.h"

test(DefineWithTotalSpace_ShouldDefineNewCollectionWithGivenSize) {
  // act
  Collection *collection = Collection_DefineWithTotalSpace(1024);

  // assert
  assert(collection->length == 1024);
}

test(DefineWithUsableSpace_ShouldDefineNewCollectionWithUsableSpace) {
  // act
  Collection *collection = Collection_DefineWithUsableSpace(7);

  // assert
  assert(collection->length > 7);
}

test(DefineWithUsableSpace_ShouldAllocatedExpectedBytesForZeroSize) {
  // arrange
  const int size = sizeof(Collection);
  const unsigned char header[] = {
    0x4c, 0x65, 0x76, 0x65, 0x6c, 0x43, 0x6f, 0x6c, 0x6c, 0x65,
    0x63, 0x74, 0x69, 0x6f, 0x6e, 0x21, size, 0x00, 0x00, 0x00,
  };

  // act
  Collection *collection = Collection_DefineWithUsableSpace(0);

  // assert
  unsigned char *data = (unsigned char *) collection;
  for (int i = 0; i < length(header); i++) {
    assert(header[i] == data[i]);
  }
}

test(AddLevel_ShouldReturnFalseWhenThereIsNoSpaceLeftForNewLevel) {
  // arrange
  Collection *collection = Collection_DefineWithUsableSpace(6);

  unsigned char data[] = { 0x00, 0x00, 0x00 };

  Buffer buffer;
  DataSource *source = Buffer_From(&buffer, data, sizeof(data));

  Binv1Level level1;
  Binv1Level_From(&level1, source);

  Binv1Level level2;
  Binv1Level_From(&level2, source);

  Binv1Level level3;
  Binv1Level_From(&level3, source);

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
  Collection *collection = Collection_DefineWithUsableSpace(1024);

  unsigned char data[] = { 0x00, 0x00, 0x00 };

  Buffer buffer;
  DataSource *source = Buffer_From(&buffer, data, sizeof(data));

  Binv1Level level;
  Binv1Level_From(&level, source);

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

  unsigned char data[][3] = {
    { 0x01, 0x02, 0x03 },
    { 0x04, 0x05, 0x06 },
    { 0x07, 0x08, 0x09 },
  };

  for (int i = 0; i < length(data); i++) {
    Buffer buffer;
    DataSource *source = Buffer_From(&buffer, &(data[i][0]), sizeof(*data));

    Binv1Level level;
    Binv1Level_From(&level, source);

    Collection_AddLevel(collection, &level);
  }

  // act
  const Binv1Level *level = Collection_GetLevelByIndex(collection, 2);

  // assert
  assert(level != NULL);
  assert(level->size == 3);

  Reader *reader = DataSource_AsReader(level->source);
  assert(0x07 == Reader_Read(reader));
  assert(0x08 == Reader_Read(reader));
  assert(0x09 == Reader_Read(reader));
  assert(  -1 == Reader_Read(reader));
}

test(GetLevelByIndex_ShouldReturnNullIfLevelWithIndexDoesNotExist) {
  // arrange
  Collection *collection = Collection_DefineWithUsableSpace(6);

  unsigned char data[][3] = {
    { 0x01, 0x02, 0x03 },
    { 0x04, 0x05, 0x06 },
  };

  for (int i = 0; i < length(data); i++) {
    Buffer buffer;
    DataSource *source = Buffer_From(&buffer, &(data[i][0]), sizeof(*data));

    Binv1Level level;
    Binv1Level_From(&level, source);

    Collection_AddLevel(collection, &level);
  }

  // act
  const Binv1Level *levels[] = {
    Collection_GetLevelByIndex(collection, -1),
    Collection_GetLevelByIndex(collection, +2),
  };

  // assert
  assert(levels[0] == NULL);
  assert(levels[1] == NULL);
}

test(ReadFrom_ShouldReturnTrueIfCollectionIsPresentInReader) {
  // arrange
  unsigned char data[] = {
    [12]  = 'L', 'e', 'v', 'e', 'l', 'C', 'o', 'l', 'l', 'e', 'c', 't', 'i', 'o', 'n', '!',
            0xFF, 0x00, 0x00, 0x00,
    [512] = 0x00
  };

  Collection *collection = Collection_DefineWithUsableSpace(200);

  Buffer buffer;
  DataSource *source = Buffer_From(&buffer, data, sizeof(data));
  Reader *reader = DataSource_AsReader(source);

  // act
  bool result = Collection_ReadFrom(collection, reader);

  // assert
  assert(result == true);
  assert(collection->length == 0xFF);
}

test(ReadFrom_ShouldReturnFalseIfCollectionInReaderWouldNotFitIntoProvidedCollection) {
  // arrange
  unsigned char data[] = {
    [12]  = 'L', 'e', 'v', 'e', 'l', 'C', 'o', 'l', 'l', 'e', 'c', 't', 'i', 'o', 'n', '!',
            0xFF, 0x00, 0x00, 0x00,
    [512] = 0x00
  };

  Collection *collection = Collection_DefineWithTotalSpace(254);

  Buffer buffer;
  DataSource *source = Buffer_From(&buffer, data, sizeof(data));
  Reader *reader = DataSource_AsReader(source);

  // act
  bool result = Collection_ReadFrom(collection, reader);

  // assert
  assert(result == false);
  assert(collection->length == 0xFF);
}

suite(
  DefineWithTotalSpace_ShouldDefineNewCollectionWithGivenSize,
  DefineWithUsableSpace_ShouldDefineNewCollectionWithUsableSpace,
  DefineWithUsableSpace_ShouldAllocatedExpectedBytesForZeroSize,
  AddLevel_ShouldAllocateSpaceForLevelAndAppendLevelData,
  AddLevel_ShouldAllocateSpaceForLevelAndAppendLevelData,
  GetLevelByIndex_ShouldReturnExpectedLevel,
  GetLevelByIndex_ShouldReturnNullIfLevelWithIndexDoesNotExist,
  ReadFrom_ShouldReturnTrueIfCollectionIsPresentInReader,
  ReadFrom_ShouldReturnFalseIfCollectionInReaderWouldNotFitIntoProvidedCollection);
