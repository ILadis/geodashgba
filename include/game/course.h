#ifndef COURSE_H
#define COURSE_H

#include <gba.h>
#include <vector.h>
#include <hit.h>
#include <grid.h>

#include <game/chunk.h>
#include <game/object.h>
#include <game/level.h>
#include <game/camera.h>

typedef struct Course {
  Vector offset, spawn;
  Level *level;
  Chunk chunks[2];
  Chunk *current, *next;
  int floor, index;
  bool redraw;
} Course;

Course*
Course_GetInstance();

void
Course_ResetAndLoad(
    Course *course,
    Level *level);

static inline const Vector*
Course_GetSpawn(Course *course) {
  return &course->spawn;
}

void
Course_CheckHits(
    Course *course,
    Unit *unit,
    HitCallback callback);

void
Course_Draw(
    Course *course,
    Camera *camera);

#endif
