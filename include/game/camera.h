#ifndef CAMERA_H
#define CAMERA_H

#include <gba.h>
#include <vector.h>
#include <hit.h>
#include <math.h>

typedef struct Camera {
  Vector position, delta;
  Bounds viewport, frame;
  Vector *target;
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

static inline Vector*
Camera_GetDelta(Camera *camera) {
  return &camera->delta;
}

// FIXME currently unused (consider removing camera->viewport as well)
static inline bool
Camera_InViewport(
    Camera *camera,
    Bounds *bounds)
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
    Vector *position)
{
  camera->target = position;
}

void
Camera_Update(Camera *camera);

void
Camera_DrawDelta(
    Camera *camera,
    const Vector *position,
    const GBA_TileMapRef *tileMap);

void
Camera_Draw(
    Camera *camera,
    const Vector *position,
    const GBA_TileMapRef *tileMap);

#endif
