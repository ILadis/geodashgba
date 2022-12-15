#ifndef COURSE_H
#define COURSE_H

#include <gba.h>
#include <vector.h>
#include <hit.h>
#include <grid.h>

#include <game/chunk.h>
#include <game/object.h>
#include <game/loader.h>
#include <game/camera.h>

typedef struct Course {
  Vector offset, spawn;
  Loader *loader;
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
    Loader *loader);

static inline const Vector*
Course_GetSpawn(Course *course) {
  return &course->spawn;
}

Hit
Course_CheckHits(
    Course *course,
    Unit *unit,
    HitCallback callback);

void
Course_Draw(
    Course *course,
    Camera *camera);

#endif
