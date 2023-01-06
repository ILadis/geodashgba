#ifndef PROGRESS_H
#define PROGRESS_H

#include <gba.h>
#include <vector.h>
#include <math.h>

#include <game/cube.h>
#include <game/course.h>

typedef struct Progress {
  int previous, current;
  int total;
  bool redraw;
} Progress;

Progress*
Progress_GetInstance();

void
Progress_SetCourse(
    Progress *progress,
    Course *course);

void
Progress_Update(
    Progress *progress,
    Cube *cube);

void
Progress_Draw(Progress *progress);

#endif
