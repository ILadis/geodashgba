
#include <hit.h>
#include "test.h"

test(Intersects_ShouldReturnNoHitWhenBoundsDoNotIntersect) {
  // arrange
  Bounds bound1 = Bounds_Of(10, 12, 4, 5);
  Bounds bound2 = Bounds_Of(3, 2, 2, 3);

  // assert
  Hit hit = Bounds_Intersects(&bound1, &bound2);

  // assert
  assert(hit.delta.x == 0);
  assert(hit.delta.y == 0);
}

test(Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnTopEdge) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 4, 2);
  Bounds bound2 = Bounds_Of(3, 3, 1, 2);

  // assert
  Hit hit = Bounds_Intersects(&bound1, &bound2);

  // assert
  assert(hit.delta.x ==  0);
  assert(hit.delta.y == -3);
}

test(Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnRightEdge) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 4, 2);
  Bounds bound2 = Bounds_Of(9, 4, 1, 3);

  // assert
  Hit hit = Bounds_Intersects(&bound1, &bound2);

  // assert
  assert(hit.delta.x == 1);
  assert(hit.delta.y == 0);
}

test(Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnBottomEdge) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 4, 2);
  Bounds bound2 = Bounds_Of(6, 5, 2, 2);

  // assert
  Hit hit = Bounds_Intersects(&bound1, &bound2);

  // assert
  assert(hit.delta.x == 0);
  assert(hit.delta.y == 3);
}

test(Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnLeftEdge) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 4, 2);
  Bounds bound2 = Bounds_Of(0, 3, 3, 2);

  // assert
  Hit hit = Bounds_Intersects(&bound1, &bound2);

  // assert
  assert(hit.delta.x == -2);
  assert(hit.delta.y ==  0);
}

test(Intersects_ShouldReturnHitWithDeltaWhenBoundsAreEmbedded) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 5, 3);
  Bounds bound2 = Bounds_Of(4, 5, 2, 2);

  // assert
  Hit hit = Bounds_Intersects(&bound1, &bound2);

  // assert
  assert(hit.delta.x == 0);
  assert(hit.delta.y == 4);
}

test(Expand_ShouldReturnExpandedBoundsWhenGivenBoundsIntersect) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 4, 2);
  Bounds bound2 = Bounds_Of(6, 6, 2, 2);

  // assert
  Bounds expand = Bounds_Expand(&bound1, &bound2);

  // assert
  assert(expand.center.x == 5);
  assert(expand.center.y == 5);
  assert(expand.size.x == 4);
  assert(expand.size.y == 3);
}

test(Expand_ShouldReturnExpandedBoundsWhenGivenBoundsDoNotIntersect) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 4, 2);
  Bounds bound2 = Bounds_Of(12, 6, 1, 4);

  // assert
  Bounds expand = Bounds_Expand(&bound1, &bound2);

  // assert
  assert(expand.center.x == 7);
  assert(expand.center.y == 6);
  assert(expand.size.x == 6);
  assert(expand.size.y == 4);
}

test(Expand_ShouldReturnExpandedBoundsWhenGivenBoundsContainEachOther) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 4, 2);
  Bounds bound2 = Bounds_Of(4, 4, 2, 2);

  // assert
  Bounds expand = Bounds_Expand(&bound1, &bound2);

  // assert
  assert(expand.center.x == 5);
  assert(expand.center.y == 4);
  assert(expand.size.x == 4);
  assert(expand.size.y == 2);
}

test(Embed_ShouldReturnEmbeddedBoundsWhenGivenBoundsIntersect) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 4, 2);
  Bounds bound2 = Bounds_Of(6, 6, 2, 2);

  // assert
  Bounds embed = Bounds_Embed(&bound1, &bound2);

  // assert
  assert(embed.center.x == 6);
  assert(embed.center.y == 5);
  assert(embed.size.x == 2);
  assert(embed.size.y == 1);
}

test(Embed_ShouldReturnEmbeddedBoundsWhenGivenBoundsContainEachOther) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 4, 2);
  Bounds bound2 = Bounds_Of(4, 4, 2, 2);

  // assert
  Bounds embed = Bounds_Embed(&bound1, &bound2);

  // assert
  assert(embed.center.x == 4);
  assert(embed.center.y == 4);
  assert(embed.size.x == 2);
  assert(embed.size.y == 2);
}

suite(
  Intersects_ShouldReturnNoHitWhenBoundsDoNotIntersect,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnTopEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnRightEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnBottomEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnLeftEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsAreEmbedded,
  Expand_ShouldReturnExpandedBoundsWhenGivenBoundsIntersect,
  Expand_ShouldReturnExpandedBoundsWhenGivenBoundsDoNotIntersect,
  Expand_ShouldReturnExpandedBoundsWhenGivenBoundsContainEachOther,
  Embed_ShouldReturnEmbeddedBoundsWhenGivenBoundsIntersect,
  Embed_ShouldReturnEmbeddedBoundsWhenGivenBoundsContainEachOther);
