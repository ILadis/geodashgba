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

#endif
