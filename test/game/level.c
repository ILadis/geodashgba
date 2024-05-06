
#include <game/level.h>
#include "../test.h"

test(From_ShouldInitializeAsciiLevelAsExpected) {
  // arrange
  char data[] = ""
    "       \n"
    "       \n"
    "       ";

  AsciiLevel ascii;
  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));

  // act
  Level *level = AsciiLevel_From(&ascii, source);

  // assert
  assert(level->self == &ascii);
  assert(ascii.source == source);
  assert(ascii.size.x == 7);
  assert(ascii.size.y == 3);
  assert(ascii.limit == 0);
}

test(From_ShouldInitializeBinv1LevelAsExpected) {
  // arrange
  unsigned char data[] = { 0x00, 0x00, 0x00 };

  Binv1Level binv1;
  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));

  // act
  Level *level = Binv1Level_From(&binv1, source);

  // assert
  assert(level->self == &binv1);
  assert(binv1.source == source);
}

test(GetId_ShouldReturnZeroWhenLevelHasNoId) {
  // arrange
  unsigned char data[] =
    "{a:Author}   \n"
    "{n:Test}     \n"
    "{d:5}        \n"
    "             \n";

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = AsciiLevel_From(&(AsciiLevel) {0}, source);

  // act
  int id = Level_GetId(level);

  // assert
  assert(id == 0);
}

test(GetId_ShouldReturnExpectedLevelId) {
  // arrange
  unsigned char data[] =
    "{a:Author}    \n"
    "{n:Test}      \n"
    "{i:87d1-903f} \n"
    "              \n";

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = AsciiLevel_From(&(AsciiLevel) {0}, source);

  // act
  int id = Level_GetId(level);

  // assert
  assert(id == 0x87D1903F);
}

test(GetId_ShouldReturnSetId) {
  // arrange
  unsigned char data[1024] = {0};

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = Binv1Level_From(&(Binv1Level) {0}, source);

  Level_SetId(level, 0x5f5f87bb);

  // act
  int id = Level_GetId(level);

  // assert
  assert(id == 0x5f5f87bb);
}

test(GetName_ShouldReturnExpectedLevelName) {
  // arrange
  unsigned char data[] =
    "{a:Author}   \n"
    "{n:Test}     \n"
    "{d:5}        \n"
    "             \n";

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = AsciiLevel_From(&(AsciiLevel) {0}, source);

  // act
  char name[5] = {0};
  Level_GetName(level, name);

  // assert
  assert(name[0] == 'T');
  assert(name[1] == 'e');
  assert(name[2] == 's');
  assert(name[3] == 't');
  assert(name[4] == '\0');
}

test(GetName_ShouldReturnSetName) {
  // arrange
  unsigned char data[1024] = {0};

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = Binv1Level_From(&(Binv1Level) {0}, source);

  Level_SetName(level, "Test");

  // act
  char name[5] = {0};
  Level_GetName(level, name);

  // assert
  assert(name[0] == 'T');
  assert(name[1] == 'e');
  assert(name[2] == 's');
  assert(name[3] == 't');
  assert(name[4] == '\0');
}

test(GetChunkCount_ShouldReturnExpectedChunkCount) {
  // arrange
  unsigned char data2[] = { 0x00, 0x00, 0x00 };
  unsigned char data1[] =
    "                              \n"
    "                              \n";

  DataSource *source1 = Buffer_From(&(Buffer) {0}, data1, length(data1));
  Level *level1 = AsciiLevel_From(&(AsciiLevel) {0}, source1);

  DataSource *source2 = Buffer_From(&(Buffer) {0}, data2, length(data2));
  Level *level2 = Binv1Level_From(&(Binv1Level) {0}, source2);

  // act
  int count1 = Level_GetChunkCount(level1);
  int count2 = Level_GetChunkCount(level2);

  // assert
  assert(count1 == 2);
  assert(count2 == 2);
}

test(GetChunk_ShouldCreatePitsAtExpectedPositions) {
  // arrange
  int offset = 448;
  unsigned char data[] =
    "       \n"
    "       \n"
    "  ...  \n";

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = AsciiLevel_From(&(AsciiLevel) {0}, source);

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(level, &chunk);

  // assert
  assert(chunk.count == 1);
  assert(chunk.objects[0].type = OBJECT_TYPE_PIT);
  assert(chunk.objects[0].hitbox.center.x == 56);
  assert(chunk.objects[0].hitbox.center.y == 46 + offset);
}

test(GetChunk_ShouldCreateRingsAndSpikesAtExpectedPositions) {
  // arrange
  int offset = 448;
  unsigned char data[] =
    "               \n"
    "              @\n"
    "            ,,,\n";

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = AsciiLevel_From(&(AsciiLevel) {0}, source);

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(level, &chunk);

  // assert
  assert(chunk.count == 4);
  assert(chunk.objects[0].type = OBJECT_TYPE_SPIKE);
  assert(chunk.objects[0].hitbox.center.x == 200);
  assert(chunk.objects[0].hitbox.center.y == 44 + offset);
  assert(chunk.objects[1].type = OBJECT_TYPE_SPIKE);
  assert(chunk.objects[1].hitbox.center.x == 216);
  assert(chunk.objects[1].hitbox.center.y == 44 + offset);
  assert(chunk.objects[2].type = OBJECT_TYPE_RING);
  assert(chunk.objects[2].hitbox.center.x == 232);
  assert(chunk.objects[2].hitbox.center.y == 24 + offset);
  assert(chunk.objects[3].type = OBJECT_TYPE_SPIKE);
  assert(chunk.objects[3].hitbox.center.x == 232);
  assert(chunk.objects[3].hitbox.center.y == 44 + offset);
}

test(GetChunk_ShouldCreateBoxWithPoleAtExpectedPosition) {
  // arrange
  int offset = 448;
  unsigned char data[] =
    "   i   \n"
    "   x   \n"
    "   x   \n";

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = AsciiLevel_From(&(AsciiLevel) {0}, source);

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(level, &chunk);

  // assert
  assert(chunk.count == 1);
  assert(chunk.objects[0].type = OBJECT_TYPE_BOX_WITH_POLE);
  assert(chunk.objects[0].hitbox.center.x == 56);
  assert(chunk.objects[0].hitbox.center.y == 32 + offset);
}

test(GetChunk_ShouldCreateBoxesAtExpectedPositions) {
  // arrange
  int offset = 448;
  unsigned char data[] =
    "x     x\n"
    "   x   \n"
    "x     x\n";

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = AsciiLevel_From(&(AsciiLevel) {0}, source);

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(level, &chunk);

  // assert
  assert(chunk.count == 5);
  assert(chunk.objects[0].type = OBJECT_TYPE_BOX);
  assert(chunk.objects[0].hitbox.center.x ==   8);
  assert(chunk.objects[0].hitbox.center.y ==   8 + offset);
  assert(chunk.objects[1].type = OBJECT_TYPE_BOX);
  assert(chunk.objects[1].hitbox.center.x ==   8);
  assert(chunk.objects[1].hitbox.center.y ==  40 + offset);
  assert(chunk.objects[2].type = OBJECT_TYPE_BOX);
  assert(chunk.objects[2].hitbox.center.x ==  56);
  assert(chunk.objects[2].hitbox.center.y ==  24 + offset);
  assert(chunk.objects[3].type = OBJECT_TYPE_BOX);
  assert(chunk.objects[3].hitbox.center.x == 104);
  assert(chunk.objects[3].hitbox.center.y ==   8 + offset);
  assert(chunk.objects[4].type = OBJECT_TYPE_BOX);
  assert(chunk.objects[4].hitbox.center.x == 104);
  assert(chunk.objects[4].hitbox.center.y ==  40 + offset);
}

test(GetChunk_ShouldCreateBoxesWithExpectedSizes) {
  // arrange
  int offset = 336;
  unsigned char data[] =
    "               \n"
    "       ^^^^    \n"
    "^      xxxx    \n"
    "xxxx          x\n"
    "xxxxxxxxxxxxxxx\n"
    "xxxxxxxxxxxxxxx\n"
    "xxxxxxxxxxxxxxx\n"
    "xxxxxxxxxxxxxxx\n"
    "xxxxxxxxxxxxxxx\n"
    "xxxxxxxxxxxxxxx\n";

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = AsciiLevel_From(&(AsciiLevel) {0}, source);

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(level, &chunk);

  // assert
  assert(chunk.count == 9);
  assert(chunk.objects[1].hitbox.center.x ==  32);
  assert(chunk.objects[1].hitbox.center.y == 104 + offset);
  assert(chunk.objects[1].hitbox.size.x   ==  32);
  assert(chunk.objects[1].hitbox.size.y   ==  56);
  assert(chunk.objects[2].hitbox.center.x == 152);
  assert(chunk.objects[2].hitbox.center.y == 112 + offset);
  assert(chunk.objects[2].hitbox.size.x   ==  88);
  assert(chunk.objects[2].hitbox.size.y   ==  48);
  assert(chunk.objects[4].hitbox.center.x == 144);
  assert(chunk.objects[4].hitbox.center.y ==  40 + offset);
  assert(chunk.objects[4].hitbox.size.x   ==  32);
  assert(chunk.objects[4].hitbox.size.y   ==   8);
  assert(chunk.objects[8].hitbox.center.x == 232);
  assert(chunk.objects[8].hitbox.center.y ==  56 + offset);
  assert(chunk.objects[8].hitbox.size.x   ==   8);
  assert(chunk.objects[8].hitbox.size.y   ==   8);
}

test(GetChunk_ShouldReturnEqualObjectsForSameLevelData) {
  // arrange
  unsigned char data1[] =
    "           \n"
    "         ^ \n";

  DataSource *source1 = Buffer_From(&(Buffer) {0}, data1, length(data1));
  Level *level1 = AsciiLevel_From(&(AsciiLevel) {0}, source1);

  unsigned char data2[] = {
    // meta data
    0x00,
    // chunk data
    0x01, 0x06, 0x98, 0x00, 0xe8, 0x01, 0x08, 0x00,
    0x08, 0x00, 0x98, 0x00, 0xe8, 0x01, 0x08, 0x00,
    0x08, 0x00, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00,
    0x98, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00,
    0x90, 0x00, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x00,
    0xa0, 0x00, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  DataSource *source2 = Buffer_From(&(Buffer) {0}, data2, length(data2));
  Level *level2 = Binv1Level_From(&(Binv1Level) {0}, source2);

  Chunk chunk1 = {0}, chunk2 = {0};
  Chunk_AssignIndex(&chunk1, 0);
  Chunk_AssignIndex(&chunk2, 0);

  // act
  Level_GetChunk(level1, &chunk1);
  Level_GetChunk(level2, &chunk2);

  // assert
  assert(chunk1.count == chunk2.count);
  assert(chunk1.objects[0].type == chunk2.objects[0].type);
  assert(chunk1.objects[0].solid == chunk2.objects[0].solid);
  assert(chunk1.objects[0].deadly == chunk2.objects[0].deadly);
  assert(chunk1.objects[0].hitbox.center.x == chunk2.objects[0].hitbox.center.x);
  assert(chunk1.objects[0].hitbox.center.y == chunk2.objects[0].hitbox.center.y);

  for (int i = 0; i < length(chunk1.objects[0].properties); i++) {
    assert(chunk1.objects[0].properties[i] == chunk2.objects[0].properties[i]);
  }
}

test(GetChunk_ShouldReturnAddedObjects) {
  // arrange
  char data2[1024] = {0};
  char data1[] =
    "i      ~  <xxx\n"
    "x      -      \n"
    "x^^ ..._  <xxx\n";

  DataSource *source1 = Buffer_From(&(Buffer) {0}, data1, length(data1));
  Level *level1 = AsciiLevel_From(&(AsciiLevel) {0}, source1);

  DataSource *source2 = Buffer_From(&(Buffer) {0}, data2, length(data2));
  Level *level2 = Binv1Level_From(&(Binv1Level) {0}, source2);

  Chunk chunk1 = {0};
  Chunk_AssignIndex(&chunk1, 0);

  Chunk chunk2 = {0};
  Chunk_AssignIndex(&chunk2, 0);

  // act
  Level_GetChunk(level1, &chunk1);
  Level_AddChunk(level2, &chunk1);
  Level_GetChunk(level2, &chunk2);

  // assert
  assert(chunk1.count == chunk2.count);

  for (int i = 0; i < chunk1.count; i++) {
    assert(chunk1.objects[i].type == chunk2.objects[i].type);
    assert(chunk1.objects[i].solid == chunk2.objects[i].solid);
    assert(chunk1.objects[i].deadly == chunk2.objects[i].deadly);
    assert(chunk1.objects[i].hitbox.center.x == chunk2.objects[i].hitbox.center.x);
    assert(chunk1.objects[i].hitbox.center.y == chunk2.objects[i].hitbox.center.y);
  }
}

test(GetChunk_ShouldNotReturnObjectsFromMetaDataSection) {
  // arrange
  unsigned char data[] =
    "{a:^^   :}   \n"
    "{n:xx i x}   \n"
    "{d:   x  }   \n"
    "             \n"
    "             \n";

  DataSource *source = Buffer_From(&(Buffer) {0}, data, length(data));
  Level *level = AsciiLevel_From(&(AsciiLevel) {0}, source);

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0);

  // act
  Level_GetChunk(level, &chunk);

  // assert
  assert(chunk.count == 0);
}

test(Convert_ShouldConvertLevelFromAsciiToBinv1) {
  // arrange
  unsigned char data2[512] = {0};
  unsigned char data1[] =
    "{i:1468-a561}        |\n"
    "{n:Test}             |\n"
    "                     |\n"
    "  x x x       T ...  |\n";

  DataSource *source1 = Buffer_From(&(Buffer) {0}, data1, length(data1));
  Level *ascii = AsciiLevel_From(&(AsciiLevel) {0}, source1);

  DataSource *source2 = Buffer_From(&(Buffer) {0}, data2, length(data2));
  Level *binv1 = Binv1Level_From(&(Binv1Level) {0}, source2);

  // act
  int chunks = Level_Convert(ascii, binv1);

  char name[5] = {0};
  int id = Level_GetId(binv1);
  Level_GetName(binv1, name);

  // assert
  assert(chunks == 2);
  assert(id == 0x1468A561);
  assert(name[0] == 'T');
  assert(name[1] == 'e');
  assert(name[2] == 's');
  assert(name[3] == 't');
  assert(name[4] == '\0');

  Chunk chunk1 = {0}, chunk2 = {0};
  for (int index = 0; index < chunks; index++) {
    Chunk_AssignIndex(&chunk1, index);
    Level_GetChunk(ascii, &chunk1);

    Chunk_AssignIndex(&chunk2, index);
    Level_GetChunk(binv1, &chunk2);

    assert(chunk1.count == chunk2.count);

    for (int i = 0; i < chunk1.count; i++) {
      assert(chunk1.objects[i].type == chunk2.objects[i].type);
      assert(chunk1.objects[i].solid == chunk2.objects[i].solid);
      assert(chunk1.objects[i].deadly == chunk2.objects[i].deadly);
      assert(chunk1.objects[i].hitbox.center.x == chunk2.objects[i].hitbox.center.x);
      assert(chunk1.objects[i].hitbox.center.y == chunk2.objects[i].hitbox.center.y);
    }
  }
}

suite(
  From_ShouldInitializeAsciiLevelAsExpected,
  From_ShouldInitializeBinv1LevelAsExpected,
  GetId_ShouldReturnZeroWhenLevelHasNoId,
  GetId_ShouldReturnExpectedLevelId,
  GetId_ShouldReturnSetId,
  GetName_ShouldReturnExpectedLevelName,
  GetName_ShouldReturnSetName,
  GetChunkCount_ShouldReturnExpectedChunkCount,
  GetChunk_ShouldCreatePitsAtExpectedPositions,
  GetChunk_ShouldCreateRingsAndSpikesAtExpectedPositions,
  GetChunk_ShouldCreateBoxWithPoleAtExpectedPosition,
  GetChunk_ShouldCreateBoxesAtExpectedPositions,
  GetChunk_ShouldCreateBoxesWithExpectedSizes,
  GetChunk_ShouldReturnEqualObjectsForSameLevelData,
  GetChunk_ShouldReturnAddedObjects,
  GetChunk_ShouldNotReturnObjectsFromMetaDataSection,
  Convert_ShouldConvertLevelFromAsciiToBinv1);
