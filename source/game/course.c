
#include <game/course.h>

Course*
Course_GetInstance() {
  static ewram Course course = {0};
  return &course;
}

static inline Chunk*
Course_LoadChunk(
    Course *course,
    unsigned int index)
{
  Chunk *chunk = Course_GetChunkAt(course, index);

  if (index == 0 || chunk->index != index) {
    Chunk_AssignIndex(chunk, index);
  }

  return chunk;
}

static inline void
Course_CalculateBounds(Course *course) {
  Level *level = course->level;
  int count = Level_GetChunkCount(level);

  Chunk chunk = {0};
  Chunk_AssignIndex(&chunk, count - 1);
  Level_GetChunk(level, &chunk);

  Bounds zero = {0};
  const Bounds *last = Chunk_GetBounds(&chunk);

  int dx = 0, dy = 16;
  Object *wall = Chunk_FindObjectByType(&chunk, OBJECT_TYPE_GOAL_WALL);
  if (wall != NULL) {
    Vector upper1 = Bounds_Upper(last);
    Vector upper2 = Bounds_Upper(&wall->hitbox);

    dx = (upper1.x - upper2.x) / 2;
  }

  Bounds bounds = Bounds_Expand(&zero, last);
  bounds.center.x -= dx;
  bounds.size.y += dy; // extra padding (for camera)
  bounds.size.x -= dx;
  course->bounds = bounds;
}

static inline void
Course_ResetChunks(Course *course) {
  const Chunk empty = {0};
  for (unsigned int i = 0; i < length(course->chunks); i++) {
    course->chunks[i] = empty;
  }
}

static inline void
Course_ResetState(
    Course *course,
    int index)
{
  course->prepare = NULL;
  course->frame = 0;
  course->redraw = true;
  course->index = index;
  course->state = COURSE_READY_STATE_PREPARE;
}

void
Course_ResetAndLoad(
    Course *course,
    Level *level)
{
  course->level = level;

  Counter counter = course->attempts;
  Counter_Reset(counter, length(course->attempts));

  Course_CalculateBounds(course);
  Course_ResetState(course, 0);
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

  unsigned int index = Math_div(position->x, width);
  Course_ResetChunks(course);
  Course_ResetState(course, index);

  // load two chunks
  Course_LoadChunk(course, index);
  Course_LoadChunk(course, index + 1);
}

void
Course_IncreaseAttempts(Course *course) {
  Counter counter = course->attempts;
  Counter_IncrementOne(counter);
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

  Chunk *current = Course_GetCurrentChunk(course);
  Chunk_CheckHits(current, unit, callback);

  Chunk *next = Course_GetNextChunk(course);
  Chunk_CheckHits(next, unit, callback);
}

static inline void
Course_AnimateObjects(Course *course) {
  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  Chunk *chunks[] = {
    Course_GetCurrentChunk(course),
    Course_GetNextChunk(course),
  };

  for (unsigned int i = 0; i < length(chunks); i++) {
    Chunk *chunk = chunks[i];

    unsigned int count = chunk->count;
    for (unsigned int j = 0; j < count; j++) {
      Object *object = &chunk->objects[j];
      Object_Animate(object, &target, course->frame);
    }
  }
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

static inline bool
Course_DrawObjects(
    Course *course,
    Chunk *chunk,
    GBA_TileMapRef *target)
{
  Object *object = Level_NextObject(course->level, chunk);
  if (object != NULL) {
    Object_Draw(object, target);
    return false;
  }

  return true;
}

static inline bool
Course_DrawAttempts(
    Course *course,
    Chunk *chunk,
    GBA_TileMapRef *target)
{
  extern const Font consoleFont;

  Text *text = &course->text;
  if (text->font == NULL) {
    // TODO use nicer font (probably "hudFont" but it's currenlty missing glyphs for digits)
    Text_SetFont(text, &consoleFont);

    int fill = GBA_Palette_FindColor(GBA_Color_From(0xffffff)); // actual color is 0xf8f8f8
    int outline = GBA_Palette_FindColor(GBA_Color_From(0x000008));

    Text_SetFillColor(text, fill);
    Text_SetOutlineColor(text, outline);
    Text_SetBackgroundColor(text, 0);
  }

  Counter counter = course->attempts;
  if (chunk->index == 0 && !Counter_IsBlank(counter)) {
    int tx = 2;
    int ty = course->floor/8 - 13;

    GBA_Tile tile = { .tileId = 129 };
    for (int y = 0; y < 2; y++) {
      for (int x = 0; x < 20; x++) {
        GBA_TileMapRef_BlitTile(target, tx + x, ty + y, &tile);
        GBA_TileMapRef_FillTile(target, tile.tileId++, 0);
      }
    }

    Text_SetCanvas(text, target);
    Text_SetCursor(text, tx * 8, ty * 8);
    Text_WriteLine(text, "Attempt ");
    Text_WriteLine(text, course->attempts);
  }

  course->prepare = Course_DrawObjects;
  return false;
}

static inline bool
Course_DrawFloor(
    Course *course,
    Chunk *chunk,
    GBA_TileMapRef *target)
{
  static const GBA_Tile empty = {0};
  static const GBA_Tile floor[] = {{ .tileId = 2 }, { .tileId = 10 }};

  const int count = sizeof(GBA_TileMap) / sizeof(GBA_Tile);
  int index = (chunk->index % 2) * count;

  GBA_Tile *top = &target->tiles[index];
  GBA_Tile *bottom = &target->tiles[index + count*2];

  int dy = course->floor/8 - 32;
  int size = 32 * sizeof(GBA_Tile);

  GBA_Memset32(top, empty.value, 32 * size);
  GBA_Memset16(&bottom[32 * 0], empty.value, dy * size);
  GBA_Memset16(&bottom[32 * dy++], floor[0].value, size);
  GBA_Memset16(&bottom[32 * dy], floor[1].value, (32 - dy) * size);

  course->prepare = Course_DrawAttempts;
  return false;
}

static inline bool
Course_PrepareChunk(
    Course *course,
    Chunk *chunk)
{
  GBA_System *system = GBA_GetSystem();
  GBA_TileMapRef target = {
    .width = 64, .height = 64,
    .tiles = system->tileMaps[14],
    .bitmaps = system->tileSets8[0],
  };

  if (course->prepare == NULL) {
    course->prepare = Course_DrawFloor;
  }

  return course->prepare(course, chunk, &target);
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
    .tileMapIndex = 10,
    .priority = 2,
  };

  while (!Course_PrepareChunk(course, chunk));
  course->prepare = NULL;

  GBA_System *system = GBA_GetSystem();
  GBA_EnableBackgroundLayer(1, layer);

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  GBA_Tile *source = system->tileMaps[14];
  GBA_Memcpy32(target.tiles, source, sizeof(GBA_Tile) * target.width * target.height);
}

static inline bool
Course_PrepareChunks(Course *course) {
  switch (course->state) {
  case COURSE_READY_STATE_PREPARE:
    Chunk *current = Course_GetCurrentChunk(course);
    if (Course_PrepareChunk(course, current)) {
      course->prepare = NULL;
      course->state++;
    }
    return false;
  case COURSE_READY_STATE_PREPARE_NEXT:
    Chunk *next = Course_GetNextChunk(course);
    if (Course_PrepareChunk(course, next)) {
      course->state++;
    }
    return false;
  case COURSE_READY_STATE_WATING:
    return false;
  case COURSE_READY_STATE_FINALIZE:
    Chunk *chunk = Course_GetNextChunk(course);
    Course_DrawChunk(course, chunk);

    course->state++;
    return false;
  }

  return true;
}

bool
Course_AwaitReadyness(Course *course) {
  switch (course->state) {
  case COURSE_READY_STATE_PREPARE:
  case COURSE_READY_STATE_PREPARE_NEXT:
    Course_PrepareChunks(course);
    return false;
  case COURSE_READY_STATE_WATING:
    course->state++;
    return true;
  case COURSE_READY_STATE_FINALIZE:
    return true;
  }

  return false;
}

void
Course_Draw(
    Course *course,
    Camera *camera)
{
  unsigned int index = course->index;

  if (Course_PrepareChunks(course)) {
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

    // FIXME animations are stopped while preparing next chunks
    Course_AnimateObjects(course);
  }

  Course_DrawBackground(course, camera);

  course->frame++;
  course->redraw = false;
}
