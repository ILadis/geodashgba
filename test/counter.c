
#include <counter.h>
#include "test.h"

test(Reset_ShouldSetAllDigitsBlank) {
  // arrange
  char text[4];

  // act
  Counter counter = text;
  Counter_Reset(counter, length(text));

  // assert
  assert(text[0] == ' ');
  assert(text[1] == ' ');
  assert(text[2] == ' ');
  assert(text[3] == '\0');
}

test(IsBlank_ShouldReturnTrueWhenCounterIsResetted) {
  // arrange
  char text[4];

  Counter counter = text;
  Counter_Reset(counter, length(text));

  // act
  bool result = Counter_IsBlank(counter);

  // assert
  assert(result == true);
}

test(IsBlank_ShouldReturnFalseWhenCounterIsIncremented) {
  // arrange
  char text[4];

  Counter counter = text;
  Counter_Reset(counter, length(text));

  // act
  Counter_IncrementOne(counter);
  bool result = Counter_IsBlank(counter);

  // assert
  assert(result == false);
}

test(IsBlank_ShouldReturnFalseWhenCounterOverflows) {
  // arrange
  char text[4];

  Counter counter = text;
  Counter_Reset(counter, length(text));

  for (int i = 0; i < 1000; i++) {
    Counter_IncrementOne(counter);
  }

  // act
  bool result = Counter_IsBlank(counter);

  // assert
  assert(result == false);
}

test(IncrementOne_ShouldReturnDigitOne) {
  // arrange
  char text[4];

  Counter counter = text;
  Counter_Reset(counter, length(text));

  // act
  Counter_IncrementOne(counter);

  // assert
  assert(text[0] == '1');
  assert(text[1] == ' ');
  assert(text[2] == ' ');
  assert(text[3] == '\0');
}

test(IncrementOne_ShouldReturnDigitThree) {
  // arrange
  char text[4];

  Counter counter = text;
  Counter_Reset(counter, length(text));

  // act
  Counter_IncrementOne(counter);
  Counter_IncrementOne(counter);
  Counter_IncrementOne(counter);

  // assert
  assert(text[0] == '3');
  assert(text[1] == ' ');
  assert(text[2] == ' ');
  assert(text[3] == '\0');
}

test(IncrementOne_ShouldAllocateNextDigit) {
  // arrange
  char text[4];

  Counter counter = text;
  Counter_Reset(counter, length(text));

  // act
  for (int i = 0; i < 17; i++) {
    Counter_IncrementOne(counter);
  }

  // assert
  assert(text[0] == '1');
  assert(text[1] == '7');
  assert(text[2] == ' ');
  assert(text[3] == '\0');
}

test(IncrementOne_ShouldCountUptoMaximum) {
  // arrange
  char text[4];

  Counter counter = text;
  Counter_Reset(counter, length(text));

  // act
  for (int i = 0; i < 999; i++) {
    Counter_IncrementOne(counter);
  }

  // assert
  assert(text[0] == '9');
  assert(text[1] == '9');
  assert(text[2] == '9');
  assert(text[3] == '\0');
}

test(IncrementOne_ShouldOverflowAtMaximumAndReset) {
  // arrange
  char text[4];

  Counter counter = text;
  Counter_Reset(counter, length(text));

  // act
  for (int i = 0; i < 998; i++) {
    Counter_IncrementOne(counter);
  }

  Counter_IncrementOne(counter);
  Counter_IncrementOne(counter);

  // assert
  assert(text[0] == '1');
  assert(text[1] == ' ');
  assert(text[2] == ' ');
  assert(text[3] == '\0');
}

suite(
  Reset_ShouldSetAllDigitsBlank,
  IsBlank_ShouldReturnTrueWhenCounterIsResetted,
  IsBlank_ShouldReturnFalseWhenCounterIsIncremented,
  IsBlank_ShouldReturnFalseWhenCounterOverflows,
  IncrementOne_ShouldReturnDigitOne,
  IncrementOne_ShouldReturnDigitThree,
  IncrementOne_ShouldAllocateNextDigit,
  IncrementOne_ShouldCountUptoMaximum,
  IncrementOne_ShouldOverflowAtMaximumAndReset);
