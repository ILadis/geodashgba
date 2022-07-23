
#include <game/camera.h>

Camera*
Camera_GetInstance() {
  static Camera camera = {0};
  return &camera;
}

void
Camera_Reset(Camera *camera) {
  camera->position = Vector_Of(0, 0);
  camera->offset = Vector_Of(56, 128);
  camera->target = NULL;
}

void
Camera_Update(Camera *camera) {
  const Vector *target = camera->target;

  if (target != NULL) {
    int x = target->x - camera->offset.x;
    int y = target->y - camera->offset.y;

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    camera->position.x = x;
    camera->position.y = y;
  }
}
