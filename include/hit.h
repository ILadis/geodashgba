#ifndef HIT_H
#define HIT_H

#include <vector.h>
#include <math.h>

typedef struct Hit {
  Vector position;
  Vector delta;
} Hit;

typedef struct Bounds {
  Vector center;
  Vector size;
} Bounds;

#define Bounds_Of(x, y, w, h) ((Bounds) {{ x, y }, { w, h }})

static inline bool
Hit_IsHit(Hit *hit) {
  return hit->delta.x != 0 || hit->delta.y != 0;
}

Hit
Bounds_Intersects(
    const Bounds *bounds,
    const Bounds *other);

static inline Vector
Bounds_Lower(const Bounds *bounds) {
  return (Vector) {
    .x = bounds->center.x - bounds->size.x,
    .y = bounds->center.y - bounds->size.y,
  };
}

static inline Vector
Bounds_Upper(const Bounds *bounds) {
  return (Vector) {
    .x = bounds->center.x + bounds->size.x,
    .y = bounds->center.y + bounds->size.y,
  };
}

static inline bool
Bounds_Equals(
    const Bounds *bounds,
    const Bounds *other)
{
  return bounds->center.x == other->center.x && bounds->center.y == other->center.y
      && bounds->size.x == other->size.x && bounds->size.y == other->size.y;
}

Bounds
Bounds_Expand(
    const Bounds *bounds,
    const Bounds *other);

Bounds
Bounds_Embed(
    const Bounds *bounds,
    const Bounds *other);

#endif
