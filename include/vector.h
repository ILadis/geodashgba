#ifndef VECTOR_H
#define VECTOR_H

#include <types.h>

typedef enum Direction {
  DIRECTION_LEFT,
  DIRECTION_RIGHT,
  DIRECTION_UP,
  DIRECTION_DOWN,
} Direction;

Direction
Direction_InverseOf(Direction direction);

bool
Direction_IsHorizontal(Direction direction);

typedef struct Vector {
  int x;
  int y;
} Vector;

#define Vector_Of(x, y) ((Vector) { x, y })

static inline int
Vector_GetLength(const Vector *vector) {
  return vector->x * vector->x + vector->y * vector->y;
}

static inline void
Vector_Rshift(Vector *vector, int value) {
  vector->x >>= value;
  vector->y >>= value;
}

static inline int
Vector_DotProduct(
    const Vector *vector,
    const Vector *other)
{
  return vector->x * other->x + vector->y * other->y;
}

static inline bool
Vector_Equals(
    const Vector *vector,
    const Vector *other)
{
  return vector->x == other->x
    && vector->y == other->y;
}

const Vector*
Vector_FromDirection(Direction direction);

#endif
