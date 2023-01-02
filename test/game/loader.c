
#include <game/level.h>
#include "../test.h"

test(GetChunk_ShouldCreatBoxesAtExpectedPositions) {
  // arrange
  const int offset = 432;

  Level level = Level_FromLayout(
    "x     x",
    "   x   ",
    "x_____x",
  );

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Level_GetChunk(&level, &chunk);

  // assert
  assert(chunk.count == 5);
  assert(chunk.objects[0].hitbox.center.x ==   8);
  assert(chunk.objects[0].hitbox.center.y ==   8 + offset);
  assert(chunk.objects[1].hitbox.center.x ==   8);
  assert(chunk.objects[1].hitbox.center.y ==  40 + offset);
  assert(chunk.objects[2].hitbox.center.x ==  56);
  assert(chunk.objects[2].hitbox.center.y ==  24 + offset);
  assert(chunk.objects[3].hitbox.center.x == 104);
  assert(chunk.objects[3].hitbox.center.y ==   8 + offset);
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
  assert(chunk.objects[1].hitbox.center.y ==  56 + offset);
  assert(chunk.objects[1].hitbox.size.x   ==  32);
  assert(chunk.objects[1].hitbox.size.y   ==   8);
  assert(chunk.objects[2].hitbox.center.x == 120);
  assert(chunk.objects[2].hitbox.center.y == 112 + offset);
  assert(chunk.objects[2].hitbox.size.x   == 120);
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
  const unsigned char buffer[] = {
    0x01, 0x04, 0x98, 0x00, 0xd8, 0x01, 0x08, 0x00,
    0x08, 0x00, 0x98, 0x00, 0xd8, 0x01, 0x08, 0x00,
    0x08, 0x00, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00,
    0x98, 0x00, 0x00, 0x00, 0xd0, 0x01, 0x00, 0x00,
    0x90, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00,
    0xa0, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
  };

  Level level1 = Level_FromLayout(
    "           ",
    "_________^_",
  );

  Level level2 = Level_FromBuffer(buffer);

  Chunk chunk1 = {0}, chunk2 = {0};
  Chunk_AssignIndex(&chunk1, 0);
  Chunk_AssignIndex(&chunk2, 0);

  // act
  Level_GetChunk(&level1, &chunk1);
  Level_GetChunk(&level2, &chunk2);

  // assert
  assert(chunk1.count == chunk2.count);
  assert(chunk1.objects[0].solid == chunk2.objects[0].solid);
  assert(chunk1.objects[0].deadly == chunk2.objects[0].deadly);
  assert(chunk1.objects[0].hitbox.center.x == chunk2.objects[0].hitbox.center.x);
  assert(chunk1.objects[0].hitbox.center.y == chunk2.objects[0].hitbox.center.y);

  for (int i = 0; i < length(chunk1.objects[0].properties); i++) {
    assert(chunk1.objects[0].properties[i] == chunk2.objects[0].properties[i]);
  }
}

suite(
  GetChunk_ShouldCreatBoxesAtExpectedPositions,
  GetChunk_ShouldCreateBoxesWithExpectedSizes,
  GetChunk_ShouldReturnEqualObjectsForSameLevelData);
