#ifndef PROGRESS_H
#define PROGRESS_H

#include <gba.h>
#include <vector.h>
#include <math.h>

#include <game/cube.h>
#include <game/course.h>

typedef enum Mode {
  MODE_PLAY,
  MODE_SELECT,
} Mode;

typedef struct Progress {
  int previous, current;
  int best, total;
  bool coins[3];
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

void
Progress_SetProgress(
    Progress *progress,
    int value);

void
Progress_SetCollectedCoin(
    Progress *progress,
    unsigned int index);

void
Progress_SetCollectedCoins(
    Progress *progress,
    const bool *coins);

static inline int
Progress_GetBestProgress(Progress *progress) {
  return progress->best;
}

static inline bool*
Progress_GetCollectedCoins(Progress *progress) {
  return progress->coins;
}

void
Progress_Update(
    Progress *progress,
    Cube *cube);

void
Progress_Draw(Progress *progress);

#endif
