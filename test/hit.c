
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

suite(
  Intersects_ShouldReturnNoHitWhenBoundsDoNotIntersect,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnTopEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnRightEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnBottomEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnLeftEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsAreEmbedded);
