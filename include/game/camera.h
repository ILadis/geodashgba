#ifndef CAMERA_H
#define CAMERA_H

#include <gba.h>
#include <vector.h>
#include <hit.h>
#include <math.h>

typedef struct Camera {
  Vector position;
  Bounds viewport;
  struct {
    Vector center;
    int threshold[4];
  } frame;
  struct {
    Vector lower;
    Vector upper;
  } limit;
  struct {
    int duration;
    int intensity;
    Vector position;
  } shake;
  const Vector *target;
} Camera;

Camera*
Camera_GetInstance();

void
Camera_Reset(Camera *camera);

static inline Vector*
Camera_GetPosition(Camera *camera) {
  return &camera->position;
}

static inline Bounds*
Camera_GetViewport(Camera *camera) {
  return &camera->viewport;
}

static inline void
Camera_SetUpperLimit(
    Camera *camera,
    const Vector *limit)
{
  /* Adjust limit to the top-left edge. This is necessary because the
   * camera position is also stored as the top-left edge. The limit
   * provided here represents the bottom-right position that should
   * not be exceeded.
   */
  camera->limit.upper.x = limit->x - camera->viewport.size.x * 2;
  camera->limit.upper.y = limit->y - camera->viewport.size.y * 2;
}

static inline bool
Camera_InViewport(
    Camera *camera,
    const Bounds *bounds)
{
  Hit hit = Bounds_Intersects(&camera->viewport, bounds);
  return Hit_IsHit(&hit);
}

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
Camera_Shake(Camera *camera);

void
Camera_Update(Camera *camera);

#endif
