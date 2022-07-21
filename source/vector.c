
#include <vector.h>
#include <math.h>

bool
Vector_Equals(
    const Vector *vector,
    const Vector *other)
{
  return vector->x == other->x
    && vector->y == other->y;
}

int
Vector_GetLength(const Vector *vector) {
  return vector->x * vector->x + vector->y * vector->y;
}

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

bool
Bounds_Intersects(
    const Bounds *bounds,
    const Bounds *other)
{
  int dx = other->center.x - bounds->center.x;
  int px = (other->size.x + bounds->size.x) - Math_abs(dx);

  if (px <= 0) {
    return false;
  }

  int dy = other->center.y - bounds->center.y;
  int py = (other->size.y + bounds->size.y) - Math_abs(dy);

  if (py <= 0) {
    return false;
  }

  return true;
}
