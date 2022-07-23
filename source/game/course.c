
#include <game/course.h>

Course*
Course_GetInstance() {
  static Course course = {0};
  return &course;
}

void
Course_Reset(Course *course) {
  course->redraw = true;
  course->offset = Vector_Of(0, 0);

  course->floor = (Bounds) {
    .center = Vector_Of(120, 148),
    .size = Vector_Of(400, 13),
  };
}

Hit
Course_CheckHits(
    Course *course,
    Cube *cube)
{
  Bounds *hitbox = Cube_GetHitbox(cube);

  Hit hit = Bounds_Intersects(&course->floor, hitbox);
  if (Hit_IsHit(&hit)) return hit;

  for (int i = 0; i < ARRAY_LENGTH(course->objects); i++) {
    hit = Bounds_Intersects(&course->objects[i].hitbox, hitbox);
    if (Hit_IsHit(&hit)) return hit;
  }

  return hit;
}

static inline void
Course_ClearColumn(int x) {
  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  static const GBA_Tile empty = {0};
  static const GBA_Tile floor[] = {{ .tileId = 24 }, { .tileId = 10 }};

  for (int y = 0; y < 16; y++) {
    GBA_TileMapRef_BlitTile(&target, x, y, &empty);
  }

  GBA_TileMapRef_BlitTile(&target, x, 16, &floor[0]);
  GBA_TileMapRef_BlitTile(&target, x, 17, &floor[1]);
  GBA_TileMapRef_BlitTile(&target, x, 18, &floor[1]);
  GBA_TileMapRef_BlitTile(&target, x, 19, &floor[1]);
}

static inline void
Course_Redraw(Course *course) {
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
    Course_ClearColumn(x);

    for (int i = 0; i < ARRAY_LENGTH(course->objects); i++) {
      Object *object = &course->objects[i];
      Object_DrawColumn(object, x);
    }
  }
}

void
Course_Draw(
    Course *course,
    Camera *camera)
{
  if (course->redraw) {
    Course_Redraw(course);
    course->redraw = false;
  }

  Vector *position = Camera_GetPosition(camera);

  course->offset.x = position->x;
  course->offset.y = position->y;

  GBA_OffsetBackgroundLayer(0, course->offset.x, course->offset.y);
  GBA_OffsetBackgroundLayer(1, course->offset.x, course->offset.y);

  Vector *delta = Camera_GetDelta(camera);
  if (delta->x == 0) return; // camera did not move

  int x = (position->x / 8) + (delta->x > 0 ? 30 : -1); // right/left most edge (not visible yet)
  Course_ClearColumn(x);

  for (int i = 0; i < ARRAY_LENGTH(course->objects); i++) {
    Object *object = &course->objects[i];
    Object_DrawColumn(object, x);
  }
}