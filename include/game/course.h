#ifndef COURSE_H
#define COURSE_H

#include <gba.h>
#include <vector.h>
#include <hit.h>

#include <game/cube.h>
#include <game/camera.h>

typedef struct Course {
  Bounds floor;
  Bounds boxes[2];
  Vector scroll;
  bool redraw;
} Course;

Course*
Course_GetInstance();

void
Course_Reset(Course *course);

Hit
Course_CheckHits(
    Course *course,
    Cube *cube);

void
Course_Draw(
    Course *course,
    Camera *camera);

#endif
