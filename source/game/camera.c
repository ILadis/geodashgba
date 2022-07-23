
#include <game/camera.h>

Camera*
Camera_GetInstance() {
  static Camera camera = {0};
  return &camera;
}

void
Camera_Reset(Camera *camera) {
  camera->position = Vector_Of(0, 0);
  camera->frame = Bounds_Of(54, 96, 22, 40);
  camera->target = NULL;
}

static inline Vector
Camera_TargetInFrame(Camera *camera) {
  Vector *target = camera->target;
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
  Vector *target = camera->target;

  if (target != NULL) {
    Vector delta = Camera_TargetInFrame(camera);

    camera->frame.center.x += delta.x;
    camera->frame.center.y += delta.y;

    camera->position.x += delta.x;
    camera->position.y += delta.y;

    // TODO keep camera in bounds?
  }
}
