#ifndef COURSE_H
#define COURSE_H

#include <gba.h>
#include <vector.h>
#include <hit.h>
#include <grid.h>

#include <game/object.h>
#include <game/camera.h>

typedef struct Course {
  Vector spawn, offset;
  int floor, count;
  Object objects[255];
  Cell grid;
  bool redraw;
} Course;

Course*
Course_GetInstance();

void
Course_Reset(Course *course);

static inline void
Course_SetSpawn(
    Course *course,
    int x, int y)
{
  course->spawn.x = x;
  course->spawn.y = y;
}

static inline Vector*
Course_GetSpawn(Course *course) {
  return &course->spawn;
}

static inline void
Course_SetFloorHeight(
    Course *course,
    int height)
{
  course->floor = height;
}

Object*
Course_AddObject(Course *course);

void
Course_Finalize(Course *course);

Hit
Course_CheckHits(
    Course *course,
    Bounds *hitbox);

void
Course_Draw(
    Course *course,
    Camera *camera);

#endif
