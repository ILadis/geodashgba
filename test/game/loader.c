
#include <game/loader.h>
#include "../test.h"

test(LoadCourse_ShouldCreatedBoxesAtExpectedPositions) {
  // arrange
  const int offset = 432;
  static const char *layout[] = {
    "x     x",
    "   x   ",
    "x_____x",
  };

  Loader loader = Loader_ForLayout(layout);

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, 0),

  // act
  Loader_GetChunk(&loader, &chunk);

  // assert
  assert(chunk.count == 5);
  assert(chunk.objects[0].hitbox.center.x ==   8);
  assert(chunk.objects[0].hitbox.center.y ==   8 + offset);
  assert(chunk.objects[1].hitbox.center.x == 104);
  assert(chunk.objects[1].hitbox.center.y ==   8 + offset);
  assert(chunk.objects[2].hitbox.center.x ==  56);
  assert(chunk.objects[2].hitbox.center.y ==  24 + offset);
  assert(chunk.objects[3].hitbox.center.x ==   8);
  assert(chunk.objects[3].hitbox.center.y ==  40 + offset);
  assert(chunk.objects[4].hitbox.center.x == 104);
  assert(chunk.objects[4].hitbox.center.y ==  40 + offset);
}

suite(
  LoadCourse_ShouldCreatedBoxesAtExpectedPositions);
