
#include <hit.h>
#include "test.h"

test(Intersects_ShouldReturnHitWhenRaycastIntersectsBounds) {
  // arrange
  Bounds bounds = Bounds_Of(5, 4, 3, 2);
  Raycast casts[] = {
    Raycast_Of(1, 4, 4, 3, 3),
    Raycast_Of(1, 4, 6,-1, 5),
    Raycast_Of(1, 4, 3, 0, 2),
    Raycast_Of(1, 1, 3, 2, 3),
    Raycast_Of(2, 8, 2,-5, 3),
  };

  // act
  for (int i = 0; i < length(casts); i++) {
    Hit hit = Raycast_Intersects(&casts[i], &bounds);

    // assert
    assert(hit.delta.x != 0 || hit.delta.y != 0);
  }
}

test(Intersects_ShouldReturnNoHitWhenRaycastDoesNotIntersectBounds) {
  // arrange
  Bounds bounds = Bounds_Of(5, 4, 3, 2);
  Raycast casts[] = {
    Raycast_Of(2, 8, 2,-2, 2),
    Raycast_Of(2, 8,-1, 3, 3),
    Raycast_Of(2, 8, 3, 1, 9),
    Raycast_Of(9, 3, 2, 2, 3),
    Raycast_Of(5,-1, 0, 1, 3),
  };

  // act
  for (int i = 0; i < length(casts); i++) {
    Hit hit = Raycast_Intersects(&casts[i], &bounds);

    // assert
    assert(hit.delta.x == 0);
    assert(hit.delta.y == 0);
  }
}

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
  Bounds bound2 = Bounds_Of(3, 2, 1, 2);

  // assert
  Hit hit = Bounds_Intersects(&bound1, &bound2);

  // assert
  assert(hit.delta.x ==  0);
  assert(hit.delta.y == -2);
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

test(Intersects_ShouldReturnHitWithDeltaWhenBoundsHaveSameCenter) {
  // arrange
  Bounds bound1 = Bounds_Of(5, 4, 5, 3);
  Bounds bound2 = Bounds_Of(5, 4, 2, 2);

  // assert
  Hit hit = Bounds_Intersects(&bound1, &bound2);

  // assert
  assert(hit.delta.x == 0);
  assert(hit.delta.y == 5);
}

test(Intersects_ShouldReturnTrueForGivenShapes) {
  // arrange
  const Vector vertices1[] = {
    Vector_Of(5, 2),
    Vector_Of(10, 4),
    Vector_Of(7, 11),
    Vector_Of(1, 3),
  };

  const Vector vertices2[] = {
    Vector_Of(7, 6),
    Vector_Of(16, 6),
    Vector_Of(10, 10),
  };

  const Vector vertices3[] = {
    Vector_Of(9, 8),
    Vector_Of(9, 10),
    Vector_Of(6, 11),
  };

  const Vector vertices4[] = {
    Vector_Of(152, 464),
    Vector_Of(144, 480),
    Vector_Of(160, 480),
  };

  const Vector vertices5[] = {
    Vector_Of(135, 464),
    Vector_Of(151, 464),
    Vector_Of(151, 480),
    Vector_Of(135, 480),
  };

  const Shape shape1 = Shape_Of(vertices1);
  const Shape shape2 = Shape_Of(vertices2);
  const Shape shape3 = Shape_Of(vertices3);
  const Shape shape4 = Shape_Of(vertices4);
  const Shape shape5 = Shape_Of(vertices5);

  // act
  bool result1 = Shape_Intersects(&shape1, &shape2);
  bool result2 = Shape_Intersects(&shape2, &shape3);
  bool result3 = Shape_Intersects(&shape3, &shape1);
  bool result4 = Shape_Intersects(&shape4, &shape5);

  // assert
  assert(result1 == true);
  assert(result2 == true);
  assert(result3 == true);
  assert(result4 == true);
}

test(Intersects_ShouldReturnFalseForGivenShapes) {
  // arrange
  const Vector vertices1[] = {
    Vector_Of(5, 2),
    Vector_Of(10, 4),
    Vector_Of(7, 11),
    Vector_Of(1, 3),
  };

  const Vector vertices2[] = {
    Vector_Of(9, 7),
    Vector_Of(16, 6),
    Vector_Of(10, 10),
  };

  const Vector vertices3[] = {
    Vector_Of(9, 8),
    Vector_Of(9, 10),
    Vector_Of(7, 12),
  };

  const Shape shape1 = Shape_Of(vertices1);
  const Shape shape2 = Shape_Of(vertices2);
  const Shape shape3 = Shape_Of(vertices3);

  // act
  bool result1 = Shape_Intersects(&shape1, &shape2);
  bool result2 = Shape_Intersects(&shape2, &shape3);
  bool result3 = Shape_Intersects(&shape3, &shape1);

  // assert
  assert(result1 == false);
  assert(result2 == false);
  assert(result3 == false);
}

test(Contains_ShouldReturnHitWhenPointIsWithinBounds) {
  // arrange
  Bounds bounds = Bounds_Of(5, 4, 3, 2);
  Vector points[] = {
    Vector_Of(3, 3),
    Vector_Of(7, 5),
    Vector_Of(5, 4),
    Vector_Of(4, 5),
  };

  // act
  for (int i = 0; i < length(points); i++) {
    Hit hit = Bounds_Contains(&bounds, &points[i]);

    // assert
    assert(hit.delta.x != 0 || hit.delta.y != 0);
  }
}

test(Contains_ShouldReturnNoHitWhenPointIsNotWithinBounds) {
  // arrange
  Bounds bounds = Bounds_Of(5, 4, 3, 2);
  Vector points[] = {
    Vector_Of(2, 2),
    Vector_Of(1, 4),
    Vector_Of(3, 8),
    Vector_Of(8, 6),
    Vector_Of(9, 4),
    Vector_Of(7, 1),
  };

  // act
  for (int i = 0; i < length(points); i++) {
    Hit hit = Bounds_Contains(&bounds, &points[i]);

    // assert
    assert(hit.delta.x == 0);
    assert(hit.delta.y == 0);
  }
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
  Intersects_ShouldReturnHitWhenRaycastIntersectsBounds,
  Intersects_ShouldReturnNoHitWhenRaycastDoesNotIntersectBounds,
  Intersects_ShouldReturnNoHitWhenBoundsDoNotIntersect,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnTopEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnRightEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnBottomEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsIntersectOnLeftEdge,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsAreEmbedded,
  Intersects_ShouldReturnHitWithDeltaWhenBoundsHaveSameCenter,
  Intersects_ShouldReturnTrueForGivenShapes,
  Intersects_ShouldReturnFalseForGivenShapes,
  Contains_ShouldReturnHitWhenPointIsWithinBounds,
  Contains_ShouldReturnNoHitWhenPointIsNotWithinBounds,
  Expand_ShouldReturnExpandedBoundsWhenGivenBoundsIntersect,
  Expand_ShouldReturnExpandedBoundsWhenGivenBoundsDoNotIntersect,
  Expand_ShouldReturnExpandedBoundsWhenGivenBoundsContainEachOther,
  Embed_ShouldReturnEmbeddedBoundsWhenGivenBoundsIntersect,
  Embed_ShouldReturnEmbeddedBoundsWhenGivenBoundsContainEachOther);
