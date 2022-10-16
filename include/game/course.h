#ifndef COURSE_H
#define COURSE_H

#include <gba.h>
#include <vector.h>
#include <hit.h>
#include <grid.h>

#include <game/object.h>
#include <game/camera.h>

typedef struct Course {
  Vector offset;
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
Course_SetFloorHeight(
    Course *course,
    int height)
{
  course->floor = height;
}

Object*
Course_AddObject(Course *course);

Hit
Course_CheckHits(
    Course *course,
    Bounds *hitbox);

void
Course_Draw(
    Course *course,
    Camera *camera);

#endif
