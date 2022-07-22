
#include <game/camera.h>

Camera*
Camera_GetInstance() {
  static Camera camera = {0};
  return &camera;
}

void
Camera_Reset(Camera *camera) {
  camera->position = Vector_Of(0, 0);
  camera->target = NULL;
}

void
Camera_Update(Camera *camera) {
  const Vector *target = camera->target;

  if (target != NULL) {
    camera->position.x = target->x;
    camera->position.y = target->y;
  }
}
