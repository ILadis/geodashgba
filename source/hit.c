
#include <hit.h>

Hit
Bounds_Intersects(
    const Bounds *bounds,
    const Bounds *other)
{
  Hit hit = {0};

  int dx = other->center.x - bounds->center.x;
  int px = (other->size.x + bounds->size.x) - Math_abs(dx);

  if (px <= 0) {
    return hit;
  }

  int dy = other->center.y - bounds->center.y;
  int py = (other->size.y + bounds->size.y) - Math_abs(dy);

  if (py <= 0) {
    return hit;
  }

  if (px < py) {
    int sx = Math_signum(dx);
    hit.delta.x = px * sx;
    hit.position.x = bounds->center.x + (bounds->size.x * sx);
    hit.position.y = bounds->center.y;
  } else {
    int sy = Math_signum(dy);
    hit.delta.y = py * sy;
    hit.position.x = other->center.x;
    hit.position.y = bounds->center.y + (bounds->size.y * sy);
  }

  return hit;
}

static Bounds
Bounds_Combine(
    const Bounds *bounds,
    const Bounds *other,
    int (*low)(int x, int y),
    int  (*up)(int x, int y))
{
  Vector l1 = Bounds_Lower(bounds);
  Vector l2 = Bounds_Lower(other);

  Vector lower = {
    .x = low(l1.x, l2.x),
    .y = low(l1.y, l2.y),
  };

  Vector u1 = Bounds_Upper(bounds);
  Vector u2 = Bounds_Upper(other);

  Vector upper = {
    .x = up(u1.x, u2.x),
    .y = up(u1.y, u2.y),
  };

  Vector center = {
    .x = (lower.x + upper.x) >> 1,
    .y = (lower.y + upper.y) >> 1,
  };

  Vector size = {
    .x = (upper.x - lower.x) >> 1,
    .y = (upper.y - lower.y) >> 1,
  };

  return (Bounds) {
    .center = center,
    .size = size,
  };
}

Bounds
Bounds_Expand(
    const Bounds *bounds,
    const Bounds *other)
{
  return Bounds_Combine(bounds, other, Math_min, Math_max);
}

Bounds
Bounds_Embed(
    const Bounds *bounds,
    const Bounds *other)
{
  return Bounds_Combine(bounds, other, Math_max, Math_min);
}
