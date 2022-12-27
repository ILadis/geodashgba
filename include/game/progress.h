#ifndef PROGRESS_H
#define PROGRESS_H

#include <game/camera.h>
#include <game/cube.h>
#include <game/level.h>

typedef struct Progress {
  int length;
  int value;
  bool redraw;
} Progress;

Progress*
Progress_GetInstance();

void
Progress_SetLevel(
    Progress *progress,
    Level *level);

void
Progress_Update(
    Progress *progress,
    Cube *cube);

void
Progress_Draw(
    Progress *progress,
    Camera *camera);

#endif
