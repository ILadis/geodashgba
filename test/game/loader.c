
#include <game/loader.h>
#include "../test.h"

test(GetChunk_ShouldCreatBoxesAtExpectedPositions) {
  // arrange
  const int offset = 432;

  Loader loader = Loader_ForLayout(
    "x     x",
    "   x   ",
    "x_____x",
  );

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Loader_GetChunk(&loader, &chunk);

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

  Loader loader = Loader_ForLayout(
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
  Loader_GetChunk(&loader, &chunk);

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

suite(
  GetChunk_ShouldCreatBoxesAtExpectedPositions,
  GetChunk_ShouldCreateBoxesWithExpectedSizes);
