
#include <math.h>
#include "test.h"

test(Abs_ShouldReturnPositiveIntegerForNegativeNumber) {
  // arrange
  int number = -7;

  // assert
  int abs = Math_abs(number);

  // assert
  assert(abs == 7);
}

test(Abs_ShouldReturnSameIntegerForPositiveNumber) {
  // arrange
  int number = +7;

  // assert
  int abs = Math_abs(number);

  // assert
  assert(abs == 7);
}

suite(
  Abs_ShouldReturnPositiveIntegerForNegativeNumber,
  Abs_ShouldReturnSameIntegerForPositiveNumber);
