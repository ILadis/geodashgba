#ifndef COURSE_H
#define COURSE_H

#include <gba.h>
#include <vector.h>
#include <hit.h>
#include <grid.h>
#include <text.h>
#include <counter.h>

#include <game/chunk.h>
#include <game/object.h>
#include <game/level.h>
#include <game/camera.h>

typedef struct Course {
  Vector spawn;
  Bounds bounds;
  Level *level;
  int index, floor;
  Chunk chunks[4];
  Text text;
  char attempts[4];
  enum {
    COURSE_READY_STATE_PREPARE = 1,
    COURSE_READY_STATE_PREPARE_NEXT = 2,
    COURSE_READY_STATE_WATING = 3,
    COURSE_READY_STATE_FINALIZE = 4,
  } state;
  bool (*prepare)(struct Course *course, Chunk *chunk, GBA_TileMapRef *target);
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

void
Course_IncreaseAttempts(Course *course);

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

bool
Course_AwaitReadyness(Course *course);

void
Course_Draw(
    Course *course,
    Camera *camera);

#endif
