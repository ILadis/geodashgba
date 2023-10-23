
#include <hit.h>

Hit
Hit_Combine(
    const Hit *hit,
    const Hit *other)
{
  int x1 = Math_abs(other->delta.x);
  int y1 = Math_abs(other->delta.y);

  int x2 = Math_abs(hit->delta.x);
  int y2 = Math_abs(hit->delta.y);

  int dx = hit->delta.x;
  int px = hit->position.x;

  int dy = hit->delta.y;
  int py = hit->position.y;

  if (x1 > x2) {
    dx = other->delta.x;
    px = other->position.x;
  }

  if (y1 > y2) {
    dy = other->delta.y;
    py = other->position.y;
  }

  return (Hit) {
    .position = Vector_Of(px, py),
    .delta = Vector_Of(dx, dy),
  };
}

iwram bool
Shape_Intersects(
    const Shape *shape,
    const Shape *other)
{
  for (unsigned int i = 0; i < shape->length; i++) {
    Vector axis = Shape_GetAxis(shape, i);

    Projection p1 = Shape_ProjectOnto(shape, &axis);
    Projection p2 = Shape_ProjectOnto(other, &axis);

    if (!Projection_Overlap(&p1, &p2)) {
      return false;
    }
  }

  for (unsigned int i = 0; i < other->length; i++) {
    Vector axis = Shape_GetAxis(other, i);

    Projection p1 = Shape_ProjectOnto(shape, &axis);
    Projection p2 = Shape_ProjectOnto(other, &axis);

    if (!Projection_Overlap(&p1, &p2)) {
      return false;
    }
  }

  return true;
}

iwram Hit
Raycast_Intersects(
    const Raycast *raycast,
    const Bounds *bounds)
{
  int dx = raycast->direction.x;
  int dy = raycast->direction.y;

  Vector step = Vector_Of(1, 1);

  if (dx < 0) {
    dx = -dx;
    step.x = -1;
  }

  if (dy < 0) {
    dy = -dy;
    step.y = -1;
  }

  int xerror = 2 * dx;
  int yerror = 2 * dy;

  Vector point = raycast->position;
  unsigned int length = raycast->length;

  Hit hit = {0};

  if (dy <= dx) {
    int error = -dx;

    do {
      hit = Bounds_Contains(bounds, &point);
      if (Hit_IsHit(&hit)) {
        break;
      }

      error += yerror;
      if (error > 0) {
        point.y += step.y;
        error -= xerror;
      }

      point.x += step.x;
    } while (length--);
  }

  else {
    int error = -dy;

    do {
      hit = Bounds_Contains(bounds, &point);
      if (Hit_IsHit(&hit)) {
        break;
      }

      error += xerror;
      if (error > 0) {
        point.x += step.x;
        error -= yerror;
      }

      point.y += step.y;
    } while (length--);
  }

  return hit;
}

iwram Hit
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
    int sx = dx < 0 ? -1 : + 1;
    hit.delta.x = px * sx;
    hit.position.x = bounds->center.x + (bounds->size.x * sx);
    hit.position.y = other->center.y;
  } else {
    int sy = dy < 0 ? -1 : + 1;
    hit.delta.y = py * sy;
    hit.position.x = other->center.x;
    hit.position.y = bounds->center.y + (bounds->size.y * sy);
  }

  return hit;
}

iwram Hit
Bounds_Contains(
    const Bounds *bounds,
    const Vector *point)
{
  Hit hit = {0};

  int dx = point->x - bounds->center.x;
  int px = bounds->size.x - Math_abs(dx);

  if (px <= 0) {
    return hit;
  }

  int dy = point->y - bounds->center.y;
  int py = bounds->size.y - Math_abs(dy);

  if (py <= 0) {
    return hit;
  }

  if (px < py) {
    int sx = dx < 0 ? -1 : + 1;
    hit.delta.x = px * sx;
    hit.position.x = bounds->center.x + (bounds->size.x * sx);
    hit.position.y = point->y;
  } else {
    int sy = dy < 0 ? -1 : + 1;
    hit.delta.y = py * sy;
    hit.position.x = point->x;
    hit.position.y = bounds->center.y + (bounds->size.y * sy);
  }

  return hit;
}

void
Bounds_Rotate(
    const Bounds *bounds,
    Shape *shape,
    int angle)
{
  const Vector *position = &bounds->center;
  const Vector *size = &bounds->size;

  const Vector vertices[] = {
    { -size->x, -size->y },
    { +size->x, -size->y },
    { +size->x, +size->y },
    { -size->x, +size->y },
  };

  for (unsigned int i = 0; i < length(vertices); i++) {
    int px = vertices[i].x;
    int py = vertices[i].y;

    int dx = Math_round((Math_cos(angle) * px - Math_sin(angle) * py), 8);
    int dy = Math_round((Math_cos(angle) * py + Math_sin(angle) * px), 8);

    shape->vertices[i].x = position->x + dx;
    shape->vertices[i].y = position->y + dy;
  }
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
Bounds_Enlarge(
    const Bounds *bounds,
    const Vector *size)
{
  return (Bounds) {
    .center = bounds->center,
    .size = {
      .x = bounds->size.x + size->x,
      .y = bounds->size.y + size->y,
    }
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

iwram Vector
Shape_GetAxis(
    const Shape *shape,
    unsigned int index)
{
  Vector axis = Vector_Of(0, 0);
  if (index > shape->length) {
    return axis;
  }

  const Vector *p1 = &shape->vertices[index++];
  const Vector *p2 = &shape->vertices[index == shape->length ? 0 : index];

  axis.x = -(p2->y - p1->y);
  axis.y =   p2->x - p1->x;

  return axis;
}

iwram Projection
Shape_ProjectOnto(
    const Shape *shape,
    const Vector *axis)
{
  int min = Vector_DotProduct(axis, &shape->vertices[0]);
  int max = min;

  for (unsigned int i = 1; i < shape->length; i++) {
    int p = Vector_DotProduct(axis, &shape->vertices[i]);

    if (p < min) {
      min = p;
    } else if (p > max) {
      max = p;
    }
  }

  return Projection_Of(axis, min, max);
}
