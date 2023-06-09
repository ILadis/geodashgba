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
  Vector spawn;
  Bounds bounds;
  Level *level;
  Chunk chunks[4];
  // TODO consider keeping references to objects that need to be animated
  struct {
    Chunk *chunk;
    int mapIndex, step;
  } prepare;
  int floor, index;
  int frame;
  bool redraw;
} Course;

Course*
Course_GetInstance();

void
Course_ResetAndLoad(
    Course *course,
    Level *level);

void
Course_ResetTo(
    Course *course,
    const Vector *position);

static inline const Vector*
Course_GetSpawn(Course *course) {
  return &course->spawn;
}

static inline const Bounds*
Course_GetBounds(Course *course) {
  return &course->bounds;
}

static inline Chunk*
Course_GetChunkAt(
    Course *course,
    int index)
{
  index %= length(course->chunks);
  return &course->chunks[index];
}

static inline Chunk*
Course_GetCurrentChunk(Course *course) {
  return Course_GetChunkAt(course, course->index);
}

static inline Chunk*
Course_GetNextChunk(Course *course) {
  return Course_GetChunkAt(course, course->index + 1);
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
