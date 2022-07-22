
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
