#ifndef PROGRESS_H
#define PROGRESS_H

#include <gba.h>
#include <vector.h>
#include <math.h>

#include <game/cube.h>
#include <game/course.h>

typedef enum Mode {
  MODE_PLAY,
  MODE_SELECTOR,
} Mode;

typedef struct Progress {
  int previous, current;
  int total;
  enum Mode mode;
  bool redraw;
} Progress;

Progress*
Progress_GetInstance();

void
Progress_SetCourse(
    Progress *progress,
    Course *course);

static inline void
Progress_SetMode(
    Progress *progress,
    Mode mode)
{
  progress->mode = mode;
  progress->redraw = true;
}

static inline void
Progress_SetProgress(
    Progress *progress,
    int value)
{
  progress->previous = progress->current;
  progress->current = Math_min(value, 1 << 8);
}

void
Progress_Update(
    Progress *progress,
    Cube *cube);

void
Progress_Draw(Progress *progress);

#endif
