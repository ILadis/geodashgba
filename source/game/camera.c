
#include <game/camera.h>

Camera*
Camera_GetInstance() {
  static Camera camera = {0};
  return &camera;
}

void
Camera_Reset(Camera *camera) {
  camera->position = Vector_Of(0, 0);
  camera->viewport = Bounds_Of(120, 80, 120, 80);
  camera->frame = Bounds_Of(54, 96, 22, 40);
  camera->limit.lower = Vector_Of(0, 0);
  camera->limit.upper = Vector_Of(0, 0);
  camera->target = NULL;
}

static inline void
Camera_MoveBy(
    Camera *camera,
    Vector *delta)
{
  int x = camera->position.x + delta->x;
  int y = camera->position.y + delta->y;

  const Vector *lower = &camera->limit.lower;
  const Vector *upper = &camera->limit.upper;

  x = Math_clamp(x, lower->x, upper->x);
  y = Math_clamp(y, lower->y, upper->y);

  int dx = x - camera->position.x;
  int dy = y - camera->position.y;

  camera->position.x = x;
  camera->position.y = y;

  camera->viewport.center.x += dx;
  camera->viewport.center.y += dy;

  camera->frame.center.x += dx;
  camera->frame.center.y += dy;
}

static inline Vector
Camera_GetTargetDelta(Camera *camera) {
  const Vector *target = camera->target;
  Bounds *frame = &camera->frame;

  int dx = target->x - frame->center.x;
  int dy = target->y - frame->center.y;

  Vector delta = Vector_Of(0, 0);

  int px = Math_abs(dx) - frame->size.x;
  int py = Math_abs(dy) - frame->size.y;

  if (px > 0) {
    int sx = Math_signum(dx);
    delta.x = sx * px;
  }

  if (py > 0) {
    int sy = Math_signum(dy);
    delta.y = sy * py;
  }

  return delta;
}

void
Camera_Update(Camera *camera) {
  const Vector *target = camera->target;

  if (target != NULL) {
    Vector delta = Camera_GetTargetDelta(camera);
    Camera_MoveBy(camera, &delta);
  }
}
