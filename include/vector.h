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

bool
Vector_Equals(
    const Vector* vector,
    const Vector* other);

int
Vector_GetLength(const Vector *vector);

const Vector*
Vector_FromDirection(Direction direction);

#endif
