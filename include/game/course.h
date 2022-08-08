#ifndef COURSE_H
#define COURSE_H

#include <gba.h>
#include <vector.h>
#include <hit.h>
#include <grid.h>

#include <game/cube.h>
#include <game/object.h>
#include <game/camera.h>

typedef struct Course {
  Vector offset;
  int floor, count;
  Object objects[30];
  Cell grid;
  bool redraw;
} Course;

Course*
Course_GetInstance();

void
Course_Reset(Course *course);

Object*
Course_AddObject(Course *course);

Hit
Course_CheckHits(
    Course *course,
    Cube *cube);

void
Course_Draw(
    Course *course,
    Camera *camera);

#endif
