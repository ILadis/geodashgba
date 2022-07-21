
#include <vector.h>
#include "test.h"

test(Equals_ShouldReturnTrueIfVectorsAreEqual) {
  // arrange
  Vector v1 = { 3, 7 };
  Vector v2 = { 3, 7 };

  // assert
  bool equal = Vector_Equals(&v1, &v2);

  // assert
  assert(equal == true);
}

test(Equals_ShouldReturnFlaseIfVectorsAreNotEqual) {
  // arrange
  Vector v1 = { 3, 7 };
  Vector v2 = { 4, 7 };
  Vector v3 = { 3, 8 };

  // assert
  bool equal1 = Vector_Equals(&v1, &v2);
  bool equal2 = Vector_Equals(&v1, &v3);
  bool equal3 = Vector_Equals(&v2, &v3);

  // assert
  assert(equal1 == false);
  assert(equal2 == false);
  assert(equal3 == false);
}

test(FromDirection_ShouldReturnExpectedLeftFacingVector) {
  // arrange
  Vector left = { -1, 0 };

  // act
  const Vector *v = Vector_FromDirection(DIRECTION_LEFT);

  // assert
  assert(Vector_Equals(v, &left) == true);
}

test(FromDirection_ShouldReturnExpectedRightFacingVector) {
  // arrange
  Vector right = { +1, 0 };

  // act
  const Vector *v = Vector_FromDirection(DIRECTION_RIGHT);

  // assert
  assert(Vector_Equals(v, &right) == true);
}

test(FromDirection_ShouldReturnExpectedUpFacingVector) {
  // arrange
  Vector up = { 0, -1 };

  // act
  const Vector *v = Vector_FromDirection(DIRECTION_UP);

  // assert
  assert(Vector_Equals(v, &up) == true);
}

test(FromDirection_ShouldReturnExpectedDownFacingVector) {
  // arrange
  Vector down = { 0, +1 };

  // act
  const Vector *v = Vector_FromDirection(DIRECTION_DOWN);

  // assert
  assert(Vector_Equals(v, &down) == true);
}

suite(
  Equals_ShouldReturnTrueIfVectorsAreEqual,
  Equals_ShouldReturnFlaseIfVectorsAreNotEqual,
  FromDirection_ShouldReturnExpectedLeftFacingVector,
  FromDirection_ShouldReturnExpectedRightFacingVector,
  FromDirection_ShouldReturnExpectedUpFacingVector,
  FromDirection_ShouldReturnExpectedDownFacingVector);
