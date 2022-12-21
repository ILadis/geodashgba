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

typedef struct Shape {
  const Vector *vertices;
  unsigned int length;
} Shape;

typedef struct Projection {
  const Vector *axis;
  int min, max;
} Projection;

typedef struct Raycast {
  Vector position;
  Vector direction;
  unsigned int length;
} Raycast;

#define Bounds_Of(x, y, w, h) ((Bounds) {{ x, y }, { w, h }})
#define Shape_Of(vertices) ((Shape) { vertices, length(vertices) })
#define Projection_Of(axis, min, max) ((Projection) { axis, min, max })
#define Raycast_Of(x, y, dx, dy, l) ((Raycast) {{ x, y }, { dx, dy }, l })

static inline bool
Hit_IsHit(Hit *hit) {
  return hit->delta.x != 0 || hit->delta.y != 0;
}

Hit
Hit_Combine(
    const Hit *hit,
    const Hit *other);

bool
Shape_Intersects(
    const Shape *shape,
    const Shape *other);

Hit
Raycast_Intersects(
    const Raycast *raycast,
    const Bounds *bounds);

Hit
Bounds_Intersects(
    const Bounds *bounds,
    const Bounds *other);

Hit
Bounds_Contains(
    const Bounds *bounds,
    const Vector *point);

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
Bounds_Enlarge(
    const Bounds *bounds,
    const Vector *size);

Bounds
Bounds_Expand(
    const Bounds *bounds,
    const Bounds *other);

Bounds
Bounds_Embed(
    const Bounds *bounds,
    const Bounds *other);

Vector
Shape_GetAxis(
    const Shape *shape,
    int index);

Projection
Shape_ProjectOnto(
    const Shape *shape,
    const Vector *axis);

static inline bool
Projection_Overlap(
    const Projection *project,
    const Projection *other)
{
  return (project->min > other->min   && project->min < other->max)
      || (  other->min > project->min &&   other->min < project->max);
}

#endif
