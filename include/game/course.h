#ifndef COURSE_H
#define COURSE_H

#include <gba.h>
#include <vector.h>
#include <hit.h>

#include <game/cube.h>
#include <game/camera.h>

/*
typedef struct Object {
  Bounds hitbox;
  GBA_TileMapRef tiles;
} Object;

static const Object block1x2 = {
  .hitbox = ...
  .tiles = ...
};

static const Object spike = {
  .hitbox = ...
  .tiles = ...
};
*/

typedef struct Course {
  Bounds floor;
  Bounds boxes[3];
  Vector scroll;
  bool redraw;

/*
  int width, height;
  const Object *objects;
  const GBA_Tile *frames[]; // width many frames, each containing height tiles
*/
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
