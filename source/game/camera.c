
#include <game/camera.h>

Camera*
Camera_GetInstance() {
  static Camera camera = {0};
  return &camera;
}

void
Camera_Reset(Camera *camera) {
  camera->position = Vector_Of(0, 0);
  camera->delta = Vector_Of(0, 0);
  camera->viewport = Bounds_Of(120, 80, 120, 80);
  camera->frame = Bounds_Of(54, 96, 22, 40);
  camera->target = NULL;
}

static inline void
Camera_MoveBy(
    Camera *camera,
    Vector *delta)
{
  int x = camera->position.x += delta->x;
  int y = camera->position.y += delta->y;

  if (x < 0) {
    camera->position.x = 0;
    delta->x -= x;
  }

  if (y < 0) {
    camera->position.y = 0;
    delta->y -= y;
  }

  camera->viewport.center.x += delta->x;
  camera->viewport.center.y += delta->y;

  camera->frame.center.x += delta->x;
  camera->frame.center.y += delta->y;
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

static inline void
Camera_SetDelta(
    Camera *camera,
    Vector *delta)
{
  int dx = ((camera->position.x & 0b0111) + delta->x) >> 3;
  int dy = ((camera->position.y & 0b0111) + delta->y) >> 3;

  camera->delta.x = dx;
  camera->delta.y = dy;
}

void
Camera_Update(Camera *camera) {
  const Vector *target = camera->target;

  if (target != NULL) {
    Vector delta = Camera_GetTargetDelta(camera);

    Camera_MoveBy(camera, &delta);
    Camera_SetDelta(camera, &delta);
  }
}
