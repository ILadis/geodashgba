
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
  camera->frame.center = Vector_Of(54, 96);
  camera->frame.threshold[DIRECTION_UP] = 40;
  camera->frame.threshold[DIRECTION_DOWN] = 10;
  camera->frame.threshold[DIRECTION_LEFT] = 22;
  camera->frame.threshold[DIRECTION_RIGHT] = 22;
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
  Vector *frame = &camera->frame.center;

  int dx = target->x - frame->x;
  int dy = target->y - frame->y;

  Vector delta = Vector_Of(0, 0);

  Direction horizontal = dx < 0 ? DIRECTION_LEFT : DIRECTION_RIGHT;
  Direction vertical   = dy < 0 ? DIRECTION_UP   : DIRECTION_DOWN;

  int tx = camera->frame.threshold[horizontal];
  int ty = camera->frame.threshold[vertical];

  int px = Math_abs(dx) - tx;
  int py = Math_abs(dy) - ty;

  if (px > 0) {
    int sx = Math_signum(dx);
    delta.x = sx * px;
  }

  if (py > 0) {
    int sy = Math_signum(dy);
    delta.y = sy * py; // TODO smooth out using bezier
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
