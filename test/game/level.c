
#include <game/level.h>
#include "../test.h"

test(GetName_ShouldReturnExpectedLevelName) {
  // arrange
  Level level = Level_FromLayout(
    "{a:Author}   ",
    "{n:Test}     ",
    "{d:5}        ",
    "             ",
  );

  // act
  char name[5] = {0};
  Level_GetName(&level, name);

  // assert
  assert(name[0] == 'T');
  assert(name[1] == 'e');
  assert(name[2] == 's');
  assert(name[3] == 't');
  assert(name[4] == '\0');
}

test(GetName_ShouldReturnSetName) {
  // arrange
  Level level = Level_AllocateNew(1024);
  Level_SetName(&level, "Test");

  // act
  char name[5] = {0};
  Level_GetName(&level, name);

  // assert
  assert(name[0] == 'T');
  assert(name[1] == 'e');
  assert(name[2] == 's');
  assert(name[3] == 't');
  assert(name[4] == '\0');
}

test(GetChunkCount_ShouldReturnExpectedChunkCount) {
  // arrange
  Level level1 = Level_FromLayout(
    "                              ",
    "                              ",
  );

  Level level2 = Level_FromData(0x00, 0x00, 0x00);

  // act
  int count1 = Level_GetChunkCount(&level1);
  int count2 = Level_GetChunkCount(&level2);

  // assert
  assert(count1 == 2);
  assert(count2 == 2);
}

test(GetChunk_ShouldCreatePitsAtExpectedPositions) {
  // arrange
  const int offset = 432;

  Level level = Level_FromLayout(
    "       ",
    "       ",
    "  ...  ",
  );

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(&level, &chunk);

  // assert
  assert(chunk.count == 1);
  assert(chunk.objects[0].type = TYPE_PIT);
  assert(chunk.objects[0].hitbox.center.x == 56);
  assert(chunk.objects[0].hitbox.center.y == 46 + offset);
}

test(GetChunk_ShouldCreateRingsAndSpikesAtExpectedPositions) {
  // arrange
  const int offset = 432;

  Level level = Level_FromLayout(
    "               ",
    "              @",
    "            ,,,",
  );

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(&level, &chunk);

  // assert
  assert(chunk.count == 4);
  assert(chunk.objects[0].type = TYPE_SPIKE);
  assert(chunk.objects[0].hitbox.center.x == 200);
  assert(chunk.objects[0].hitbox.center.y == 44 + offset);
  assert(chunk.objects[1].type = TYPE_SPIKE);
  assert(chunk.objects[1].hitbox.center.x == 216);
  assert(chunk.objects[1].hitbox.center.y == 44 + offset);
  assert(chunk.objects[2].type = TYPE_RING);
  assert(chunk.objects[2].hitbox.center.x == 232);
  assert(chunk.objects[2].hitbox.center.y == 24 + offset);
  assert(chunk.objects[3].type = TYPE_SPIKE);
  assert(chunk.objects[3].hitbox.center.x == 232);
  assert(chunk.objects[3].hitbox.center.y == 44 + offset);
}

test(GetChunk_ShouldCreateBoxWithPoleAtExpectedPosition) {
  // arrange
  const int offset = 432;

  Level level = Level_FromLayout(
    "   i   ",
    "   x   ",
    "   x   ",
  );

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(&level, &chunk);

  // assert
  assert(chunk.count == 1);
  assert(chunk.objects[0].type = TYPE_BOX_WITH_POLE);
  assert(chunk.objects[0].hitbox.center.x == 56);
  assert(chunk.objects[0].hitbox.center.y == 32 + offset);
}

test(GetChunk_ShouldCreateBoxesAtExpectedPositions) {
  // arrange
  const int offset = 432;

  Level level = Level_FromLayout(
    "x     x",
    "   x   ",
    "x     x",
  );

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(&level, &chunk);

  // assert
  assert(chunk.count == 5);
  assert(chunk.objects[0].type = TYPE_BOX);
  assert(chunk.objects[0].hitbox.center.x ==   8);
  assert(chunk.objects[0].hitbox.center.y ==   8 + offset);
  assert(chunk.objects[1].type = TYPE_BOX);
  assert(chunk.objects[1].hitbox.center.x ==   8);
  assert(chunk.objects[1].hitbox.center.y ==  40 + offset);
  assert(chunk.objects[2].type = TYPE_BOX);
  assert(chunk.objects[2].hitbox.center.x ==  56);
  assert(chunk.objects[2].hitbox.center.y ==  24 + offset);
  assert(chunk.objects[3].type = TYPE_BOX);
  assert(chunk.objects[3].hitbox.center.x == 104);
  assert(chunk.objects[3].hitbox.center.y ==   8 + offset);
  assert(chunk.objects[4].type = TYPE_BOX);
  assert(chunk.objects[4].hitbox.center.x == 104);
  assert(chunk.objects[4].hitbox.center.y ==  40 + offset);
}

test(GetChunk_ShouldCreateBoxesWithExpectedSizes) {
  // arrange
  const int offset = 320;

  Level level = Level_FromLayout(
    "               ",
    "       ^^^^    ",
    "^      xxxx    ",
    "xxxx          x",
    "xxxxxxxxxxxxxxx",
    "xxxxxxxxxxxxxxx",
    "xxxxxxxxxxxxxxx",
    "xxxxxxxxxxxxxxx",
    "xxxxxxxxxxxxxxx",
    "xxxxxxxxxxxxxxx",
  );

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(&level, &chunk);

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
  Level level1 = Level_FromLayout(
    "           ",
    "         ^ ",
  );

  Level level2 = Level_FromData(
    // meta data
    0x00,
    // chunk data
    0x01, 0x06, 0x98, 0x00, 0xd8, 0x01, 0x08, 0x00,
    0x08, 0x00, 0x98, 0x00, 0xd8, 0x01, 0x08, 0x00,
    0x08, 0x00, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00,
    0x98, 0x00, 0x00, 0x00, 0xd0, 0x01, 0x00, 0x00,
    0x90, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00,
    0xa0, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  );

  Chunk chunk1 = {0}, chunk2 = {0};
  Chunk_AssignIndex(&chunk1, 0);
  Chunk_AssignIndex(&chunk2, 0);

  // act
  Level_GetChunk(&level1, &chunk1);
  Level_GetChunk(&level2, &chunk2);

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
  const int offset = 432;

  Level level1 = Level_FromLayout(
    "i      ~  <xxx",
    "x      -      ",
    "x^^ ..._  <xxx",
  );

  Level level2 = Level_AllocateNew(1024);

  Chunk chunk1 = {0};
  Chunk_AssignIndex(&chunk1, 0);

  Chunk chunk2 = {0};
  Chunk_AssignIndex(&chunk2, 0);

  // act
  Level_GetChunk(&level1, &chunk1);
  Level_AddChunk(&level2, &chunk1);
  Level_GetChunk(&level2, &chunk2);

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
  Level level = Level_FromLayout(
    "{a:^^   :}   ",
    "{n:xx i x}   ",
    "{d:   x  }   ",
    "             ",
    "             ",
  );

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0);

  // act
  Level_GetChunk(&level, &chunk);

  // assert
  assert(chunk.count == 0);
}

test(Convert_ShouldConvertLevelFromAsciiToBinv1) {
  // arrange
  Level binv1 = Level_AllocateNew(512);
  Level ascii = Level_FromLayout(
    "{n:Test} ",
    "         ",
    "  x x x  ",
  );

  char name[5] = {0};

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0);

  // act
  int chunks = Level_Convert(&ascii, &binv1);
  Level_GetName(&binv1, name);
  Level_GetChunk(&binv1, &chunk);

  // assert
  assert(chunks == 1);
  assert(name[0] == 'T');
  assert(name[1] == 'e');
  assert(name[2] == 's');
  assert(name[3] == 't');
  assert(name[4] == '\0');
  assert(chunk.count == 3);
}

suite(
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
