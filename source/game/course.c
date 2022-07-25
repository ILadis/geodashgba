
#include <game/course.h>

Course*
Course_GetInstance() {
  static Course course = {0};
  return &course;
}

void
Course_Reset(Course *course) {
  course->redraw = true;
  course->count = 0;
  course->floor = 16;
  course->offset = Vector_Of(0, 0);
}

Object*
Course_AddObject(Course *course) {
  int index = course->count++;
  return &course->objects[index];
}

static inline Hit
Course_CheckFloorHit(
    Course *course,
    Bounds *hitbox)
{
  Hit hit = {0};

  int dy = (course->floor + 1)*8 - (hitbox->center.y + hitbox->size.y + 1);
  if (dy < 0) {
    hit.delta.y = dy;
  }

  return hit;
}

Hit
Course_CheckHits(
    Course *course,
    Cube *cube)
{
  Bounds *hitbox = Cube_GetHitbox(cube);

  Hit hit = Course_CheckFloorHit(course, hitbox);
  if (Hit_IsHit(&hit)) return hit;

  for (int i = 0; i < course->count; i++) {
    Hit hit = Bounds_Intersects(&course->objects[i].hitbox, hitbox);
    if (Hit_IsHit(&hit)) return hit;
  }

  return hit;
}

static inline const GBA_Tile*
Course_ClearTile(
    Course *course,
    int y)
{
  static const GBA_Tile empty = {0};
  static const GBA_Tile floor[] = {{ .tileId = 24 }, { .tileId = 10 }};

  int dy = y - course->floor;

  if (dy < 0) {
    return &empty;
  } else if (dy == 0) {
    return &floor[0];
  } else if (dy < 3) {
    return &floor[1];
  } else {
    return &empty;
  }
}

static inline void
Course_ClearColumn(
    Course *course,
    int x, int y)
{
  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  for (int row = 0; row < 32; row++, y++) {
    const GBA_Tile *tile = Course_ClearTile(course, y);
    GBA_TileMapRef_BlitTile(&target, x, y, tile);
  }
}

static inline void
Course_ClearRow(
    Course *course,
    int x, int y)
{
  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  const GBA_Tile *tile = Course_ClearTile(course, y);

  for (int col = 0; col < 32; col++, x++) {
    GBA_TileMapRef_BlitTile(&target, x, y, tile);
  }
}

static inline void
Course_Redraw(
    Course *course,
    Camera *camera)
{
  extern const GBA_TileMapRef backgroundTileMap;

  GBA_EnableBackgroundLayer(0, (GBA_BackgroundControl) {
    .size = 1,
    .colorMode = 1,
    .tileSetIndex = 0,
    .tileMapIndex = 8,
    .priority = 3,
  });

  GBA_EnableBackgroundLayer(1, (GBA_BackgroundControl) {
    .size = 0,
    .colorMode = 1,
    .tileSetIndex = 0,
    .tileMapIndex = 14,
    .priority = 2,
  });

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 0);
  GBA_TileMapRef_Blit(&target, 0, 0, &backgroundTileMap);

  for (int x = 0; x < 32; x++) {
    Course_ClearColumn(course, x, 0);

    for (int i = 0; i < course->count; i++) {
      Object *object = &course->objects[i];
      Object_Draw(object, camera);
    }
  }
}

static inline void
Course_DrawColumn(
    Course *course,
    Camera *camera)
{
  Vector *position = Camera_GetPosition(camera);
  Vector *delta = Camera_GetDelta(camera);

  if (delta->x != 0) {
    int x = (position->x >> 3) + (delta->x > 0 ? 30 : 0); // right/left most edge (not visible yet)
    int y = (position->y >> 3);

    Course_ClearColumn(course, x, y);

    for (int i = 0; i < course->count; i++) {
      Object *object = &course->objects[i];
      Object_Draw(object, camera);
    }
  }
}

static inline void
Course_DrawRow(
    Course *course,
    Camera *camera)
{
  Vector *position = Camera_GetPosition(camera);
  Vector *delta = Camera_GetDelta(camera);

  if (delta->y != 0) {
    int y = (position->y >> 3) + (delta->y > 0 ? 20 : 0); // top/bottom most edge (not visible yet)
    int x = (position->x >> 3);

    Course_ClearRow(course, x, y);

    for (int i = 0; i < course->count; i++) {
      Object *object = &course->objects[i];
      Object_Draw(object, camera);
    }
  }
}

static inline void
Course_DrawOffset(
    Course *course,
    Camera *camera)
{
  Vector *position = Camera_GetPosition(camera);

  course->offset.x = position->x;
  course->offset.y = position->y;

  GBA_OffsetBackgroundLayer(0, course->offset.x >> 1, course->offset.y >> 1);
  GBA_OffsetBackgroundLayer(1, course->offset.x, course->offset.y);
}

void
Course_Draw(
    Course *course,
    Camera *camera)
{
  if (course->redraw) {
    Course_Redraw(course, camera);
    course->redraw = false;
  }

  Course_DrawOffset(course, camera);

  Course_DrawColumn(course, camera);
  Course_DrawRow(course, camera);
}