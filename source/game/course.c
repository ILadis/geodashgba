
#include <game/course.h>

Course*
Course_GetInstance() {
  static Course course = {0};
  return &course;
}

static inline Chunk*
Course_LoadChunk(
    Course *course,
    int index)
{
  Chunk *chunk = Course_GetChunkAt(course, index);

  if (index == 0 || chunk->index != index) {
    Chunk_AssignIndex(chunk, index);
    Level_GetChunk(course->level, chunk);
  }

  return chunk;
}

static inline void
Course_CalculateBounds(Course *course) {
  Bounds bounds = Bounds_Of(120, 256, 120, 256);
  Level *level = course->level;

  int count = Level_GetChunkCount(level);
  bounds.center.x *= count;
  bounds.size.x *= count;

  Chunk last = {0};
  Chunk_AssignIndex(&last, count - 1);
  Level_GetChunk(level, &last);

  int dx = 0;
  Object *wall = Chunk_FindObjectByType(&last, TYPE_GOAL_WALL);
  if (wall != NULL) {
    const Bounds *bounds = Chunk_GetBounds(&last);

    Vector upper1 = Bounds_Upper(bounds);
    Vector upper2 = Bounds_Upper(&wall->hitbox);

    dx = (upper1.x - upper2.x) / 2;
  }

  bounds.center.x -= dx;
  bounds.size.x -= dx;

  course->bounds = bounds;
}

static inline void
Course_ResetChunks(Course *course) {
  const Chunk empty = {0};
  for (int i = 0; i < length(course->chunks); i++) {
    course->chunks[i] = empty;
  }
}

void
Course_ResetAndLoad(
    Course *course,
    Level *level)
{
  course->prepare.chunk = NULL;
  course->redraw = true;
  course->index = 0;
  course->level = level;

  Course_CalculateBounds(course);
  Course_ResetChunks(course);

  // load two chunks
  Chunk *current = Course_LoadChunk(course, 0);
  Course_LoadChunk(course, 1);

  const Bounds *bounds = Chunk_GetBounds(current);
  Vector floor = Bounds_Upper(bounds);

  course->floor = floor.y;
  course->spawn = Vector_Of(20, floor.y - 7);
}

void
Course_ResetTo(
    Course *course,
    const Vector *position)
{
  Chunk *current = Course_GetChunkAt(course, course->index);

  const Bounds *bounds = Chunk_GetBounds(current);
  int width = bounds->size.x * 2;

  int index = Math_div(position->x, width);
  Course_ResetChunks(course);

  // load two chunks
  Course_LoadChunk(course, index);
  Course_LoadChunk(course, index + 1);

  course->prepare.chunk = NULL;
  course->redraw = true;
  course->index = index;
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

    Object floor = {0};
    Object_CreateFloor(&floor);

    HitCallback_Invoke(callback, unit, &floor, &hit);
  }
}

void
Course_CheckHits(
    Course *course,
    Unit *unit,
    HitCallback callback)
{
  Course_CheckFloorHit(course, unit, callback);

  int index = course->index;

  Chunk *current = Course_GetChunkAt(course, index);
  Chunk_CheckHits(current, unit, callback);

  Chunk *next = Course_GetChunkAt(course, index + 1);
  Chunk_CheckHits(next, unit, callback);
}

static inline void
Course_DrawBackground(
    Course *course,
    Camera *camera)
{
  extern const GBA_TileMapRef courseBackgroundTileMap;
  static const GBA_BackgroundControl layer = {
    .size = 1, // 512x256
    .colorMode = 1,
    .tileSetIndex = 0,
    .tileMapIndex = 8,
    .priority = 3,
  };

  if (course->redraw) {
    GBA_EnableBackgroundLayer(0, layer);

    GBA_TileMapRef target;
    GBA_TileMapRef_FromBackgroundLayer(&target, 0);
    GBA_TileMapRef_Blit(&target, 0, 0, &courseBackgroundTileMap);
  }

  Vector *position = Camera_GetPosition(camera);

  GBA_OffsetBackgroundLayer(0, position->x >> 1, position->y >> 1);
  GBA_OffsetBackgroundLayer(1, position->x, position->y);
}

static inline const GBA_Tile*
Course_FloorTile(
    Course *course,
    int y)
{
  static const GBA_Tile empty = {0};
  static const GBA_Tile floor[] = {{ .tileId = 2 }, { .tileId = 10 }};

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
Course_DrawFloor(
    Course *course,
    Chunk *chunk,
    GBA_TileMapRef *target)
{
  const Bounds *bounds = Chunk_GetBounds(chunk);
  Vector lower = Bounds_Lower(bounds);
  Vector upper = Bounds_Upper(bounds);

  Vector_Rshift(&lower, 3);
  Vector_Rshift(&upper, 3);

  for (int y = 0; y < target->height; y++) {
    for (int x = lower.x; x < upper.x; x++) {
      const GBA_Tile *tile = Course_FloorTile(course, y);
      GBA_TileMapRef_BlitTile(target, x, y, tile);
    }
  }
}

static inline bool
Course_PrepareChunk(
    Course *course,
    Chunk *chunk)
{
  const int mapIndexes[] = { 10, 14 };
  int mapIndex = course->prepare.mapIndex;

  if (course->prepare.chunk != chunk) {
    course->prepare.chunk = chunk;
    course->prepare.step = -2;
    course->prepare.mapIndex = mapIndexes[chunk->index % 2];

    return false;
  }

  GBA_System *system = GBA_GetSystem();

  GBA_TileMapRef shadow = {
    .width = 64, .height = 64,
    .tiles = system->tileMaps[mapIndex]
  };

  int step = course->prepare.step++;
  switch (step) {
  case -2:
    GBA_Tile *tiles = system->tileMaps[mapIndex == 10 ? 14 : 10];
    GBA_Memcpy(shadow.tiles, tiles, sizeof(GBA_Tile) * shadow.width * shadow.height);
    break;

  case -1:
    Course_DrawFloor(course, chunk, &shadow);
    break;

  default:
    int count = chunk->count;
    if (step < count) {
      Object *object = &chunk->objects[step];
      Object_Draw(object, &shadow);
    }
    else return true;
  }

  return false;
}

static inline void
Course_DrawChunk(
    Course *course,
    Chunk *chunk)
{
  static GBA_BackgroundControl layer = {
    .size = 3, // 512x512
    .colorMode = 1,
    .tileSetIndex = 0,
    .priority = 2,
  };

  while (!Course_PrepareChunk(course, chunk));

  int mapIndex = course->prepare.mapIndex;
  layer.tileMapIndex = mapIndex;

  GBA_EnableBackgroundLayer(1, layer);
}

static inline void
Course_DrawChunks(Course *course) {
  int index = course->index;

  if (course->redraw) {
    Chunk *current = Course_GetChunkAt(course, index);
    Course_DrawChunk(course, current);

    Chunk *next = Course_GetChunkAt(course, index + 1);
    Course_DrawChunk(course, next);
  }
}

void
Course_Draw(
    Course *course,
    Camera *camera)
{
  int index = course->index;

  Chunk *current = Course_GetChunkAt(course, index);
  if (!Chunk_InViewport(current, camera)) {
    Chunk *pending = Course_GetChunkAt(course, index + 2);
    Course_DrawChunk(course, pending);

    course->index = ++index;
  }

  Chunk *next = Course_GetChunkAt(course, index + 1);
  if (Chunk_InViewport(next, camera)) {
    Chunk *pending = Course_LoadChunk(course, index + 2);
    Course_PrepareChunk(course, pending);
  }

  Course_DrawBackground(course, camera);
  Course_DrawChunks(course);

  course->redraw = false;
}
