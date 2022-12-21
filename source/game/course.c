
#include <game/course.h>

Course*
Course_GetInstance() {
  static Course course = {0};
  return &course;
}

static inline Chunk*
Course_LoadNextChunk(Course *course) {
  int index = course->index++;

  Chunk *current = course->current;
  Chunk *next = course->next;

  course->current = next;
  course->next = current;

  Chunk_AssignIndex(current, index);
  Loader_GetChunk(course->loader, current);

  return current;
}

void
Course_ResetAndLoad(
    Course *course,
    Loader *loader)
{
  course->redraw = true;
  course->index = 0;
  course->offset = Vector_Of(0, 0);

  Chunk *current = &course->chunks[0];
  Chunk *next = &course->chunks[1];

  course->current = current;
  course->next = next;

  if (loader != NULL) {
    course->loader = loader;
  }

  // load two chunks
  Course_LoadNextChunk(course);
  Course_LoadNextChunk(course);

  const Bounds *bounds = Chunk_GetBounds(current);
  Vector floor = Bounds_Upper(bounds);

  course->floor = floor.y - 1; // floor tile consists of 7 empty pixels and a 1 pixel line
  course->spawn = Vector_Of(20, floor.y);
}

static inline void
Course_CheckFloorHit(
    Course *course,
    Unit *unit,
    HitCallback callback)
{
  Bounds *hitbox = unit->bounds;
  Hit hit = {0};

  int dy  = course->floor - (hitbox->center.y + hitbox->size.y);
  if (dy < 0) {
    hit.delta.y = dy;

    Object floor = (Object) {
      .solid = true,
      .deadly = false,
    };

    HitCallback_Invoke(callback, unit, &floor, &hit);
  }
}

void
Course_CheckHits(
    Course *course,
    Unit *unit,
    HitCallback callback)
{
  Chunk_CheckHits(course->current, unit, callback);
  Chunk_CheckHits(course->next, unit, callback);
  Course_CheckFloorHit(course, unit, callback);
}

static inline void
Course_DrawBackground(
    Course *course,
    Camera *camera)
{
  extern const GBA_TileMapRef backgroundTileMap;
  static const GBA_BackgroundControl layers[] = {
   { // background layer
      .size = 1, // 512x256
      .colorMode = 1,
      .tileSetIndex = 0,
      .tileMapIndex = 8,
      .priority = 3,
    },
    { // objects layer
      .size = 3, // 512x512
      .colorMode = 1,
      .tileSetIndex = 0,
      .tileMapIndex = 14,
      .priority = 2,
    }
  };

  if (course->redraw) {
    GBA_EnableBackgroundLayer(0, layers[0]);
    GBA_EnableBackgroundLayer(1, layers[1]);

    GBA_TileMapRef target;
    GBA_TileMapRef_FromBackgroundLayer(&target, 0);
    GBA_TileMapRef_Blit(&target, 0, 0, &backgroundTileMap);
  }

  Vector *position = Camera_GetPosition(camera);

  course->offset.x = position->x;
  course->offset.y = position->y;

  GBA_OffsetBackgroundLayer(0, course->offset.x >> 1, course->offset.y >> 1);
  GBA_OffsetBackgroundLayer(1, course->offset.x, course->offset.y);
}

static inline const GBA_Tile*
Course_FloorTile(
    Course *course,
    int y)
{
  static const GBA_Tile empty = {0};
  static const GBA_Tile floor[] = {{ .tileId = 24 }, { .tileId = 10 }};

  int dy = y - course->floor/8;

  if (dy < 0) {
    return &empty;
  } else if (dy == 0) {
    return &floor[0];
  } else {
    return &floor[1];
  }
}

static inline void
Course_DrawChunk(
    Course *course,
    Chunk *chunk,
    Camera *camera)
{
  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  const Bounds *bounds = Chunk_GetBounds(chunk);
  Vector lower = Bounds_Lower(bounds);
  Vector upper = Bounds_Upper(bounds);

  Vector_Rshift(&lower, 3);
  Vector_Rshift(&upper, 3);

  for (int y = 0; y < target.height; y++) {
    for (int x = lower.x; x < upper.x; x++) {
      const GBA_Tile *tile = Course_FloorTile(course, y);
      GBA_TileMapRef_BlitTile(&target, x, y, tile);
    }
  }

  int count = chunk->count;
  for (int i = 0; i < count; i++) {
    Object *object = &chunk->objects[i];
    Object_Draw(object, &target);
  }
}

static inline void
Course_DrawChunks(
    Course *course,
    Camera *camera)
{
  Chunk *current = &course->chunks[0];
  Chunk *next = &course->chunks[1];

  if (course->redraw) {
    Course_DrawChunk(course, current, camera);
    Course_DrawChunk(course, next, camera);
  }
}

void
Course_Draw(
    Course *course,
    Camera *camera)
{
  Chunk *chunk = course->current;
  const Bounds *bounds = Chunk_GetBounds(chunk);

  if (!Camera_InViewport(camera, bounds)) {
    Chunk *next = Course_LoadNextChunk(course);
    Course_DrawChunk(course, next, camera);
  }

  Course_DrawBackground(course, camera);
  Course_DrawChunks(course, camera);

  course->redraw = false;
}
