
#include <vector.h>
#include <math.h>

const Vector*
Vector_FromDirection(Direction direction) {
  static const Vector directions[] = {
    [DIRECTION_LEFT]  = { .x = -1, .y = 0 },
    [DIRECTION_RIGHT] = { .x = +1, .y = 0 },
    [DIRECTION_UP]    = { .y = -1, .x = 0 },
    [DIRECTION_DOWN]  = { .y = +1, .x = 0 },
  };

  return &directions[direction];
}

Direction
Direction_InverseOf(Direction direction) {
  static const Direction inverses[] = {
    [DIRECTION_LEFT]  = DIRECTION_RIGHT,
    [DIRECTION_RIGHT] = DIRECTION_LEFT,
    [DIRECTION_UP]    = DIRECTION_DOWN,
    [DIRECTION_DOWN]  = DIRECTION_UP,
  };

  return inverses[direction];
}

bool
Direction_IsHorizontal(Direction direction) {
  return direction <= DIRECTION_RIGHT;
}
