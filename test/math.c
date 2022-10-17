
#include <math.h>
#include "test.h"

test(Abs_ShouldReturnPositiveIntegerForNegativeNumber) {
  // arrange
  int number = -7;

  // act
  int abs = Math_abs(number);

  // assert
  assert(abs == 7);
}

test(Abs_ShouldReturnSameIntegerForPositiveNumber) {
  // arrange
  int number = +7;

  // act
  int abs = Math_abs(number);

  // assert
  assert(abs == 7);
}

test(Rand_ShouldReturnExpectedNumberSequence) {
  // act
  int num1 = Math_rand();
  int num2 = Math_rand();
  int num3 = Math_rand();
  int num4 = Math_rand();

  // assert
  assert(num1 == 12345);
  assert(num2 == 1406932606);
  assert(num3 == 654583775);
  assert(num4 == 1449466924);
}

suite(
  Abs_ShouldReturnPositiveIntegerForNegativeNumber,
  Abs_ShouldReturnSameIntegerForPositiveNumber,
  Rand_ShouldReturnExpectedNumberSequence);
