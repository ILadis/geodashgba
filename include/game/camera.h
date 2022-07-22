#ifndef CAMERA_H
#define CAMERA_H

#include <gba.h>
#include <vector.h>

typedef struct Camera {
  Vector position;
  const Vector *target;
} Camera;

Camera*
Camera_GetInstance();

void
Camera_Reset(Camera *camera);

static inline void
Camera_RelativeTo(
    Camera *camera,
    Vector *position)
{
  position->x -= camera->position.x;
  position->y -= camera->position.y;
}

static inline void
Camera_FollowTarget(
    Camera *camera,
    const Vector *position)
{
  camera->target = position;
}

void
Camera_Update(Camera *camera);

#endif
